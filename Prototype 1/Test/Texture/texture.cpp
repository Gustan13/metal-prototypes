//
//  texture.cpp
//  Test
//
//  Created by Gustavo Binder on 03/07/24.
//

#include "texture.hpp"

Texture::Texture(MTL::Device* device) {
    this->device = device;
}

Texture::~Texture() {
    texture->release();
}

void Texture::setTexture(const char* filepath) {
    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* image = stbi_load(filepath, &width, &height, &channels, 0);
    assert(image != NULL);
    
    MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
    textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
    textureDescriptor->setWidth(width);
    textureDescriptor->setHeight(height);
    
    texture = device->newTexture(textureDescriptor);
    
    MTL::Region region = MTL::Region(0, 0, 0, width, height, 1);
    NS::UInteger bytesPerRow = 4 * width;
    
    texture->replaceRegion(region, 0, image, bytesPerRow);
    
    textureDescriptor->release();
    stbi_image_free(image);
}
