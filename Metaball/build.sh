clear

rm a.out

g++ src/OGLMain.cpp src/utils/GLShaders.cpp src/utils/GLLog.cpp src/utils/common.cpp src/utils/Noise.cpp src/effects/metaball/Matrix.cpp src/effects/metaball/Shader.cpp -g -lX11 -lGL -lGLEW -lSOIL -lm
# g++ src/OGLMain.cpp src/utils/GLShaders.cpp src/utils/GLLog.cpp src/utils/common.cpp src/utils/Noise.cpp -g -lX11 -lGL -lGLEW -lSOIL -lm -lpthread

./a.out
