
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
    Window window(640, 480, "Perceptron - Learn to draw");
    
    Texture tex_input("data/simple_earth.png");
    
    float output_scale = 1.0f;
    Texture tex_output(tex_input.wid * output_scale, tex_input.hei * output_scale);
    
    Perceptron<float> perceptron(2, 10, 10, 3);
    if(!perceptron.load("data.txt"))
    {
        srand(time(0));
        perceptron.randomize();
        perceptron.gain = 0.5f;
    }
    
    unsigned iteration = 0;
    auto iterate = [&]()
    {
        double global_mse = 0.0;
        double global_mae = 0.0;
        for(int i=0; i<tex_input.wid*tex_input.hei; ++i)
        {
            int x = rand() % tex_input.wid;
            int y = rand() % tex_input.hei;
            float rate = 0.01f / (1.f + iteration / 2000.f);
            perceptron.activate( {x/(float)tex_input.wid, y/(float)tex_input.hei} ); // activate == eval
            
            float r = tex_input(x, y).r / 255.f;
            float g = tex_input(x, y).g / 255.f;
            float b = tex_input(x, y).b / 255.f;
            
            //perceptron.learn({r, g, b}, 1.f - rate); // back propagation
            
            float mse, mae;
            perceptron.computeOutputError({r, g, b}, mse, mae);
            global_mse += mse;
            global_mae += mae;
            
            perceptron.backPropagateError();
            perceptron.adjustWeights(1.f - rate);
        }
        
        global_mse /= tex_input.wid*tex_input.hei;
        global_mae /= tex_input.wid*tex_input.hei;
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
        
        for(unsigned y=0; y<tex_output.hei; ++y)
        for(unsigned x=0; x<tex_output.wid; ++x)
        {
            perceptron.activate( {x/(float)tex_output.wid, y/(float)tex_output.hei} );
            auto output = perceptron.output();
            Color c( int(output[0]*255), int(output[1]*255), int(output[2]*255), 255);
            tex_output.setPixel(x, y, c);
        }
        tex_output.updatePixels();
        
        tex_input.draw(16, 16);
        tex_output.draw(32 + tex_input.wid, 16);
        
        window.swap();
    }
    
    perceptron.save("data.txt");
	
    return 0;
}
