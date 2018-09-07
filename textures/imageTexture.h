#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H
#include "../hit.h"
#include "../texture.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#endif

class ImageTexture : public Texture {
  public:
    int width;
    int height;
    unsigned char* img;

    ImageTexture(const std::string& filename) {
      int n;
      img = stbi_load(filename.c_str(), &width, &height, &n, 3);
    };
    ~ImageTexture() {
      stbi_image_free(img);
    };

    RGB getColor(const Hit& res) const {
      int i = (int)(res.u*width);
      int j = (int)(res.v*height);
      int adr = 3*i + 3*width*j;
      return RGB(img[adr]/255.0f, img[adr+1]/255.0f, img[adr+2]/255.0f);
    };
};
#endif
