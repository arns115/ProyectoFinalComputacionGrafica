#ifndef COMMONVALS
#define COMMONVALS
#include "stb_image.h"

const int MAX_POINT_LIGHTS = 5;
const int MAX_SPOT_LIGHTS = 2;

// Esta conbstante define el límite de FPS para la aplicación (Se usa en SceneInformation.cpp y Main.cpp)
const double LIMIT_FPS = 1.0 / 60.0;  

#endif