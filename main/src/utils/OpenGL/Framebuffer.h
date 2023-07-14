#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../common.h"

class FrameBuffer
{
public:
    bool CreateFBO(GLint textureWidth, GLint textureHeight, GLuint &fbo, GLuint &rbo, GLuint &fbo_texture)
    {

        int maxRenderbufferSize;

        // CODE
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);

        // CHECK AVAILABLE BUFFER SIZE
        if (maxRenderbufferSize < textureWidth || maxRenderbufferSize < textureHeight)
        {
            PrintLog("UnSufficient Render Buffer Size...\n");
            return (false);
        }

        // CREATE FRAME BUFFER OBJECT
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // CREATE RENDER BUFFER OBJECT
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);

        // WHERE TO KEEP THIS RENDER BUFFER AND WHAT WILL BE THE FORMAT OF RENDER BUFFER
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT16, // NOT RELATED TO DEPTH - BUT 16 MACRO SIZE THIS ONE HENCE USED HERE
            textureWidth,
            textureHeight);

        // CREATE EMPTY TEXTURE
        glGenTextures(1, &fbo_texture);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            textureWidth,
            textureHeight,
            0,
            GL_RGB,
            GL_UNSIGNED_SHORT_5_6_5,
            NULL // NULL - EMPTY TEXTURE
        );

        // GIVE ABOVE TEXTURE TO FBO
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            fbo_texture,
            0 // MIPMAP LEVEL
        );

        // GIVE RENDER BUFFER TO FBO
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            rbo //
        );

        // CHECK WATHER FBO IS CREATED SUCCESSFULLY OR NOT
        GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (result != GL_FRAMEBUFFER_COMPLETE)
        {
            PrintLog("FRAME BUFFER IS NOT COMPLETE...\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // IMPLICITEL UNBINDS THE RBO AND TEXTURE BUFFER OBJECT

        return true;
    }
};

#endif