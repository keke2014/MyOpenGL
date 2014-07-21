
#include "renderScene.h"

#include "shader.h"
#include "texture.h"
#include "static_geometry.h"
#include "vertexBufferObject.h"

#include "skybox.h"

/*
* ���ֶ���
* VBO ���� Vertex Buffer Object�������洢��������
* VAO ���� Vertex Array Object��������֯���VBO��ͬһ��VBO�Ĳ�ͬ����
* 
* ����ת������
* Model Matrix ���� ������ģ�����꣨������Ϊ���ģ�ת��Ϊ�������꣨World Coordinate��
* Normal Matrix ���� ����modelMatrix��������ת�ã������任�����������ڹ���ģ�ͣ�
* View Matrix ���� ��������������ת��Ϊ�����꣨�� Camara Ϊ��׼��
* mProjectionMatrix Matrix ���� ����ʵ��ͶӰ�任
* ��дΪ MVP
*/

GLuint uiVAOs[2];
CVertexBufferObject uiVBO;

CShaderProgram spSkybox, spDirectionalLight, spColored;

#define NUMTEXTURES 5
CTexture tTextures[NUMTEXTURES];

CGLCamera cCamera;
CSkybox sbSkybox;

int iTorusFaces1, iTorusFaces2;

/* ��ʼ�����г��� */
void initScene(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glGenVertexArrays(2, uiVAOs);
	uiVBO.createVBO();

	// ���������������VBO
	for(int i = 0; i < 36; ++i) {
		uiVBO.addData(&vCubeVertices[i], sizeof(vec3));
		uiVBO.addData(&vCubeTexCoords[i%6], sizeof(vec2));
		uiVBO.addData(&vCubeNormals[i/6], sizeof(vec3));
	}
	// �������������VBO
	for(int i = 0; i < 6; ++i) {
		uiVBO.addData(&vGround[i], sizeof(vec3));
		vCubeTexCoords[i] *= 10.0f;
		uiVBO.addData(&vCubeTexCoords[i%6], sizeof(vec2));
		vec3 vGroundNormal(0.0f, 1.0f, 0.0f);
		uiVBO.addData(&vGroundNormal, sizeof(vec3));
	}
	// ��Բ���壨torus������VBO������¼�����������Σ�
	iTorusFaces1 = generateTorus(uiVBO, 7.0f, 2.0f, 20, 20);
	iTorusFaces2 = generateTorus(uiVBO, 10.0f, 6.0f, 10, 10);

	uiVBO.uploadDataToGPU(GL_STATIC_DRAW); // ��VBO���������Դ�

	/* ������Ķ����������� */
	glBindVertexArray(uiVAOs[0]);
	// ���ö���λ������
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3)+sizeof(vec2), 0);
	// ����������������
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(vec3)+sizeof(vec2), (void*)sizeof(vec3));
	// ���÷���������
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3)+sizeof(vec2), (void*)(sizeof(vec3)+sizeof(vec2)));

	/* ������Ķ����������á���͸������ */
	glBindVertexArray(uiVAOs[1]);
	// ���ö���λ������
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3)+sizeof(vec2), 0);
	// ���÷���������
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3)+sizeof(vec2), (void*)(sizeof(vec3)+sizeof(vec2)));

	// ��ȡ��������ɫ����shader��, ������ɫ��������Ӳ�������ɫ��
	spSkybox.loadShaders("data\\shaders\\shaderSky.vert", "data\\shaders\\shaderSky.frag");
	spDirectionalLight.loadShaders("data\\shaders\\shaderTex.vert", "data\\shaders\\shaderTex.frag");
	spColored.loadShaders("data\\shaders\\shaderCol.vert", "data\\shaders\\shaderCol.frag");

	// ��������
	string sTextureNames[] = { "sun.jpg", "grass.jpg", "crate.jpg", "box.jpg", "metalplate.jpg"};
	for(int i = 0; i < NUMTEXTURES; ++i) {
		tTextures[i].loadTexture2D("data\\textures\\"+sTextureNames[i], true);
		tTextures[i].setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
	}

	glEnable(GL_DEPTH_TEST); // ������Ȼ���
	glClearDepth(1.0);
	
	// ���� Camara
	cCamera = CGLCamera(vec3(-30.0f, 0.0f, 100.0f), vec3(15.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 25.0f, 0.1f);
	cCamera.setMovingKeys('W', 'S', 'A', 'D');
	// ���� Skybox
	sbSkybox.loadSkybox("data\\skybox\\", "front.jpg", "back.jpg", "left.jpg", "right.jpg", "up.jpg", "down.jpg");
}

/* ��Ⱦ���г��� */

float fGlobalAngle = 0;	 // ���ڿ���������ת
float fSunAngle = 15.0f; // ̫����ʼλ��

vec4 vBoxColors[] = {
	vec4(1.0f, 0.0f, 0.0f, 0.5f), // ��
	vec4(0.0f, 1.0f, 0.0f, 0.5f), // ��
	vec4(0.0f, 0.0f, 1.0f, 0.5f), // ��
};

void renderScene(mat4& mProjectionMatrix)
{
	// ����̫��λ�ó�ʼ��������ɫ
	float fSine = sin(fSunAngle*PI/180.0f);
	glClearColor(0.0f, std::max(0.0f, 0.9f*fSine), std::max(0.0f, 0.9f*fSine), 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vec3 vSunPos(cos(fSunAngle*PI/180.0)*140, sin(fSunAngle*PI/180.0)*140, 0.0);
	mat4 mViewMatrix = cCamera.look();
	mat4 mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	mat4 mModelMatrix;

	glEnable(GL_TEXTURE_2D);

	// ��Ⱦ Skybox
	spSkybox.useProgram();
	spSkybox.setUniform("gSampler", 0);
	mModelMatrix = translate(mat4(1.0), cCamera.m_vEye);
	spSkybox.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
	sbSkybox.renderSkybox();

	/* ��Ⱦ�������� */
	glBindVertexArray(uiVAOs[0]);
	spDirectionalLight.useProgram();
	spDirectionalLight.setUniform("gSampler", 0);	
	spDirectionalLight.setUniform("sunLight.vDirection", -normalize(vSunPos));
	spDirectionalLight.setUniform("sunLight.vColor", vec3(1.0f, 1.0f, 1.0f));
	spDirectionalLight.setUniform("sunLight.fAmbientIntensity", 0.3f);
	
	// ��Ⱦ���
	tTextures[1].bindTexture();
	mModelMatrix = mat4(1.0);
	spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
	glDrawArrays(GL_TRIANGLES, 36, 6);

	// ��Ⱦ���ӶѳɵĽ���
	tTextures[2].bindTexture();
	for (int nb = 1; nb <= 9; ++nb) {
		int iCnt = nb > 5 ? 10-nb : nb;
		for (int i = 0; i < iCnt; ++i) {
			vec3 vPos = vec3(-20.0f+nb*8.02f, -6.0f+i*8.02f, -50.0f);
			mModelMatrix = translate(mat4(1.0), vPos);
			mModelMatrix = scale(mModelMatrix, vec3(8.0f, 8.0f, 8.0f));
			// ���÷������ı任���� ���� Model�����������ת��
			spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
			spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	// ��Ⱦ������ת�������������
	tTextures[3].bindTexture();
	for (int i = 0; i < 3; ++i) {
		float fSign = -1.0f+float(i%2)*2.0f; // fSign = -1.0f �� 1.0f
		vec3 vPos = vec3(fSign*15.0f, 0.0f, 50.0f-float(i)*25.0f);
		mModelMatrix = translate(mat4(1.0), vPos);
		mModelMatrix = scale(mModelMatrix, vec3(8.0f, 8.0f, 8.0f));
		mModelMatrix = rotate(mModelMatrix, fGlobalAngle+i*50.0f, vec3(0.0f, 1.0f, 0.0f));
		// ���÷������ı任���� ���� Model�����������ת��
		spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
		spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// ��Ⱦ����Բ������ɵ���������
	tTextures[4].bindTexture();
	vec3 vPos = vec3(0.0f, 0.0, 40.0f);

	mModelMatrix = translate(mat4(1.0), vPos);
	mModelMatrix = rotate(mModelMatrix, fGlobalAngle, vec3(0.0f, 1.0f, 0.0f));
	spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
	spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
	glDrawArrays(GL_TRIANGLES, 42, iTorusFaces1*3);

	mModelMatrix = translate(mat4(1.0), vPos);
	mModelMatrix = rotate(mModelMatrix, fGlobalAngle, vec3(0.0f, 1.0f, 0.0f));
	mModelMatrix = rotate(mModelMatrix, 90.0f, vec3(1.0f, 0.0f, 0.0f));
	spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
	spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
	glDrawArrays(GL_TRIANGLES, 42, iTorusFaces1*3);

	mModelMatrix = translate(mat4(1.0), vPos);
	mModelMatrix = rotate(mModelMatrix, fGlobalAngle+90.0f, vec3(0.0f, 1.0f, 0.0f));
	spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
	spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
	glDrawArrays(GL_TRIANGLES, 42, iTorusFaces1*3);

	// ��Ⱦ̫��
	tTextures[0].bindTexture();
	mModelMatrix = translate(mat4(1.0), vSunPos);
	spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
	spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
	spDirectionalLight.setUniform("sunLight.fAmbientIntensity", 0.8f); // ̫���������
	glDrawArrays(GL_TRIANGLES, 42+iTorusFaces1*3, iTorusFaces2*3);

	// ��Ⱦ����͸���Ĳ�ɫ��ת����
	glDisable(GL_TEXTURE_2D); // �ر�����
	glBindVertexArray(uiVAOs[1]);
	spColored.useProgram();
	spColored.setUniform("sunLight.vColor", vec3(1.0f, 1.0f, 1.0f));
	spColored.setUniform("sunLight.fAmbientIntensity", 0.3f);
	spColored.setUniform("sunLight.vDirection", -normalize(vSunPos));
	
	glEnable(GL_BLEND); // ������ɫ���
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(0); // ��ֹд����Ȼ��棨͸�����ӿ���͸���������壩
	for (int i = 0; i < 3; ++i) {
		float fSign = 1.0f-float(i%2)*2.0f; // fSign = -1.0f �� 1.0f
		vec3 vPos = vec3(fSign*15.0f, 0.0f, 50.0f-float(i)*25.0f);
		mModelMatrix = translate(mat4(1.0), vPos);
		mModelMatrix = scale(mModelMatrix, vec3(8.0f, 8.0f, 8.0f));
		mModelMatrix = rotate(mModelMatrix, fGlobalAngle*0.8f+i*30.0f, vec3(1.0f, 0.0f, 0.0f));
		spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
		spColored.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
		spColored.setUniform("vColor", vBoxColors[i]); // ���ò�ͬ��ɫ
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glDisable(GL_BLEND);
	glDepthMask(1); // �ָ���Ȼ���

	// ��������
	fGlobalAngle += cCamera.sof(100.0f);
	cCamera.update();

	// ���ҷ��������̫��λ��
	if(glfwGetKey(GLFW_KEY_LEFT)) fSunAngle += cCamera.sof(45.0f);
	if(glfwGetKey(GLFW_KEY_RIGHT)) fSunAngle -= cCamera.sof(45.0f);

	glfwSwapBuffers(); // ����ǰ�󻺴�
}

void releaseScene(void)
{
	glDeleteVertexArrays(2, uiVAOs);
}