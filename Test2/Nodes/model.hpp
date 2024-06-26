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

struct Constants {
    float animateBy = 0.0f;
};

class Model {
public:
    Model( std::string name, MTL::Device* pDevice );
    void render( MTL::RenderCommandEncoder* pCommandEncoder, MTL::RenderPipelineState* _pPST );
    
private:
    MTL::Buffer*    vertexBuffer = nullptr;
    MTL::Buffer*    indexBuffer = nullptr;
    MTL::Buffer*    colorBuffer = nullptr;
    
    simd::float3*   vertices = nullptr;
    UInt16*         indices = nullptr;
    
    size_t          vertexAmount = 0;
    size_t          indexAmount = 0;
    
    MTL::Device*    pDevice = nullptr;
};

#endif /* model_hpp */
