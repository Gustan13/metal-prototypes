//
//  Shaders.metal
//  Test
//
//  Created by Gustavo Binder on 02/07/24.
//

#include <metal_stdlib>
#import "Common.h"
using namespace metal;

struct v2f
{
    float4 position [[position]];
    float2 texturePosition;
};

v2f vertex vertexMain( uint vertexId [[vertex_id]],
                       device const float3* positions [[buffer(0)]],
                       constant Uniforms &uniforms [[buffer(1)]],
                       device const float2* texturePositions [[buffer(2)]])
{
    float4 fixedInPosition = float4( positions[ vertexId ], 1.0 );
    float4 position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * fixedInPosition;
    float2 texturePosition = texturePositions[ vertexId ];
    
    v2f o;
    o.position = position;
    o.texturePosition = texturePosition;
    return o;
}

float4 fragment fragmentMain( v2f in [[stage_in]], texture2d<float> texture [[texture(0)]] )
{
    constexpr sampler textureSampler (mag_filter::linear, min_filter::linear);
    const float4 colorSample = texture.sample(textureSampler, in.texturePosition);
    
    return colorSample;
}
