//
//  renderer.hpp
//  Test2
//
//  Created by Gustavo Binder on 10/06/24.
//

#ifndef renderer_hpp
#define renderer_hpp

#include <cassert>

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <simd/simd.h>
#include "../Nodes/model.hpp"
#include "../Common.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class Renderer
{
public:
    Renderer( MTL::Device* pDevice );
    ~Renderer();
    void draw( MTK::View* pView );
    void buildBuffers();
    void buildShaders();
    void buildMatrices();
    
    void importModel( const std::string pFile );
    
private:
    MTL::Device* _pDevice;
    MTL::CommandQueue* _pCommandQueue;
    MTL::RenderPipelineState* _pPSO;
    MTL::Buffer* _pVertexPositionsBuffer;
    MTL::Buffer* _pVertexColorsBuffer;
    MTL::Buffer* _pIndexBuffer;
    Constants constants;
    float time = 0.0f;
    const aiScene* train = nullptr;
    
    simd::float3* vertices = nullptr;
    UInt32* indices        = nullptr;
    
    int numV = 0;
    int numI = 0;
    
    Uniforms *uniforms = new Uniforms;
};

#endif /* renderer_hpp */
