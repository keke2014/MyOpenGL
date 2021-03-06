#pragma once

#include "vertexBufferObject.h"

extern vec3 vCubeVertices[36];
extern vec2 vCubeTexCoords[6];
extern vec3 vCubeNormals[6];
extern vec3 vGround[6];

int generateTorus(CVertexBufferObject &vboDest, float fRadius, float fTubeRadius, int iSubDivAround, int iSubDivTube);