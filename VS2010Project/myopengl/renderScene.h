#pragma once

#include "glCamera.h"

void initScene(void);
void renderScene(mat4& mProjectionMatrix);
void releaseScene(void);

extern CGLCamera cCamera;