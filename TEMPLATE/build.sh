clear

rm a.out

g++ src/OGLMain.cpp src/utils/GLShaders.cpp src/utils/GLLog.cpp src/utils/common.cpp src/utils/Noise.cpp -g -lX11 -lGL -lGLEW -lSOIL -lm
# g++ src/OGLMain.cpp /I src/03_UtilityFiles/GLShaders.cpp src/03_UtilityFiles/GLLog.cpp src/03_UtilityFiles/common.cpp src/03_UtilityFiles/Noise.cpp -g -lX11 -lGL -lGLEW -lSOIL -lm

./a.out
