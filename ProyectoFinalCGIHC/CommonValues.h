#ifndef COMMONVALS
#define COMMONVALS
#include "stb_image.h"

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 4;

// Se mueve esta constante aqui para poder utilizarla en SceneInformation.cpp y Main.cpp
const double LIMIT_FPS = 1.0 / 60.0;  

#endif