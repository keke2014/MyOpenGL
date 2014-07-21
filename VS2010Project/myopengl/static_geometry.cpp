
#include "static_geometry.h"

/*
* 生成圆环体
* 输入:	vboDest - 储存数据的VBO
		fRadius - 外环半径
		fTubeRadius - 内环半径
		iSubDivAround - 外环细分
		iSubDivTube - 内环细分
*/

int generateTorus(CVertexBufferObject &vboDest, float fRadius, float fTubeRadius, int iSubDivAround, int iSubDivTube)
{
	float fAddAngleAround = 360.0f/(float)iSubDivAround;
	float fAddAngleTube = 360.0f/(float)iSubDivTube;

	float fCurAngleAround = 0.0f;
	int iStepsAround = 1;

	int iFacesAdded = 0;

	while(iStepsAround <= iSubDivAround) {
		float fSineAround = sin(fCurAngleAround/180.0f*PI);
		float fCosineAround = cos(fCurAngleAround/180.0f*PI);
		vec3 vDir1(fSineAround, fCosineAround, 0.0f);
		float fNextAngleAround = fCurAngleAround+fAddAngleAround;
		float fNextSineAround = sin(fNextAngleAround/180.0f*PI);
		float fNextCosineAround = cos(fNextAngleAround/180.0f*PI);
		vec3 vDir2(fNextSineAround, fNextCosineAround, 0.0f);
		float fCurAngleTube = 0.0f;
		int iStepsTube = 1;

		while(iStepsTube <= iSubDivTube) {
			float fSineTube = sin(fCurAngleTube/180.0f*PI);
			float fCosineTube = cos(fCurAngleTube/180.0f*PI);
			float fNextAngleTube = fCurAngleTube+fAddAngleTube;
			float fNextSineTube = sin(fNextAngleTube/180.0f*PI);
			float fNextCosineTube = cos(fNextAngleTube/180.0f*PI);
			vec3 vMid1 = vDir1*(fRadius-fTubeRadius/2), vMid2 = vDir2*(fRadius-fTubeRadius/2);
			
			vec3 vQuadPoints[] =  {
				vMid1 + vec3(0.0f, 0.0f, -fNextSineTube*fTubeRadius) + vDir1*fNextCosineTube*fTubeRadius,
				vMid1 + vec3(0.0f, 0.0f, -fSineTube*fTubeRadius) + vDir1*fCosineTube*fTubeRadius,
				vMid2 + vec3(0.0f, 0.0f, -fSineTube*fTubeRadius) + vDir2*fCosineTube*fTubeRadius,
				vMid2 + vec3(0.0f, 0.0f, -fNextSineTube*fTubeRadius) + vDir2*fNextCosineTube*fTubeRadius
			};

			vec3 vNormals[] = {
				vec3(0.0f, 0.0f, -fNextSineTube) + vDir1*fNextCosineTube,
				vec3(0.0f, 0.0f, -fSineTube) + vDir1*fCosineTube,
				vec3(0.0f, 0.0f, -fSineTube) + vDir2*fCosineTube,
				vec3(0.0f, 0.0f, -fNextSineTube) + vDir2*fNextCosineTube
			};

			vec2 vTexCoords[] = {
				vec2(fCurAngleAround/360.0f, fNextAngleTube/360.0f),
				vec2(fCurAngleAround/360.0f, fCurAngleTube/360.0f),
				vec2(fNextAngleAround/360.0f, fCurAngleTube/360.0f),
				vec2(fNextAngleAround/360.0f, fNextAngleTube/360.0f)
			};

			int iIndices[] = { 0, 1, 2, 2, 3, 0 };

			for(int i = 0; i < 6; ++i) {
				int index = iIndices[i];
				vboDest.addData(&vQuadPoints[index], sizeof(vec3));
				vboDest.addData(&vTexCoords[index], sizeof(vec2));
				vboDest.addData(&vNormals[index], sizeof(vec3));
			}
			iFacesAdded += 2; // 计数面数
			fCurAngleTube += fAddAngleTube;
			iStepsTube++;
		}
		fCurAngleAround += fAddAngleAround;
		iStepsAround++;
	}
	return iFacesAdded;
}

vec3 vCubeVertices[36] = {
	// 前平面位置
	vec3(-0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, -0.5f, 0.5f), vec3(0.5f, -0.5f, 0.5f), vec3(-0.5f, -0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f),
	// 后平面位置
	vec3(0.5f, 0.5f, -0.5f), vec3(-0.5f, 0.5f, -0.5f), vec3(-0.5f, -0.5f, -0.5f), vec3(-0.5f, -0.5f, -0.5f), vec3(0.5f, -0.5f, -0.5f), vec3(0.5f, 0.5f, -0.5f),
	// 左平面位置
	vec3(-0.5f, 0.5f, -0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(-0.5f, -0.5f, 0.5f), vec3(-0.5f, -0.5f, 0.5f), vec3(-0.5f, -0.5f, -0.5f), vec3(-0.5f, 0.5f, -0.5f),
	// 右平面位置
	vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, -0.5f), vec3(0.5f, -0.5f, -0.5f), vec3(0.5f, -0.5f, -0.5f), vec3(0.5f, -0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f),
	// 上平面位置
	vec3(-0.5f, 0.5f, -0.5f), vec3(0.5f, 0.5f, -0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f, -0.5f),
	// 下平面位置
	vec3(-0.5f, -0.5f, 0.5f), vec3(0.5f, -0.5f, 0.5f), vec3(0.5f, -0.5f, -0.5f), vec3(0.5f, -0.5f, -0.5f), vec3(-0.5f, -0.5f, -0.5f), vec3(-0.5f, -0.5f, 0.5f),
};
vec2 vCubeTexCoords[6] = {vec2(0.0f, 1.0f), vec2(1.0f, 1.0f), vec2(1.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 0.0f), vec2(0.0f, 1.0f)};

vec3 vCubeNormals[6] = { // 立方体法向量
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(-1.0f, 0.0f, 0.0f),
	vec3(1.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, -1.0f, 0.0f)
};

vec3 vGround[6] = { // 地平面坐标
	vec3(-100, -10, -100), vec3(50, -10, -100), vec3(100, -10, 100), vec3(100, -10, 100), vec3(-100, -10, 100), vec3(-100, -10, -100)
};