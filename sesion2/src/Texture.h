#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <ogl/glew.h>
#include <freeimage/freeImage.h>

class Texture {
    
    public:
               
        unsigned int initTexture(const char *textureFile); 

        virtual ~Texture();
           
    private:
       
        unsigned int  texture;
        
        unsigned char *loadTexture(const char *textureFile, unsigned int &w, unsigned int &h);
        
};

#endif /* TEXTURE_H */
