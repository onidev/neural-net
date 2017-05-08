#include "texture.h"
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <cassert>

Texture::Texture(const std::string& fname)
{
    surf.reset( IMG_Load(fname.c_str()) );
    if(surf == nullptr)
        throw std::runtime_error("Unable to load texture " + fname + ": " + std::string{IMG_GetError()});
    
    wid = surf->w;
    hei = surf->h;
    
    glGenTextures(1, &tex_id);
    updatePixels();
}

Texture::Texture(unsigned wid, unsigned hei):
    wid(wid), hei(hei)
{
    surf.reset( SDL_CreateRGBSurface(0, wid, hei, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000) );
    
    if(surf == nullptr)
        throw std::runtime_error("Unable to create empty texture");
    
    glGenTextures(1, &tex_id);
    updatePixels();
}

Texture::~Texture()
{
    if(tex_id != 0) {
        glDeleteTextures(1, &tex_id);
    }
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, tex_id);
}
void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::updatePixels()
{
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    unbind();
}

Color Texture::operator()(int i, int j) const
{
    const uint8_t* c = &reinterpret_cast<const uint8_t*>(surf->pixels)[i*4 + j*surf->pitch];
    return Color{c[0], c[1], c[2], c[3]};
}

void Texture::setPixel(int i, int j, const Color& in)
{
    // standard?
    Uint32 *target_pixel = (Uint32*)( (Uint8 *) surf->pixels + j * surf->pitch + i*4 );
    *target_pixel = in.r | in.g << 8 | in.b << 16 | in.a << 24;
}

void Texture::draw(float x, float y) const {
    bind();
    glBegin(GL_QUADS);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(x, y);
        
        glTexCoord2f(1.f, 0.f);
        glVertex2f(x + wid, y);
        
        glTexCoord2f(1.f, 1.f);
        glVertex2f(x + wid, y + hei);
        
        glTexCoord2f(0.f, 1.f);
        glVertex2f(x, y + hei);
    glEnd();
    unbind();
}
