#pragma once

#include "common_header.h"

/* 载入着色器(shader)文件并编译；创建着色器程序并链接 */
class CShaderProgram {
public:
	void loadShaders(const char* vertexFilePath,const char* fragmentFilePath);
	void useProgram();
	~CShaderProgram();

	/* 设置着色器(shader)中的统一变量(uniform) */
	// 设置矢量
	void setUniform(string sUniformName, vec2* vVectors, int iCount = 1);
	void setUniform(string sUniformName, const vec2 vVector);
	void setUniform(string sUniformName, vec3* vVectors, int iCount = 1);
	void setUniform(string sUniformName, const vec3 vVector);
	void setUniform(string sUniformName, vec4* vVectors, int iCount = 1);
	void setUniform(string sUniformName, const vec4 vVector);
	// 设置浮点数
	void setUniform(string sUniformName, float* fValues, int iCount = 1);
	void setUniform(string sUniformName, const float fValue);
	// 设置 4x4 矩阵
	void setUniform(string sUniformName, mat4* mMatrices, int iCount = 1);
	void setUniform(string sUniformName, const mat4 mMatrix);
	// 设置整型
	void setUniform(string sUniformName, int* iValues, int iCount = 1);
	void setUniform(string sUniformName, const int iValue);

private:
	GLuint m_uiProgramID; // 着色器程序的ID
};