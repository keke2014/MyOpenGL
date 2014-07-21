#pragma once

#include "common_header.h"

/* VBO: vertex buffer object */
class CVertexBufferObject {
public:
	void createVBO(GLenum iBufferType = GL_ARRAY_BUFFER);
	~CVertexBufferObject();

	void uploadDataToGPU(int iUsageHint);
	
	void addData(void* ptrData, GLuint uiDataSize);

private:
	GLuint m_uiBuffer;
	GLenum m_iBufferType;
	vector<GLubyte> m_data;
};