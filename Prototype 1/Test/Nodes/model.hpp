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

class Model {
public:
    Model(MTL::Device* device);
    void setModel(const std::string filename);
    void render(MTL::RenderCommandEncoder* pEnc, Uniforms* uniforms);
private:
    MTL::Device* device;
    MTL::Buffer* vertexBuffer;
    MTL::Buffer* indexBuffer;
    
    Mesh** meshes;
    simd::float3* vertices;
    
    size_t numMeshes = 0;
    size_t numVertices = 0;
    
    void buildBuffers();
};

#endif /* model_hpp */
