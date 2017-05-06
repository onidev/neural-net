
#include <SDL2/SDL.h>

#include <GL/gl.h>
#include <memory>
#include <cassert>
#include <iostream>

#include "texture.h"
#include "window.h"
#include "perceptron.h"

int main(int argc, char **argv)
{	
    Window window(640, 480, "Hi");
    Texture tex("simple_earth.png");
    Texture out(tex.wid, tex.hei);
    
    Perceptron<float> perceptron(2, 10, 10, 3);
    
    srand(time(0));
    perceptron.randomize();
    perceptron.gain = 0.5f;
    
    unsigned iteration = 0;
    auto iterate = [&]()
    {
        double global_mse = 0.0;
        double global_mae = 0.0;
        for(int i=0; i<128*128; ++i)
        {
            int x = rand() % 128;
            int y = rand() % 128;
            float rate = 0.01f / (1.f + iteration / 2000.f);
            perceptron.activate( {x/128.f, y/128.f} ); // activate == eval
            
            float r = tex(x, y).r / 255.f;
            float g = tex(x, y).g / 255.f;
            float b = tex(x, y).b / 255.f;
            
            //perceptron.learn({r, g, b}, 1.f - rate); // back propagation
            
            float mse, mae;
            perceptron.computeOutputError({r, g, b}, mse, mae);
            global_mse += mse;
            global_mae += mae;
            
            perceptron.backPropagateError();
            perceptron.adjustWeights(1.f - rate);
        }
        
        global_mse /= 128*128;
        global_mae /= 128*128;
        std::cout << global_mse << ' ' << global_mae << std::endl;
        
        ++iteration;
    };
    
	bool loop = true;
    while(loop)
    {
        SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			if(ev.type == SDL_QUIT)
				loop = false;

			if(ev.type == SDL_KEYDOWN)
			{
				switch (ev.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					loop = false;
					break;
				}
			}
		}
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        iterate();
        
        for(unsigned y=0; y<tex.hei; ++y)
        for(unsigned x=0; x<tex.wid; ++x)
        {
            perceptron.activate( {x/128.f, y/128.f} );
            auto output = perceptron.output();
            Color c( int(output[0]*255), int(output[1]*255), int(output[2]*255), 255);
            out.setPixel(x, y, c);
        }
        out.updatePixels();
        
        tex.draw(16, 16);
        out.draw(32 + tex.wid, 16);
        
        window.swap();
    }
	
    return 0;
}
