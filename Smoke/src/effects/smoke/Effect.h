#ifndef _EFFECT_H
#define _EFFECT_H

#include "../../utils/common.h"
#include "../../utils/Camera.h"
#include "../../utils/OpenGL/dds.h"
#include "SortParticles.h"
#include "Smoke.h"

// inline OpenGLContentRegistry& OpenGLContentManager() {
// 	return OpenGLContentRegistry::Instance();
// }

GLint map_Format_Internal[] = {
    0,
    GL_R8,
    GL_RG8,
    GL_RGB8,
    GL_SRGB8,
    GL_RGB8,
    GL_SRGB8,
    GL_RGBA8,
    GL_SRGB8_ALPHA8,
    GL_RGBA8,
    GL_SRGB8_ALPHA8,

    GL_DEPTH24_STENCIL8,
    GL_DEPTH_COMPONENT32F,

    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,

    GL_R16F,
    GL_RG16F,
    GL_RGBA16F_ARB,
    GL_R32F,
    GL_RG32F,
    GL_RGBA32F_ARB};

GLenum map_Format_Format[] = {
    0,
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGB,
    GL_BGR,
    GL_BGR,
    GL_RGBA,
    GL_RGBA,
    GL_BGRA,
    GL_BGRA,

    GL_DEPTH_STENCIL,
    GL_DEPTH_COMPONENT,

    GL_RGBA,
    GL_RGBA,
    GL_RGBA,
    GL_RGBA,

    GL_RED,
    GL_RG,
    GL_RGBA,
    GL_RED,
    GL_RG,
    GL_RGBA};

GLenum map_Format_Type[] = {
    0,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,

    GL_UNSIGNED_INT_24_8,
    GL_FLOAT,

    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,

    GL_HALF_FLOAT,
    GL_HALF_FLOAT,
    GL_HALF_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT};

enum OpenGLFormat
{
    GLFMT_UNKNOWN = 0,
    GLFMT_R8,
    GLFMT_R8G8,
    GLFMT_R8G8B8,
    GLFMT_R8G8B8_sRGB,
    GLFMT_B8G8R8,
    GLFMT_B8G8R8_sRGB,
    GLFMT_A8R8G8B8,
    GLFMT_A8R8G8B8_sRGB,
    GLFMT_A8B8G8R8,
    GLFMT_A8B8G8R8_sRGB,

    GLFMT_D24S8,
    GLFMT_D32F,

    GLFMT_DXT1,
    GLFMT_DXT1_sRGB,
    GLFMT_DXT5,
    GLFMT_DXT5_sRGB,

    GLFMT_R16F,
    GLFMT_G16R16F,
    GLFMT_A16B16G16R16F,

    GLFMT_R32F,
    GLFMT_G32R32F,
    GLFMT_A32B32G32R32F
};

class Effect
{
public:
    Smoke *smoke = NULL;
    SortParticles *sort = NULL;
    Camera *camera = NULL;

    GLuint particlelayout = 0;
    GLuint emittersbuffer = 0;
    GLuint particlebuffers[3] = {0, 0, 0};
    GLuint transformfeedbacks[3] = {0, 0, 0};
    Particle *emitters = nullptr;
    GLuint randomtex = 0;
    GLuint countquery = 0;
    GLuint smoketex = 0;
    GLuint gradienttex = 0;

    // light position
    GLfloat lightPos[4] = {1.0f, 0.65f, -0.5f, 0.0f};

    // color array
    GLfloat ambient[4] = {0.01f, 0.01f, 0.01f, 1.0f};
    GLfloat color[4] = {1.0f, 0.782f, 0.344f, 1.0f};

    float elapsedtime = 0.0f;

    Bool initialize(void)
    {
        glClearColor(0.0f, 0.0103f, 0.0707f, 1.0f);
        glClearDepth(1.0);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // create input layout for particles
        glGenVertexArrays(1, &particlelayout);
        glBindVertexArray(particlelayout);
        {
            glBindVertexBuffers(0, 0, NULL, NULL, NULL);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);

            glVertexAttribBinding(0, 0);
            glVertexAttribBinding(1, 0);
            glVertexAttribBinding(2, 0);

            glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
            glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 16);
            glVertexAttribFormat(2, 4, GL_FLOAT, GL_FALSE, 32);
        }
        glBindVertexArray(0);

        // create buffers & transform feedbacks
        glGenBuffers(1, &emittersbuffer);
        glGenBuffers(3, particlebuffers);
        glGenTransformFeedbacks(3, transformfeedbacks);

        emitters = new Particle[NUM_EMITTERS];
        memset(emitters, 0, NUM_EMITTERS * sizeof(Particle));

        glBindBuffer(GL_ARRAY_BUFFER, emittersbuffer);
        glBufferData(GL_ARRAY_BUFFER, NUM_EMITTERS * sizeof(Particle), NULL, GL_DYNAMIC_DRAW);

        for (int i = 0; i < 3; ++i)
        {
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformfeedbacks[i]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particlebuffers[i]);
        }

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        glBindBuffer(GL_ARRAY_BUFFER, particlebuffers[0]);
        glBufferStorage(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle), NULL, 0);

        glBindBuffer(GL_ARRAY_BUFFER, particlebuffers[1]);
        glBufferStorage(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle), NULL, 0);

        glBindBuffer(GL_ARRAY_BUFFER, particlebuffers[2]);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle), NULL, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // create random texture
        float *data = new float[2048 * 4];

        for (int i = 0; i < 2048 * 4; ++i)
            data[i] = Math::RandomFloat();

        glGenTextures(1, &randomtex);

        glBindTexture(GL_TEXTURE_2D, randomtex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2048, 1, 0, GL_RGBA, GL_FLOAT, data);

        delete[] data;

        // initialize shaders
        if (!sort->initialize())
        {
            PrintLog("Compute Shader for Sort :: Failed to load\n");
            return False;
        }

        if (!smoke->initializeSmokeEmit())
        {
            PrintLog("ShaderProgramObjectEmit :: Failed to load\n");
            return False;
        }

        if (!smoke->initializeSmokeUpdate())
        {
            PrintLog("ShaderProgramObjectUpdate :: Failed to load\n");
            return False;
        }

        if (!smoke->initializeBillboard())
        {
            PrintLog("ShaderProgramObjectBillboard :: Failed to load\n");
            return False;
        }

        // load smoke texture
        GLCreateVolumeTextureFromFile("./assets/textures/smoke/smokevol1.dds", true, &smoketex);

        glBindTexture(GL_TEXTURE_3D, smoketex);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // load gradient texture
        GLCreateTextureFromFile("./assets/textures/smoke/colorgradient.dds", true, &gradienttex);

        glBindTexture(GL_TEXTURE_2D, gradienttex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // create query
        glGenQueries(1, &countquery);

        // Camera initialization
        camera = new Camera();
    }

    bool GLCreateVolumeTextureFromFile(const char *file, bool srgb, GLuint *out)
    {
        DDS_Image_Info info;
        GLuint texid; // = OpenGLContentManager().IDTexture(file);

        if (texid != 0)
        {
            printf("Pointer %s\n", file);
            *out = texid;

            return true;
        }

        if (!LoadFromDDS(file, &info))
        {
            std::cout << "Error: Could not load texture!\n";
            return false;
        }

        glGenTextures(1, &texid);
        glBindTexture(GL_TEXTURE_3D, texid);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (info.MipLevels > 1)
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        else
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        GLenum format = info.Format;

        if (info.Format == GLFMT_DXT1 || info.Format == GLFMT_DXT5)
        {
            // compressed
            GLsizei size;
            GLsizei offset = 0;

            if (srgb)
            {
                if (format == GLFMT_DXT1)
                    format = GLFMT_DXT1_sRGB;
                else
                    format = GLFMT_DXT5_sRGB;
            }

            for (uint32_t j = 0; j < info.MipLevels; ++j)
            {
                size = GetCompressedLevelSize(info.Width, info.Height, info.Depth, j, info.Format);

                glCompressedTexImage3D(GL_TEXTURE_3D, j, map_Format_Internal[format],
                                       info.Width, info.Height, info.Depth, 0, size, (char *)info.Data + offset);

                offset += size * info.Depth;
            }
        }
        else
        {
            // TODO:
        }

        if (info.Data)
            free(info.Data);

        GLenum err = glGetError();

        if (err != GL_NO_ERROR)
        {
            glDeleteTextures(1, &texid);
            texid = 0;

            std::cout << "Error: Could not create texture!\n";
        }
        else
        {
            std::cout << "Created volume texture " << info.Width << "x" << info.Height << "x" << info.Depth << "\n";
        }

        *out = texid;
        // OpenGLContentManager().RegisterTexture(file, texid);

        return (texid != 0);
    }

    bool GLCreateTextureFromFile(const char *file, bool srgb, GLuint *out /*, GLuint flags*/)
    {
        if (out == nullptr)
            return false;

        std::string ext;
        uint8_t *imgdata = nullptr;
        GLuint texid; // = OpenGLContentManager().IDTexture(file);
        GLsizei width = 0;
        GLsizei height = 0;

        if (texid != 0)
        {
            printf("Pointer %s\n", file);
            *out = texid;

            return true;
        }

        Math::GetExtension(ext, file);

        if (ext == "dds")
            return GLCreateTextureFromDDS(file, srgb, out);

#ifdef _WIN32
        Gdiplus::Bitmap *bitmap = Win32LoadPicture(file);

        if (bitmap == nullptr)
            return false;

        if (bitmap->GetLastStatus() != Gdiplus::Ok)
        {
            delete bitmap;
            return false;
        }

        Gdiplus::BitmapData data;

        bitmap->LockBits(0, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data);
        {
            imgdata = new uint8_t[data.Width * data.Height * 4];
            memcpy(imgdata, data.Scan0, data.Width * data.Height * 4);

            for (UINT i = 0; i < data.Height; ++i)
            {
                // swap red and blue
                for (UINT j = 0; j < data.Width; ++j)
                {
                    UINT index = (i * data.Width + j) * 4;
                    Math::Swap<uint8_t>(imgdata[index + 0], imgdata[index + 2]);
                }

                // flip on X
                if (flags & GLTEX_FLIPX)
                {
                    for (UINT j = 0; j < data.Width / 2; ++j)
                    {
                        UINT index1 = (i * data.Width + j) * 4;
                        UINT index2 = (i * data.Width + (data.Width - j - 1)) * 4;

                        Math::Swap<uint32_t>(*((uint32_t *)(imgdata + index1)), *((uint32_t *)(imgdata + index2)));
                    }
                }
            }

            if (flags & GLTEX_FLIPY)
            {
                // flip on Y
                for (UINT j = 0; j < data.Height / 2; ++j)
                {
                    for (UINT i = 0; i < data.Width; ++i)
                    {
                        // UINT index1 = (i * data.Width + j) * 4;
                        // UINT index2 = (i * data.Width + (data.Width - j - 1)) * 4;

                        UINT index1 = (j * data.Width + i) * 4;
                        UINT index2 = ((data.Height - j - 1) * data.Width + i) * 4;

                        Math::Swap<uint32_t>(*((uint32_t *)(imgdata + index1)), *((uint32_t *)(imgdata + index2)));
                    }
                }
            }
        }
        bitmap->UnlockBits(&data);

        width = data.Width;
        height = data.Height;

        delete bitmap;
#else
            // TODO:
#endif

        glGenTextures(1, &texid);
        glBindTexture(GL_TEXTURE_2D, texid);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (srgb)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);

        glGenerateMipmap(GL_TEXTURE_2D);

        GLenum err = glGetError();

        if (err != GL_NO_ERROR)
        {
            glDeleteTextures(1, &texid);
            texid = 0;

            std::cout << "Error: Could not create texture!\n";
        }
        else
        {
            std::cout << "Created texture " << width << "x" << height << "\n";
        }

        delete[] imgdata;

        *out = texid;
        // OpenGLContentManager().RegisterTexture(file, texid);

        return (texid != 0);
    }

    bool GLCreateTextureFromDDS(const char *file, bool srgb, GLuint *out)
    {
        DDS_Image_Info info;
        GLuint texid; // = OpenGLContentManager().IDTexture(file);

        if (texid != 0)
        {
            printf("Pointer %s\n", file);
            *out = texid;

            return true;
        }

        if (!LoadFromDDS(file, &info))
        {
            std::cout << "Error: Could not load texture!\n";
            return false;
        }

        glGenTextures(1, &texid);
        glBindTexture(GL_TEXTURE_2D, texid);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (info.MipLevels > 1)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        else
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        GLsizei pow2w = Math::NextPow2(info.Width);
        GLsizei pow2h = Math::NextPow2(info.Height);
        GLsizei mipsize;
        GLuint format = info.Format;

        if (info.Format == GLFMT_DXT1 || info.Format == GLFMT_DXT5)
        {
            if (srgb)
            {
                if (info.Format == GLFMT_DXT1)
                    format = GLFMT_DXT1_sRGB;
                else
                    format = GLFMT_DXT5_sRGB;
            }

            // compressed
            GLsizei width = info.Width;
            GLsizei height = info.Height;
            GLsizei offset = 0;

            for (uint32_t j = 0; j < info.MipLevels; ++j)
            {
                mipsize = GetCompressedLevelSize(info.Width, info.Height, j, info.Format);

                glCompressedTexImage2D(GL_TEXTURE_2D, j, map_Format_Internal[format],
                                       width, height, 0, mipsize, (char *)info.Data + offset);

                offset += mipsize;

                width = (pow2w >> (j + 1));
                height = (pow2h >> (j + 1));
            }
        }
        else
        {
            // uncompressed
            uint32_t bytes = 4;

            if (info.Format == GLFMT_G32R32F)
            {
                bytes = 8;
            }
            else if (info.Format == GLFMT_G16R16F)
            {
                bytes = 4;
            }
            else if (info.Format == GLFMT_R8G8B8 || info.Format == GLFMT_B8G8R8)
            {
                if (srgb)
                {
                    // see the enum
                    format = info.Format + 1;
                }

                bytes = 3;
            }

            mipsize = info.Width * info.Height * bytes;

            // TODO: mipmap
            glTexImage2D(GL_TEXTURE_2D, 0, map_Format_Internal[format], info.Width, info.Height, 0,
                         map_Format_Format[format], map_Format_Type[format], (char *)info.Data);

            if (info.MipLevels > 1)
                glGenerateMipmap(GL_TEXTURE_2D);
        }

        if (info.Data)
            free(info.Data);

        GLenum err = glGetError();

        if (err != GL_NO_ERROR)
        {
            glDeleteTextures(1, &texid);
            texid = 0;

            std::cout << "Error: Could not create texture!\n";
        }
        else
        {
            std::cout << "Created texture " << info.Width << "x" << info.Height << "\n";
        }

        *out = texid;
        // OpenGLContentManager().RegisterTexture(file, texid);

        return (texid != 0);
    }

    void display(void)
    {
        // code
        static float time = 0;
        static bool prevbufferusable = false;

        time += elapsedtime;

        // update emitters
        for (int i = 0; i < NUM_EMITTERS; ++i)
        {
            emitters[i].velocity[3] = ((emitters[i].velocity[3] > EMIT_RATE) ? 0.0f : (emitters[i].velocity[3] + elapsedtime));

            if (i == 0)
            {
                emitters[i].position = vmath::vec4(BOWL1_POSITION, 1);
                emitters[i].position[1] += 0.75f;
            }
            else if (i == 1)
            {
                emitters[i].position = vmath::vec4(BOWL2_POSITION, 1);
                emitters[i].position[1] += 0.75f;
            }
            else
            {
                // assert(false);
                printf(" FALSE \n");
            }
        }

        glUseProgram(0);

        glBindBuffer(GL_ARRAY_BUFFER, emittersbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_EMITTERS * sizeof(Particle), emitters);

        // emit particles
        GLintptr offset = 0;
        GLsizei stride = sizeof(Particle);

        glEnable(GL_RASTERIZER_DISCARD);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformfeedbacks[2]);
        {
            glBindTexture(GL_TEXTURE_2D, randomtex);
            glProgramUniform1f(smokeemitpipeline->GetProgram(), 1, time);
            glProgramUniform1f(smokeemitpipeline->GetProgram(), 2, EMIT_RATE);

            smokeemitpipeline->Bind();

            glBindVertexArray(particlelayout);
            glBindVertexBuffers(0, 1, &emittersbuffer, &offset, &stride);

            glBeginTransformFeedback(GL_POINTS);
            {
                glDrawArrays(GL_POINTS, 0, NUM_EMITTERS);
            }
            glEndTransformFeedback();
        }

        // update particles
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformfeedbacks[currentbuffer]);
        {
            glProgramUniform1f(smokeupdatepipeline->GetProgram(), 0, elapsedtime);
            glProgramUniform1f(smokeupdatepipeline->GetProgram(), 1, BUOYANCY);
            glProgramUniform1f(smokeupdatepipeline->GetProgram(), 2, LIFE_SPAN);

            smokeupdatepipeline->Bind();

            glBindVertexArray(particlelayout);
            glBindVertexBuffers(0, 1, &particlebuffers[2], &offset, &stride);

            glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, countquery);
            glBeginTransformFeedback(GL_POINTS);
            {
                // update and add newly generated particles
                glDrawTransformFeedback(GL_POINTS, transformfeedbacks[2]);

                // update and add older particles
                if (prevbufferusable)
                {
                    glBindVertexBuffers(0, 1, &particlebuffers[1 - currentbuffer], &offset, &stride);
                    glDrawTransformFeedback(GL_POINTS, transformfeedbacks[1 - currentbuffer]);
                }
            }
            glEndTransformFeedback();
            glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
        }
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
        glDisable(GL_RASTERIZER_DISCARD);

        // sort particles
        GLuint count = 0;
        glGetQueryObjectuiv(countquery, GL_QUERY_RESULT, &count);

        camera->getEye(); // returns eye position

    }

    void update(void)
    {
        // code
        elapsedtime += 0.01f;
    }
};

#endif