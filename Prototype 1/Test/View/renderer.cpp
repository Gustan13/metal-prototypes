//
//  renderer.cpp
//  Test
//
//  Created by Gustavo Binder on 02/07/24.
//

#include "renderer.hpp"

Renderer::Renderer( MTL::Device* pDevice )
: _pDevice( pDevice->retain() )
{
    _pCommandQueue = _pDevice->newCommandQueue();
    buildMatrices();
    buildBuffers();
    buildShaders();
}

Renderer::~Renderer()
{
    _pCommandQueue->release();
    _pDevice->release();
    _pVertexPositionsBuffer->release();
    _pVertexColorsBuffer->release();
    _pPSO->release();
    
    delete uniforms;
}

void Renderer::buildMatrices() {
    
    simd::float4x4 translation;
    translation.columns[0] = { 1, 0, 0, 0 };
    translation.columns[1] = { 0, 1, 0, 0 };
    translation.columns[2] = { 0, 0, 1, 0 };
    translation.columns[3] = { 0, 0, 0, 1 };
    
    simd::float4x4 rotation;
    rotation.columns[0] = { (float)cos(0.4), 0, (float)sin(0.4), 0 };
    rotation.columns[1] = {        0,        1,               0, 0 };
    rotation.columns[2] = {-(float)sin(0.4), 0,(float)cos(0.4), 0 };
    rotation.columns[3] = {               0, 0,               0, 1 };
    
    uniforms->modelMatrix = translation;
    
    simd::float4x4 vtranslation;
    vtranslation.columns[0] = { 1, 0, 0, 0};
    vtranslation.columns[1] = { 0, 1, 0, 0};
    vtranslation.columns[2] = { 0, 0, 1, 0};
    vtranslation.columns[3] = { 0, 0, 20, 1};
    vtranslation = simd::inverse(vtranslation);
    
    uniforms->viewMatrix = vtranslation;
    
    simd::float4x4 projection;
    float y = 1 / tan(0.4 * 0.5);
    float x = y / 1;
    float z = 100 / (0.1 - 100);
    simd::float4 X = { x, 0, 0, 0 };
    simd::float4 Y = { 0, y, 0, 0 };
    simd::float4 Z = { 0, 0, z, -1 };
    simd::float4 W = { 0, 0, z * (float)(0.1), 0 };
    
    projection.columns[0] = X;
    projection.columns[1] = Y;
    projection.columns[2] = Z;
    projection.columns[3] = W;
    
    uniforms->projectionMatrix = projection;
}

void Renderer::buildBuffers() {
    const size_t NumVertices = 4;
    const size_t NumIndices  = 6;
    
    numVertices = NumVertices;
    numIndices = NumIndices;
    
    vertices = new simd::float3[NumVertices] {
        {-1,  1, 0},
        { 1,  1, 0},
        {-1, -1, 0},
        { 1, -1, 0}
    };
    
    indices = new UInt32[NumIndices] {
        0, 1, 3, 0, 3, 2
    };
    
    const size_t SizeOfVertexPositionsBuffer = sizeof(simd::float3) * NumVertices;
    const size_t SizeOfIndexBuffer = sizeof(UInt32) * NumIndices;
    
    _pVertexPositionsBuffer = _pDevice->newBuffer(SizeOfVertexPositionsBuffer, MTL::ResourceStorageModeManaged);
    _pIndexBuffer = _pDevice->newBuffer(SizeOfIndexBuffer, MTL::ResourceStorageModeManaged);
    
    memcpy(_pVertexPositionsBuffer->contents(), vertices, SizeOfVertexPositionsBuffer);
    memcpy(_pIndexBuffer->contents(), indices, SizeOfIndexBuffer);
    
    _pVertexPositionsBuffer->didModifyRange(NS::Range::Make(0, _pVertexPositionsBuffer->length()));
    _pIndexBuffer->didModifyRange(NS::Range::Make(0, _pIndexBuffer->length()));
}

void Renderer::buildShaders() {
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Error* pError = nullptr;
    MTL::Library* pLibrary = _pDevice->newDefaultLibrary();
    if ( !pLibrary )
    {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }

    MTL::Function* pVertexFn = pLibrary->newFunction( NS::String::string("vertexMain", UTF8StringEncoding) );
    MTL::Function* pFragFn = pLibrary->newFunction( NS::String::string("fragmentMain", UTF8StringEncoding) );

    MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    pDesc->setVertexFunction( pVertexFn );
    pDesc->setFragmentFunction( pFragFn );
    pDesc->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );
    pDesc->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);

    _pPSO = _pDevice->newRenderPipelineState( pDesc, &pError );
    if ( !_pPSO )
    {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }

    pVertexFn->release();
    pFragFn->release();
    pDesc->release();
    pLibrary->release();
}

void Renderer::draw( MTK::View* pView )
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

    time += 1 / (float)pView->preferredFramesPerSecond();
    simd::float4x4 rotMtx;
    rotMtx.columns[0] = { (float)cos(time), 0, (float)sin(time), 0 };
    rotMtx.columns[1] = {        0,        1,               0, 0 };
    rotMtx.columns[2] = {-(float)sin(time), 0, (float)cos(time), 0 };
    rotMtx.columns[3] = {               0, 0,               0, 1 };
    
    uniforms->modelMatrix = rotMtx;
    
    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
    
    pEnc->setRenderPipelineState(_pPSO);
    pEnc->setVertexBuffer(_pVertexPositionsBuffer, 0, 0);
    pEnc->setVertexBytes(uniforms, sizeof(Uniforms), 1);
//    pEnc->setTriangleFillMode(MTL::TriangleFillModeLines);
    
    pEnc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, numIndices, MTL::IndexTypeUInt32, _pIndexBuffer, 0);
    
    pEnc->endEncoding();
    
    pCmd->presentDrawable( pView->currentDrawable() );
    pCmd->commit();

    pPool->release();
}
