#pragma once
#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"
#include "OrderedArray.hpp"
#include "3Dmath.h"

#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cfloat>
#include <cmath>
#include <cstdarg>
#include <string>

/* Minimum and maximum values a `signed int' can hold.  */
#define INT_MIN (-INT_MAX - 1)
#define INT_MAX 2147483647

using namespace std;

class SortParticles
{
public:
    GLuint shaderProgramObjectSort;
    GLuint program;

    GLuint numValuesUniform;
    GLuint outerIndexUniform;
    GLuint innerIndexUniform;
    GLuint ascendingUniform;

    uint32_t floatsize;
    float *floatvalues;
    uint32_t floatcap;

    int *intvalues;
    uint32_t intcap;
    uint32_t intsize;

    GLuint *uintvalues;
    uint32_t uintcap;
    uint32_t uintsize;

    struct Uniform
    {
        char Name[32];
        GLint StartRegister;
        GLint RegisterCount;
        GLint Location;
        GLenum Type;

        mutable bool Changed;

        inline bool operator<(const Uniform &other) const
        {
            return (0 > strcmp(Name, other.Name));
        }
    };

    typedef OrderedArray<Uniform> UniformTable;
    UniformTable uniforms;

    struct UniformBlock
    {
        char Name[32];
        GLint Index;
        mutable GLint Binding;
        GLint BlockSize;

        inline bool operator<(const UniformBlock &other) const
        {
            return (0 > strcmp(Name, other.Name));
        }
    };

    typedef OrderedArray<UniformBlock> UniformBlockTable;
    UniformBlockTable uniformblocks;

    struct UniformBlock
    {
        char Name[32];
        GLint Index;
        mutable GLint Binding;
        GLint BlockSize;

        inline bool operator<(const UniformBlock &other) const
        {
            return (0 > strcmp(Name, other.Name));
        }
    };

    uint32_t Log2OfPow2(uint32_t x)
    {
        uint32_t ret = 0;

        while (x >>= 1)
            ++ret;

        return ret;
    }

    bool initialize(void)
    {
        GLuint computeShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/sortparticles.cmps", COMPUTE);

        shaderProgramObjectSort = glCreateProgram();
        glAttachShader(shaderProgramObjectSort, computeShaderObject);

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObjectSort);

        if (bShaderLinkStatus == False)
            return False;

        QueryUniforms();
    }

    void QueryUniforms(void)
    {
        GLint count;
        GLenum type;
        GLsizei length;
        GLint size, loc;
        GLchar uniname[256];

        if (program == 0)
            return;

        memset(uniname, 0, sizeof(uniname));
        // uniforms.Clear();

        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &count);

        if (count > 0)
        {
            // uniform blocks
            std::vector<std::string> blocknames;
            blocknames.reserve(count);

            for (int i = 0; i < count; ++i)
            {
                GLsizei namelength = 0;
                char name[32];

                glGetActiveUniformBlockName(program, i, 32, &namelength, name);

                name[namelength] = 0;
                blocknames.push_back(name);
            }

            for (size_t i = 0; i < blocknames.size(); ++i)
            {
                GLint blocksize = 0;
                GLint index = glGetUniformBlockIndex(program, blocknames[i].c_str());

                glGetActiveUniformBlockiv(program, (GLuint)i, GL_UNIFORM_BLOCK_DATA_SIZE, &blocksize);
                AddUniformBlock(blocknames[i].c_str(), index, INT_MAX, blocksize);
            }
        }
        else
        {
            glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

            // uniforms
            for (int i = 0; i < count; ++i)
            {
                memset(uniname, 0, sizeof(uniname));

                glGetActiveUniform(program, i, 256, &length, &size, &type, uniname);
                loc = glGetUniformLocation(program, uniname);

                for (int j = 0; j < length; ++j)
                {
                    if (uniname[j] == '[')
                        uniname[j] = '\0';
                }

                if (loc == -1 || type == 0x1405) // not specificed in standard, subroutine
                    continue;

                AddUniform(uniname, loc, size, type);
            }
        }
    }

    void AddUniformBlock(const char *name, GLint index, GLint binding, GLint blocksize)
    {
        UniformBlock block;

        if (strlen(name) >= sizeof(block.Name))
            throw std::length_error("Uniform block name too long");

        block.Index = index;
        block.Binding = binding;
        block.BlockSize = blocksize;

        // CopyString(block.Name, name);
        strcpy(block.Name, name);
        uniformblocks.Insert(block);
    }

    void AddUniform(const char *name, GLuint location, GLuint count, GLenum type)
    {
        Uniform uni;

        if (strlen(name) >= sizeof(uni.Name))
            throw std::length_error("Uniform name too long");

        strcpy(uni.Name, name);
        // CopyString(uni.Name, name);

        if (type == GL_FLOAT_MAT4)
            count = 4;

        uni.Type = type;
        uni.RegisterCount = count;
        uni.Location = location;
        uni.Changed = true;

        if (type == GL_FLOAT || (type >= GL_FLOAT_VEC2 && type <= GL_FLOAT_VEC4) || type == GL_FLOAT_MAT4)
        {
            uni.StartRegister = floatsize;

            if (floatsize + count > floatcap)
            {
                uint32_t newcap = std::max<uint32_t>(floatsize + count, floatsize + 8);

                floatvalues = (float *)realloc(floatvalues, newcap * 4 * sizeof(float));
                floatcap = newcap;
            }

            float *reg = (floatvalues + uni.StartRegister * 4);

            if (type == GL_FLOAT_MAT4)
                Math::MatrixIdentity((Math::Matrix &)*reg);
            else
                memset(reg, 0, uni.RegisterCount * 4 * sizeof(float));

            floatsize += count;
        }
        else if (
            type == GL_INT ||
            (type >= GL_INT_VEC2 && type <= GL_INT_VEC4) ||
            type == GL_SAMPLER_2D || type == GL_SAMPLER_2D_ARRAY || type == GL_SAMPLER_BUFFER ||
            type == GL_SAMPLER_CUBE || type == GL_IMAGE_2D)
        {
            uni.StartRegister = intsize;

            if (intsize + count > intcap)
            {
                uint32_t newcap = std::max<uint32_t>(intsize + count, intsize + 8);

                intvalues = (int *)realloc(intvalues, newcap * 4 * sizeof(int));
                intcap = newcap;
            }

            int *reg = (intvalues + uni.StartRegister * 4);
            memset(reg, 0, uni.RegisterCount * 4 * sizeof(int));

            intsize += count;
        }
        else if (type == GL_UNSIGNED_INT_VEC4)
        {
            uni.StartRegister = uintsize;

            if (uintsize + count > uintcap)
            {
                uint32_t newcap = std::max<uint32_t>(uintsize + count, uintsize + 8);

                uintvalues = (GLuint *)realloc(uintvalues, newcap * 4 * sizeof(GLuint));
                uintcap = newcap;
            }

            GLuint *reg = (uintvalues + uni.StartRegister * 4);
            memset(reg, 0, uni.RegisterCount * 4 * sizeof(GLuint));

            uintsize += count;
        }
        else
        {
            // not handled
            throw std::invalid_argument("This uniform type is not supported");
        }

        uniforms.Insert(uni);
    }

    void Sort(GLuint buffer, int count, bool ascending)
    {
        // NOTE: optimized version

        int pow2count = Math::NextPow2(count);
        int log2threadgroupsize = Math::Log2OfPow2(256); // threadgroupsize = 256
        int log2count = Math::Log2OfPow2(pow2count);

        GLuint funcindex = 0;
        GLuint numthreadgroups = pow2count / 256;

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);

        // computeshader->SetInt("numValues", count);
        // computeshader->SetInt("ascending", (ascending ? 1 : 0));
        // computeshader->Begin();
        glUniform1i(numValuesUniform, count);
        glUniform1i(ascendingUniform, (ascending ? 1 : 0));
        glUseProgram(shaderProgramObjectSort);
        CommitChanges();

        // presort up to k = threadgroupsize
        glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &funcindex);
        glDispatchCompute(numthreadgroups, 1, 1);

        // must synchronize incoherent writes
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // progressive sort from k = 2 * threadgroupsize
        for (int s = log2threadgroupsize + 1; s <= log2count; ++s)
        {
            // computeshader->SetInt("outerIndex", s);
            glUniform1i(outerIndexUniform, s);

            for (int t = s - 1; t >= log2threadgroupsize; --t)
            {
                funcindex = 1;

                // computeshader->SetInt("innerIndex", t);
                glUniform1i(innerIndexUniform, t);
                CommitChanges();

                glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &funcindex);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
                glDispatchCompute(numthreadgroups, 1, 1);

                // must synchronize incoherent writes
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            }

            // we can sort this part more efficiently
            funcindex = 2;

            glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &funcindex);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
            glDispatchCompute(numthreadgroups, 1, 1);

            // must synchronize incoherent writes
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        // computeshader->End();
        glUseProgram(0);
    }

    void CommitChanges()
    {
        for (size_t i = 0; i < uniforms.Size(); ++i)
        {
            const Uniform &uni = uniforms[i];

            float *floatdata = (floatvalues + uni.StartRegister * 4);
            int *intdata = (intvalues + uni.StartRegister * 4);
            GLuint *uintdata = (uintvalues + uni.StartRegister * 4);

            if (!uni.Changed)
                continue;

            uni.Changed = false;

            switch (uni.Type)
            {
            case GL_FLOAT:
                glUniform1fv(uni.Location, uni.RegisterCount, floatdata);
                break;

            case GL_FLOAT_VEC2:
                glUniform2fv(uni.Location, uni.RegisterCount, floatdata);
                break;

            case GL_FLOAT_VEC3:
                glUniform3fv(uni.Location, uni.RegisterCount, floatdata);
                break;

            case GL_FLOAT_VEC4:
                glUniform4fv(uni.Location, uni.RegisterCount, floatdata);
                break;

            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(uni.Location, uni.RegisterCount / 4, false, floatdata);
                break;

            case GL_UNSIGNED_INT_VEC4:
                glUniform4uiv(uni.Location, uni.RegisterCount, uintdata);
                break;

            case GL_INT:
            case GL_SAMPLER_2D:
            case GL_SAMPLER_CUBE:
            case GL_SAMPLER_BUFFER:
            case GL_IMAGE_2D:
                glUniform1i(uni.Location, intdata[0]);
                break;

            default:
                break;
            }
        }
    }
};