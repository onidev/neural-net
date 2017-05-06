#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <cstdint>

struct Color
{
    uint8_t r, g, b, a;
    
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a):
        r(r),g(g),b(b),a(a) {}
};

struct Texture 
{
    struct FreeSurface_Functor {
      void operator() (SDL_Surface *s) const {
        SDL_FreeSurface(s);
      }
    };
    using Surface_ptr = std::unique_ptr<SDL_Surface, FreeSurface_Functor>;
    
    Surface_ptr surf;
    unsigned tex_id = 0;
    unsigned wid = 0;
    unsigned hei = 0;
    
    Texture(const std::string& fname);
    Texture(unsigned wid, unsigned hei);
    ~Texture();
    
    void bind() const;
    void unbind() const;
    void updatePixels(); // send pixels to vram for rendering
    
    Color operator()(int i, int j) const;
    void setPixel(int i, int j, const Color& c);

    void draw(float x, float y) const;
};

