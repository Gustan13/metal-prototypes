//
//  model.cpp
//  Test
//
//  Created by Gustavo Binder on 02/07/24.
//

#include "model.hpp"

Model::Model(MTL::Device* device) {
    this->device = device;
    
    texture = new Texture(device);
    texture->setTexture("Models/textures.png");
}

void Model::setModel(const std::string filename) {
    Assimp::Importer importer;
    
    const aiScene* scene = importer.ReadFile(
                                             filename,
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate);
    assert(scene != nullptr);
    
    // CHECK IF MODEL ALREADY HAS MESHES
    // DELETES THEM IF IT DOES
    numMeshes = scene->mNumMeshes;
    if (meshes != nullptr)
        delete meshes;
    meshes = new Mesh*[numMeshes];
    
    // CHECK IF MODEL ALREADY HAS VERTICES
    // DELETES THEM IF IT DOES
    if (vertices != nullptr)
        delete vertices;
    for (int i = 0; i < scene->mNumMeshes; i++)
        numVertices += scene->mMeshes[i]->mNumVertices;
    vertices = new simd::float3[numVertices];
    
    if (textureVertices != nullptr)
        delete textureVertices;
    numTextVertices = numVertices;
    textureVertices = new simd::float2[numTextVertices];
    
    size_t vertexTrack = 0;
    
    for (int i = 0; i < scene->mNumMeshes; i++) {
        Mesh* newMesh = new Mesh;
        aiMesh* currentMesh = scene->mMeshes[i];
        
        newMesh->indexAmount = currentMesh->mNumFaces * 3;
        
        if (newMesh->indices != nullptr)
            delete newMesh->indices;
        
        newMesh->indices = new UInt32[newMesh->indexAmount];
        
        // ADD VERTICES
        for (int j = 0; j < currentMesh->mNumVertices; j++) {
            vertices[j + vertexTrack][0] = currentMesh->mVertices[j].x;
            vertices[j + vertexTrack][1] = currentMesh->mVertices[j].y;
            vertices[j + vertexTrack][2] = currentMesh->mVertices[j].z;
            
            if (currentMesh->HasTextureCoords(0)) {
                textureVertices[j + vertexTrack][0] = currentMesh->mTextureCoords[0][j].x;
                textureVertices[j + vertexTrack][1] = currentMesh->mTextureCoords[0][j].y;
            }
        }
        
        // ADD INDICES
        for (int j = 0; j < currentMesh->mNumFaces; j++) {
            aiFace* currentFace = &currentMesh->mFaces[j];
            
            for (int k = 0; k < 3; k++)
                newMesh->indices[j * 3 + k] = currentFace->mIndices[k] + (UInt32)vertexTrack;
        }
        
        vertexTrack += currentMesh->mNumVertices;
        
        // ADD MESH
        meshes[i] = newMesh;
    }
    
    buildBuffers();
}

void Model::buildBuffers() {
    const size_t sizeOfVertexBuffer = numVertices * sizeof(simd::float3);
    const size_t sizeOfTxtVtxBuffer = numTextVertices * sizeof(simd::float2);
    
    vertexBuffer = device->newBuffer(sizeOfVertexBuffer, MTL::ResourceStorageModeManaged);
    textureBuffer = device->newBuffer(sizeOfTxtVtxBuffer, MTL::ResourceStorageModeManaged);
    
    memcpy(vertexBuffer->contents(), vertices, sizeOfVertexBuffer);
    memcpy(textureBuffer->contents(), textureVertices, sizeOfTxtVtxBuffer);
    
    for (int i = 0; i < numMeshes; i++) {
        Mesh* currentMesh = meshes[i];
        const size_t sizeOfIndexBuffer = currentMesh->indexAmount * sizeof(UInt32);
        
        currentMesh->indexBuffer = device->newBuffer(sizeOfIndexBuffer, MTL::ResourceStorageModeManaged);
        memcpy(currentMesh->indexBuffer->contents(), currentMesh->indices, sizeOfIndexBuffer);
        
        currentMesh->indexBuffer->didModifyRange(NS::Range::Make( 0, currentMesh->indexBuffer->length() ));
    }
    
    vertexBuffer->didModifyRange(NS::Range::Make(0, vertexBuffer->length()));
    textureBuffer->didModifyRange(NS::Range::Make(0, textureBuffer->length()));
}

void Model::render(MTL::RenderCommandEncoder* pEnc, Uniforms* uniforms) {
    pEnc->setVertexBuffer(vertexBuffer, 0, 0);
    pEnc->setVertexBytes(uniforms, sizeof(Uniforms), 1);
    pEnc->setVertexBuffer(textureBuffer, 0, 2);
    
    pEnc->setFragmentTexture(texture->texture, 0);
    
    for (int i = 0; i < numMeshes; i++) {
        pEnc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, meshes[i]->indexAmount, MTL::IndexTypeUInt32, meshes[i]->indexBuffer, 0);
    }
}
