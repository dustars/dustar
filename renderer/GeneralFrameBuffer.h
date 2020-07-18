/*
    Description:
    A framebuffer object with a single color attachment and depth attachment.
*/


#pragma once
#include "FrameBuffer.h"
class GeneralFrameBuffer final :
    public FrameBuffer 
{
public:
    GeneralFrameBuffer(int width, int height);
    ~GeneralFrameBuffer() override;

    GeneralFrameBuffer(const GeneralFrameBuffer&) = delete;
    GeneralFrameBuffer& operator=(const GeneralFrameBuffer&) = delete;

    GLuint GetFrameBuffer()     const override { return frameBuffer; }
    GLuint GetColorTexture()    const override { return colorTexture; }
    GLuint GetDepthTexture()    const override { return depthTexture; }

protected:
    GLuint frameBuffer;
    GLuint colorTexture;
    GLuint depthTexture;

    static const GLenum draw_buffers[];
};

