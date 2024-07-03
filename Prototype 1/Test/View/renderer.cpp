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
    
    model = new Model(pDevice);
    model->setModel("Models/Anime_charcter.dae");
    
    buildMatrices();
    buildShaders();
}

Renderer::~Renderer()
{
    _pCommandQueue->release();
    _pDevice->release();
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
    vtranslation.columns[3] = { 0, 1, 8, 1};
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
    
    MTL::DepthStencilDescriptor* pDSD = MTL::DepthStencilDescriptor::alloc()->init();
    pDSD->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
    pDSD->setDepthWriteEnabled(true);
    _pDSS = _pDevice->newDepthStencilState(pDSD);
    pDSD->release();

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
    pEnc->setCullMode(MTL::CullModeBack);
    pEnc->setFrontFacingWinding(MTL::WindingCounterClockwise);
    pEnc->setDepthStencilState(_pDSS);
//    pEnc->setTriangleFillMode(MTL::TriangleFillModeLines);
    model->render(pEnc, uniforms);
    
    pEnc->endEncoding();
    
    pCmd->presentDrawable( pView->currentDrawable() );
    pCmd->commit();

    pPool->release();
}
