//////////////////////////////////////////////////////////////

//SPHERE
//////////////////////////////////////////////////////////////
#pragma once

#include <stdio.h>
#include <math.h>

const int CACHE_SIZE = 240;
const int MAX_ARRAY_SIZE = 100000;

class Sphere
{
    private:
        int VertexPointer = 0;
        int TexcoordPointer = 0;
        int NormalPointer = 0;

        float model_Vertices[MAX_ARRAY_SIZE];
        float model_Texcoord[MAX_ARRAY_SIZE];
        float model_Normal[MAX_ARRAY_SIZE];

        void addTexcoord(float a, float b)
        {
            model_Texcoord[TexcoordPointer++] = a;
            model_Texcoord[TexcoordPointer++] = b;
        }

        void addVertices(float x, float y, float z)
        {
            model_Vertices[VertexPointer++] = x;
            model_Vertices[VertexPointer++] = y;
            model_Vertices[VertexPointer++] = z;
        }

        void addNormals(float p, float q, float r)
        {
            model_Normal[NormalPointer++] = p;
            model_Normal[NormalPointer++] = q;
            model_Normal[NormalPointer++] = r;
        }

    public:
        Sphere(float radius, int slices, int stacks)
        {
            generateSphereData1(radius, slices, stacks);
        }

        int getNumberOfSphereVertices()
        {
            return(VertexPointer);
        }

        int getNumberOfSphereTexcoord()
        {
            return(TexcoordPointer);
        }

        int getNumberOfSphereNormal()
        {
            return(NormalPointer);
        }

        float* getSphereVertex()
        {
            return(model_Vertices);
        }

        float* getSphereTexcoord()
        {
            return(model_Texcoord);
        }

        float* getSphereNormal()
        {
            return(model_Normal);
        }


        void generateSphereData1(double radius, int slices, int stacks)
        {
            // int k = 0;
            // int kk = 0;

            int i, j;
            float sinCache1a[CACHE_SIZE];
            float cosCache1a[CACHE_SIZE];
            float sinCache2a[CACHE_SIZE];
            float cosCache2a[CACHE_SIZE];
            // float sinCache3a[CACHE_SIZE];
            // float cosCache3a[CACHE_SIZE];
            float sinCache1b[CACHE_SIZE];
            float cosCache1b[CACHE_SIZE];
            float sinCache2b[CACHE_SIZE];
            float cosCache2b[CACHE_SIZE];
            // float sinCache3b[CACHE_SIZE];
            // float cosCache3b[CACHE_SIZE];
            float angle;
            float zLow, zHigh;
            float sintemp1 = 0.0, sintemp2 = 0.0, sintemp3 = 0.0, sintemp4 = 0.0;
            float costemp1 = 0.0, costemp2 = 0.0, costemp3 = 0.0, costemp4 = 0.0;
            int start, finish;

            if (slices >= CACHE_SIZE) 
                slices = CACHE_SIZE - 1;
            if (stacks >= CACHE_SIZE) 
                stacks = CACHE_SIZE - 1;
            if (slices < 2 || stacks < 1 || radius < 0.0) {
                return;
            }

            for (i = 0; i < slices; i++) {
                angle = 2 * (float)M_PI * i / slices;
                sinCache1a[i] = (float)sin(angle);
                cosCache1a[i] = (float)cos(angle);
                sinCache2a[i] = sinCache1a[i];
                cosCache2a[i] = cosCache1a[i];
            }

            for (j = 0; j <= stacks; j++) {
                angle = (float)M_PI * j / stacks;
                sinCache2b[j] = (float)sin(angle);
                cosCache2b[j] = (float)cos(angle);
                sinCache1b[j] = radius * (float)sin(angle);
                cosCache1b[j] = radius * (float)cos(angle);
            }
            /* Make sure it comes to a point */
            sinCache1b[0] = 0;
            sinCache1b[stacks] = 0;

            sinCache1a[slices] = sinCache1a[0];
            cosCache1a[slices] = cosCache1a[0];
            sinCache2a[slices] = sinCache2a[0];
            cosCache2a[slices] = cosCache2a[0];

            start = 0;
            finish = stacks;
            for (j = start; j < finish; j++) {
                zLow = cosCache1b[j];
                zHigh = cosCache1b[j + 1];
                sintemp1 = sinCache1b[j];
                sintemp2 = sinCache1b[j + 1];

                sintemp3 = sinCache2b[j + 1];
                costemp3 = cosCache2b[j + 1];
                sintemp4 = sinCache2b[j];
                costemp4 = cosCache2b[j];


                //fprintf(gpFile, "Slices are %d\n", slices);
                for (i = 0; i <= slices; i++)
                {

                    addVertices(sintemp2 * sinCache1a[i], sintemp2 * cosCache1a[i], zHigh);
                    addVertices(sintemp1 * sinCache1a[i], sintemp1 * cosCache1a[i], zLow);

                    addNormals(sinCache2a[i] * sintemp3, cosCache2a[i] * sintemp3, costemp3);
                    addNormals(sinCache2a[i] * sintemp4, cosCache2a[i] * sintemp4, costemp4);

                    addTexcoord(1 - (float)i / slices, 1 - (float)(j + 1) / stacks);
                    addTexcoord(1 - (float)i / slices, 1 - (float)(j) / stacks);

                }

            }
        }
};
