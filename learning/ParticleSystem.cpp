#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() {

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(MAX_BUFFER, vbo);

    static const GLfloat g_vertex_buffer_data[] = {
         -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         -0.5f, 0.5f, 0.0f,
         0.5f, 0.5f, 0.0f,
    };

    glBufferData(vao, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(vao, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(vao, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

ParticleSystem::~ParticleSystem()
{
    glDeleteVertexArrays(1, &vao);			//Delete our VAO
    glDeleteBuffers(MAX_BUFFER, vbo);	//Delete our VBOs
}

void ParticleSystem::ParticleUpdate(float dt)
{
    int newparticles = (int)(dt * 10000.0);

    if (newparticles > (int)(0.016f * 10000.0)) {
        newparticles = (int)(0.016f * 10000.0);
    }
       
}
