//
//  texture.hpp
//  Test
//
//  Created by Gustavo Binder on 03/07/24.
//

#ifndef texture_hpp
#define texture_hpp

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include "stb_image.h"

class Texture {
public:
    Texture (MTL::Device* device);
    ~Texture();
    void setTexture(const char* filepath);
    
    MTL::Texture* texture;
    int width, height, channels;
private:
    MTL::Device* device;
};

#endif /* texture_hpp */
