#ifndef TEXTURE_H
#define TEXTURE_H

#include "image.hpp"


class Texture {
public:
    Texture(const char *filename) 
    {
        img = Image::LoadBMP(filename);
        w = img->Width();
        h = img->Height();
    }
    virtual ~Texture() {delete img;}
    inline int getwidth() const {return w;}
    inline int getheight() const {return h;}
    Vector3f getTexel(const int x, const int y)  {
        assert(0 <= x && x < w && 0 <= y && y < h);
        return img->GetPixel(x, y);
    }
protected:
    Image *img;
    int w, h;
};

#endif // TEXTURE