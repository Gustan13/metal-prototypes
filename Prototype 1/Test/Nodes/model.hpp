//
//  model.hpp
//  Test
//
//  Created by Gustavo Binder on 02/07/24.
//

#ifndef model_hpp
#define model_hpp

#include <Metal/Metal.hpp>
#include <simd/simd.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hpp"
#include "Common.h"
#include "Texture/texture.hpp"

class Model {
public:
    Model(MTL::Device* device);
    void setModel(const std::string filename);
    void render(MTL::RenderCommandEncoder* pEnc, Uniforms* uniforms);
private:
    MTL::Device* device;
    MTL::Buffer* vertexBuffer;
    MTL::Buffer* textureBuffer;
    
    Mesh** meshes;
    simd::float2* textureVertices;
    simd::float3* vertices;
    
    size_t numMeshes = 0;
    size_t numVertices = 0;
    size_t numTextVertices = 0;
    
    Texture* texture = nullptr;
    
    void buildBuffers();
};

#endif /* model_hpp */
