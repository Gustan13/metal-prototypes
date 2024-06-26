//
//  mesh.hpp
//  Test2
//
//  Created by Gustavo Binder on 26/06/24.
//

#ifndef mesh_hpp
#define mesh_hpp

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <simd/simd.h>

class Mesh {
public:
    MTL::Buffer     *indexBuffer    = nullptr;
    UInt32          *indices        = nullptr;
    size_t          indexAmount     = 0;
};

#endif /* mesh_hpp */
