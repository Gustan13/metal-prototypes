//
//  model.cpp
//  Test2
//
//  Created by Gustavo Binder on 10/06/24.
//

#include "model.hpp"

Model::Model( std::string name, MTL::Device* pDevice) {
    const size_t NumVertices =  4;
    const size_t NumIndices = 6;
    
    this->pDevice = pDevice;
    
    simd::float3 vertices[NumVertices] = {
        {-1, 1, 0},
        { 1, 1, 0},
        {-1, 0, 0},
        { 1, 0, 0}
    };
    
    simd::float3 colors[NumVertices] =
    {
        {  1.0, 0.3f, 0.2f },
        {  0.8f, 1.0, 0.0f },
        {  0.8f, 0.0f, 1.0 },
        {  1.0, 0.3f, 0.2f }
    };
    
    UInt16 indices[NumIndices] = {
        0, 1, 3, 0, 3, 2
    };
    
    const size_t sizeOfVertexBuffer = NumVertices * sizeof(simd::float3);
    const size_t sizeOfColorBuffer = NumVertices * sizeof(simd::float3);
    const size_t sizeOfIndexBuffer = NumIndices * sizeof(UInt16);
    
    vertexBuffer = this->pDevice->newBuffer(sizeOfVertexBuffer, MTL::ResourceStorageModeManaged);
    indexBuffer = this->pDevice->newBuffer(sizeOfIndexBuffer, MTL::ResourceStorageModeManaged);
    colorBuffer = this->pDevice->newBuffer(sizeOfColorBuffer, MTL::ResourceStorageModeManaged);
    
    memcpy(vertexBuffer, vertices, sizeOfVertexBuffer);
    memcpy(indexBuffer, indices, sizeOfIndexBuffer);
    memcpy(colorBuffer, colors, sizeOfColorBuffer);
    
    vertexBuffer->didModifyRange( NS::Range::Make( 0, vertexBuffer->length() ) );
    colorBuffer->didModifyRange( NS::Range::Make( 0, colorBuffer->length() ) );
    indexBuffer->didModifyRange(NS::Range::Make(0, indexBuffer->length()));
}

void Model::render( MTL::RenderCommandEncoder* pCommandEncoder, MTL::RenderPipelineState* _pPST ) {
    
}

void Model::setModel(aiScene* scene) {
    if (scene == nullptr)
        return;
    
    aiMesh* meshes = *scene->mMeshes;
    
    meshAmount = scene->mNumMeshes;
    
    for (int i = 0; i < scene->mNumMeshes; i++) {
        Mesh* newMesh = new Mesh;
        aiMesh* currentMesh = scene->mMeshes[i];
        
        vertexAmount = currentMesh->mNumVertices;
        newMesh->indexAmount = currentMesh->mNumFaces * 3;
        newMesh->indices = new UInt32[newMesh->indexAmount];
        
        for (int j = 0; j < currentMesh->mNumVertices; j++) {
            vertices[j + vertexAmount][0] = currentMesh->mVertices[j].x;
            vertices[j + vertexAmount][1] = currentMesh->mVertices[j].y;
            vertices[j + vertexAmount][2] = currentMesh->mVertices[j].z;
        }
        
        for (int j = 0; j < currentMesh->mNumFaces; j++) {
            aiFace* currentFace = &currentMesh->mFaces[j];
            
            for (int k = 0; k < 3; k++)
                newMesh->indices[j + k] = currentFace->mIndices[k];
        }
    }
}

void Model::buildBuffers() {
    const size_t sizeOfVertexBuffer = vertexAmount * sizeof(simd::float3);
    
    
//    const size_t sizeOfIndexBuffer = vertexAmount * sizeof(UInt32);
}
