/*
	Description:
	Abstraction for FBO

    TODO:
    1. The initialization of static member draw_buffers[]?
*/


#pragma once
#include <GL/glew.h>
#include <iostream>

/*
    Govern the type of FBO.
    General:    1 color buffer + 1 depth buffer
    Depth:      1 depth buffer
    Multiple:   > 1 color buffers + 1 depth buffer (Not implemented)
    (More?)
*/
enum class FBOCreationType {GENERAL, DEPTH, MULTIPLE};

class FrameBuffer
{
public:
    FrameBuffer(int width, int height, FBOCreationType type = FBOCreationType::GENERAL);
    ~FrameBuffer();
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    GLuint GetFrameBuffer()     const { return frameBuffer; }
    GLuint GetColorTexture()    const { return colorTexture; }
    GLuint GetDepthTexture()    const { return depthTexture; }

private:
    int width, height;
    FBOCreationType FBOtype;

    GLuint frameBuffer  = 0;
    GLuint colorTexture = 0;
    GLuint depthTexture = 0;

    void GeneralFBO();
    void DepthOnlyFBO();

    static const GLenum draw_buffers[];
};
