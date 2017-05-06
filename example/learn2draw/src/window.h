#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <string>

// window with opengl context
struct Window
{
    struct DestroyWindow_Functor {
        void operator() (SDL_Window *w) const {
          SDL_DestroyWindow(w);
        }
        void operator() (SDL_GLContext context) const {
          SDL_GL_DeleteContext(context);
        }
    };
    using Window_ptr = std::unique_ptr<SDL_Window, DestroyWindow_Functor>;
    using Context_ptr = std::unique_ptr<void, DestroyWindow_Functor>;
    
    static int  instances;
    Window_ptr  window;
    Context_ptr context;
    
    Window(int wid, int hei, const std::string& caption);
    ~Window();
    
    void swap();
};


