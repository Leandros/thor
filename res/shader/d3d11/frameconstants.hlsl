
#ifndef FRAME_CONSTANTS_H
#define FRAME_CONSTANTS_H

cbuffer FrameConstants : register(b0)
{
    float4x4 CameraWorldViewProj;
    float4x4 CameraWorldView;
    float4x4 CameraViewProj;
    float4x4 CameraProj;
    float4 CameraNearFar;
    uint4 FramebufferDimensions;
};

#endif /* FRAME_CONSTANTS_H */

