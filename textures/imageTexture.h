#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H
#include "../hit.h"
#include "../texture.h"
#include "../util.h"

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
      std::cout << "Loading Image" << std::endl;
      std::cout << "path: " << filename.c_str() << std::endl;
      if(!img) {
        std::cout << "Loading Image Failed" << std::endl;
        std::cout << "channels: " << n << std::endl;
        std::cout << stbi_failure_reason() << std::endl;
        std::exit(1);
      }
    };
    ~ImageTexture() {
      stbi_image_free(img);
    };

    RGB getColor(const Hit& res) const {
      int i = clamp((int)(res.u*width), 0, width - 1);
      int j = clamp((int)(res.v*height), 0, height - 1);
      int adr = 3*i + 3*width*j;
      return RGB(img[adr]/255.0f, img[adr+1]/255.0f, img[adr+2]/255.0f);
    };
};
#endif
