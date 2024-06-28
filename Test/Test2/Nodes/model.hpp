//
//  model.hpp
//  Test2
//
//  Created by Gustavo Binder on 10/06/24.
//

#ifndef model_hpp
#define model_hpp

#include <cassert>

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <simd/simd.h>

//#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "mesh.hpp"
#include "../Common.h"
//#include "assimp/postprocess.h"

struct Constants {
    float animateBy = 0.0f;
};

class Model {
public:
    Model( std::string name, MTL::Device* pDevice );
    void render( MTL::RenderCommandEncoder* pCommandEncoder, Uniforms* uniforms );
    void setModel(const aiScene* scene);
    void buildBuffers();
    
private:
    MTL::Buffer*    vertexBuffer = nullptr;
    MTL::Buffer*    colorBuffer  = nullptr;
    
    simd::float3*   vertices     = nullptr;
    Mesh**           meshes      = nullptr;
    
    size_t          vertexAmount = 0;
    size_t          meshAmount   = 0;
    
    MTL::Device*    pDevice      = nullptr;
};

#endif /* model_hpp */
