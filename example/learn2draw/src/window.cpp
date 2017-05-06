#include "window.h"
#include <GL/gl.h>

int Window::instances = 0;

Window::Window(int wid, int hei, const std::string& caption)
{
    ++instances;
    if(instances == 1) {
        if(SDL_Init(SDL_INIT_VIDEO) < 0)
            throw std::runtime_error("Unable to init SDL Video");
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    window.reset(SDL_CreateWindow(caption.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  wid, hei, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL));
    if(!window)
        throw std::runtime_error("Unable to create window of caption " + caption);
    
    context.reset( SDL_GL_CreateContext(window.get()) );
    if(!context)
        throw std::runtime_error("Unable to create the OpenGL context");
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, wid, hei);
    glOrtho(0, wid, hei, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.6, 0.6, 0.6, 0);
}

Window::~Window() {
    --instances;
    if(instances == 0) {
        SDL_Quit();
    }
}

void Window::swap() {
    SDL_GL_SwapWindow(window.get());
}
