clear

rm a.out

g++ src/OGLMain.cpp src/utils/GLShaders.cpp src/utils/GLLog.cpp src/utils/common.cpp src/utils/Noise.cpp -g -lX11 -lGL -lGLEW -lSOIL -lm

./a.out
