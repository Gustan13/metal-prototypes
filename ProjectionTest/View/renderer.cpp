//
//  renderer.cpp
//  Test2
//
//  Created by Gustavo Binder on 10/06/24.
//

#include "renderer.hpp"

Renderer::Renderer( MTL::Device* pDevice )
: _pDevice( pDevice->retain() )
{
    _pCommandQueue = _pDevice->newCommandQueue();
    importModel("Models/train.obj");
    buildMatrices();
    buildShaders();
    buildBuffers();
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

void Renderer::buildBuffers()
{
    simd::float3 colors[4] =
    {
        {  1.0, 0.0f, 0.0f },
        {  0.8f, 1.0, 0.0f },
        {  0.8f, 0.0f, 1.0 },
        {  1.0, 0.3f, 0.2f }
    };
    
    for (int i = 0; i < numI; i++) {
        printf("%u\n", indices[i]);
    }
    
    const size_t positionsDataSize = sizeof(simd::float3) * numV;
    const size_t colorsDataSize = sizeof(simd::float3) * 4;
    const size_t indicesSize = sizeof(UInt32) * numI;
    
    _pVertexPositionsBuffer = _pDevice->newBuffer(positionsDataSize, MTL::ResourceStorageModeManaged);
    _pVertexColorsBuffer = _pDevice->newBuffer(colorsDataSize, MTL::ResourceStorageModeManaged);
    _pIndexBuffer = _pDevice->newBuffer(indicesSize, MTL::ResourceStorageModeManaged);
    
    memcpy(_pVertexPositionsBuffer->contents(), vertices, positionsDataSize);
    memcpy(_pVertexColorsBuffer->contents(), colors, colorsDataSize);
    memcpy(_pIndexBuffer->contents(), indices, indicesSize);
    
    _pVertexPositionsBuffer->didModifyRange( NS::Range::Make( 0, _pVertexPositionsBuffer->length() ) );
    _pVertexColorsBuffer->didModifyRange( NS::Range::Make( 0, _pVertexColorsBuffer->length() ) );
    _pIndexBuffer->didModifyRange(NS::Range::Make(0,_pIndexBuffer->length()));
    
    delete(vertices);
    delete(indices);
}

void Renderer::buildShaders()
{
    using NS::StringEncoding::UTF8StringEncoding;

    const char* shaderSrc = R"(
        #include <metal_stdlib>
        using namespace metal;

        struct Uniforms {
            float4x4 modelMatrix;
            float4x4 viewMatrix;
            float4x4 projectionMatrix;
        };
    
        struct v2f
        {
            float4 position [[position]];
            half3 color;
        };

        v2f vertex vertexMain( uint vertexId [[vertex_id]],
                               device const float3* positions [[buffer(0)]],
                               device const float3* colors [[buffer(1)]],
                                constant Uniforms &uniforms[[buffer(2)]])
        {
            float4 fixedInPosition = float4( positions[ vertexId ], 1.0);
            float4 position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * fixedInPosition;
            v2f o;
            o.position = position;
            o.color = half3 ( colors[ 0 ] );
            return o;
        }

        half4 fragment fragmentMain( v2f in [[stage_in]] )
        {
            return half4( in.color, 1.0 );
        }
    )";

    NS::Error* pError = nullptr;
    MTL::Library* pLibrary = _pDevice->newLibrary( NS::String::string(shaderSrc, UTF8StringEncoding), nullptr, &pError );
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

void Renderer::buildMatrices() {
    
    simd::float4x4 translation;
    translation.columns[0] = { 1, 0, 0, 0 };
    translation.columns[1] = { 0, 1, 0, 0 };
    translation.columns[2] = { 0, 0, 1, 0 };
    translation.columns[3] = { 0, 0.3, 0, 1 };
    
    simd::float4x4 rotation;
    rotation.columns[0] = { (float)cos(0.4), 0, (float)sin(0.4), 0 };
    rotation.columns[1] = {        0,        1,               0, 0 };
    rotation.columns[2] = {-(float)sin(0.4), 0,-(float)cos(0.4), 0 };
    rotation.columns[3] = {               0, 0,               0, 1 };
    
    uniforms->modelMatrix = translation;
    
    simd::float4x4 vtranslation;
    vtranslation.columns[0] = { 1, 0, 0, 0};
    vtranslation.columns[1] = { 0, 1, 0, 0};
    vtranslation.columns[2] = { 0, 0, 1, 0};
    vtranslation.columns[3] = { 0, 0, 3, 1};
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

void Renderer::draw( MTK::View* pView )
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

    time += 1 / (float)pView->preferredFramesPerSecond();
    simd::float4x4 rotMtx;
    rotMtx.columns[0] = { (float)cos(time), 0, (float)sin(time), 0 };
    rotMtx.columns[1] = {        0,        1,               0, 0 };
    rotMtx.columns[2] = {-(float)sin(time), 0,(float)cos(time), 0 };
    rotMtx.columns[3] = {               0, 0.35,               0, 1 };
    
    uniforms->modelMatrix = rotMtx;
    
    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
    
    pEnc->setRenderPipelineState(_pPSO);
    pEnc->setVertexBuffer(_pVertexPositionsBuffer, 0, 0);
    pEnc->setVertexBuffer(_pVertexColorsBuffer, 0, 1);
    
    pEnc->setVertexBytes(uniforms, sizeof(Uniforms), NS::UInteger(2));
    
//    pEnc->setTriangleFillMode(MTL::TriangleFillModeLines);
    
    pEnc->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, numI, MTL::IndexTypeUInt32, _pIndexBuffer, NS::UInteger(0));
//    pEnc->drawIndexe

//    pEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeLine, NS::UInteger(0), NS::UInteger(numV));
    
    pEnc->endEncoding();
    
    pCmd->presentDrawable( pView->currentDrawable() );
    pCmd->commit();

    pPool->release();
}

void Renderer::importModel(const std::string pFile) {
    Assimp::Importer importer;
    
    const aiScene* scene = importer.ReadFile(
                                             pFile,
                                             aiProcess_CalcTangentSpace       |
                                                 aiProcess_Triangulate);
    
    if (scene == nullptr) {
        printf("Failed to import train\n");
        return;
    }
    
    train = scene;
    
    aiMesh* meshes               = *train->mMeshes;
    aiVector3t<float>* mvertices = meshes->mVertices;
    aiFace* faces                = meshes->mFaces;
    
    int numIndices               = 0;
    int numVertices              = meshes->mNumVertices;
    
    simd::float3 *returnVertices = new simd::float3[numVertices];
    UInt32 *returnIndices = new UInt32[meshes->mNumFaces * 3];
    
    for (int i = 0; i < numVertices; i++) {
        returnVertices[i][0] = mvertices[i].x / 10;
        returnVertices[i][1] = mvertices[i].y / 10;
        returnVertices[i][2] = mvertices[i].z / 10;
    }
    
    for (int i = 0; i < meshes->mNumFaces; i++) {
        const unsigned int indexNumInFace = faces[i].mNumIndices;
        for (int j = 0; j < indexNumInFace; j++) {
            returnIndices[numIndices] = faces[i].mIndices[j];
            numIndices++;
        }
    }
    
    vertices = returnVertices;
    indices = returnIndices;
    
    numV = numVertices;
    numI = meshes->mNumFaces * 3;
    
    printf("Train has been imported\n");
    
    for (int i = 0; i < numV; i++) {
        printf("%f %f %f\n", vertices[i][0], vertices[i][1], vertices[i][2]);
    }
}
