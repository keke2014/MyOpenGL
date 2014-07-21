
#include "renderScene.h"

#include "shader.h"
#include "texture.h"
#include "static_geometry.h"
#include "vertexBufferObject.h"

#include "skybox.h"

/*
* 两种对象：
* VBO —— Vertex Buffer Object，用来存储各种坐标
* VAO —— Vertex Array Object，用来组织多个VBO或同一个VBO的不同部分
* 
* 几个转换矩阵：
* Model Matrix —— 用来将模型坐标（以物体为中心）转换为世界坐标（World Coordinate）
* Normal Matrix —— 等于modelMatrix的逆矩阵的转置，用来变换法向量（用于光照模型）
* View Matrix —— 用来将世界坐标转换为视坐标（以 Camara 为基准）
* mProjectionMatrix Matrix —— 用来实现投影变换
* 合写为 MVP
*/

GLuint uiVAOs[2];
CVertexBufferObject uiVBO;

CShaderProgram spSkybox, spDirectionalLight, spColored;

#define NUMTEXTURES 5
CTexture tTextures[NUMTEXTURES];

CGLCamera cCamera;
CSkybox sbSkybox;

int iTorusFaces1, iTorusFaces2;

/* 初始化所有场景 */
void initScene(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glGenVertexArrays(2, uiVAOs);
	uiVBO.createVBO();

	// 将立方体坐标加入VBO
	for(int i = 0; i < 36; ++i) {
		uiVBO.addData(&vCubeVertices[i], sizeof(vec3));
		uiVBO.addData(&vCubeTexCoords[i%6], sizeof(vec2));
		uiVBO.addData(&vCubeNormals[i/6], sizeof(vec3));
	}
	// 将地面坐标加入VBO
	for(int i = 0; i < 6; ++i) {
		uiVBO.addData(&vGround[i], sizeof(vec3));
		vCubeTexCoords[i] *= 10.0f;
		uiVBO.addData(&vCubeTexCoords[i%6], sizeof(vec2));
		vec3 vGroundNormal(0.0f, 1.0f, 0.0f);
		uiVBO.addData(&vGroundNormal, sizeof(vec3));
	}
	// 将圆环体（torus）加入VBO，并记录面数（三角形）
	iTorusFaces1 = generateTorus(uiVBO, 7.0f, 2.0f, 20, 20);
	iTorusFaces2 = generateTorus(uiVBO, 10.0f, 6.0f, 10, 10);

	uiVBO.uploadDataToGPU(GL_STATIC_DRAW); // 将VBO数据载入显存

	/* 有纹理的顶点属性设置 */
	glBindVertexArray(uiVAOs[0]);
	// 设置顶点位置属性
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3)+sizeof(vec2), 0);
	// 设置纹理坐标属性
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(vec3)+sizeof(vec2), (void*)sizeof(vec3));
	// 设置法向量属性
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3)+sizeof(vec2), (void*)(sizeof(vec3)+sizeof(vec2)));

	/* 无纹理的顶点属性设置——透明箱子 */
	glBindVertexArray(uiVAOs[1]);
	// 设置顶点位置属性
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3)+sizeof(vec2), 0);
	// 设置法向量属性
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3)+sizeof(vec2), (void*)(sizeof(vec3)+sizeof(vec2)));

	// 读取并编译着色器（shader）, 创建着色器程序，添加并链接着色器
	spSkybox.loadShaders("data\\shaders\\shaderSky.vert", "data\\shaders\\shaderSky.frag");
	spDirectionalLight.loadShaders("data\\shaders\\shaderTex.vert", "data\\shaders\\shaderTex.frag");
	spColored.loadShaders("data\\shaders\\shaderCol.vert", "data\\shaders\\shaderCol.frag");

	// 载入纹理
	string sTextureNames[] = { "sun.jpg", "grass.jpg", "crate.jpg", "box.jpg", "metalplate.jpg"};
	for(int i = 0; i < NUMTEXTURES; ++i) {
		tTextures[i].loadTexture2D("data\\textures\\"+sTextureNames[i], true);
		tTextures[i].setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
	}

	glEnable(GL_DEPTH_TEST); // 开启深度缓存
	glClearDepth(1.0);
	
	// 设置 Camara
	cCamera = CGLCamera(vec3(-30.0f, 0.0f, 100.0f), vec3(15.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 25.0f, 0.1f);
	cCamera.setMovingKeys('W', 'S', 'A', 'D');
	// 载入 Skybox
	sbSkybox.loadSkybox("data\\skybox\\", "front.jpg", "back.jpg", "left.jpg", "right.jpg", "up.jpg", "down.jpg");
}

/* 渲染所有场景 */

float fGlobalAngle = 0;	 // 用于控制箱子旋转
float fSunAngle = 15.0f; // 太阳初始位置

vec4 vBoxColors[] = {
	vec4(1.0f, 0.0f, 0.0f, 0.5f), // 红
	vec4(0.0f, 1.0f, 0.0f, 0.5f), // 绿
	vec4(0.0f, 0.0f, 1.0f, 0.5f), // 蓝
};

void renderScene(mat4& mProjectionMatrix)
{
	// 根据太阳位置初始化背景颜色
	float fSine = sin(fSunAngle*PI/180.0f);
	glClearColor(0.0f, std::max(0.0f, 0.9f*fSine), std::max(0.0f, 0.9f*fSine), 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vec3 vSunPos(cos(fSunAngle*PI/180.0)*140, sin(fSunAngle*PI/180.0)*140, 0.0);
	mat4 mViewMatrix = cCamera.look();
	mat4 mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	mat4 mModelMatrix;

	glEnable(GL_TEXTURE_2D);

	// 渲染 Skybox
	spSkybox.useProgram();
	spSkybox.setUniform("gSampler", 0);
	mModelMatrix = translate(mat4(1.0), cCamera.m_vEye);
	spSkybox.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
	sbSkybox.renderSkybox();

	/* 渲染其他物体 */
	glBindVertexArray(uiVAOs[0]);
	spDirectionalLight.useProgram();
	spDirectionalLight.setUniform("gSampler", 0);	
	spDirectionalLight.setUniform("sunLight.vDirection", -normalize(vSunPos));
	spDirectionalLight.setUniform("sunLight.vColor", vec3(1.0f, 1.0f, 1.0f));
	spDirectionalLight.setUniform("sunLight.fAmbientIntensity", 0.3f);
	
	// 渲染大地
	tTextures[1].bindTexture();
	mModelMatrix = mat4(1.0);
	spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
	glDrawArrays(GL_TRIANGLES, 36, 6);

	// 渲染箱子堆成的阶梯
	tTextures[2].bindTexture();
	for (int nb = 1; nb <= 9; ++nb) {
		int iCnt = nb > 5 ? 10-nb : nb;
		for (int i = 0; i < iCnt; ++i) {
			vec3 vPos = vec3(-20.0f+nb*8.02f, -6.0f+i*8.02f, -50.0f);
			mModelMatrix = translate(mat4(1.0), vPos);
			mModelMatrix = scale(mModelMatrix, vec3(8.0f, 8.0f, 8.0f));
			// 设置法向量的变换矩阵 —— Model矩阵的逆矩阵的转置
			spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
			spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	// 渲染三个旋转的有纹理的箱子
	tTextures[3].bindTexture();
	for (int i = 0; i < 3; ++i) {
		float fSign = -1.0f+float(i%2)*2.0f; // fSign = -1.0f 或 1.0f
		vec3 vPos = vec3(fSign*15.0f, 0.0f, 50.0f-float(i)*25.0f);
		mModelMatrix = translate(mat4(1.0), vPos);
		mModelMatrix = scale(mModelMatrix, vec3(8.0f, 8.0f, 8.0f));
		mModelMatrix = rotate(mModelMatrix, fGlobalAngle+i*50.0f, vec3(0.0f, 1.0f, 0.0f));
		// 设置法向量的变换矩阵 —— Model矩阵的逆矩阵的转置
		spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
		spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// 渲染三个圆环面组成的特殊物体
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

	// 渲染太阳
	tTextures[0].bindTexture();
	mModelMatrix = translate(mat4(1.0), vSunPos);
	spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
	spDirectionalLight.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
	spDirectionalLight.setUniform("sunLight.fAmbientIntensity", 0.8f); // 太阳本身很亮
	glDrawArrays(GL_TRIANGLES, 42+iTorusFaces1*3, iTorusFaces2*3);

	// 渲染三个透明的彩色旋转箱子
	glDisable(GL_TEXTURE_2D); // 关闭纹理
	glBindVertexArray(uiVAOs[1]);
	spColored.useProgram();
	spColored.setUniform("sunLight.vColor", vec3(1.0f, 1.0f, 1.0f));
	spColored.setUniform("sunLight.fAmbientIntensity", 0.3f);
	spColored.setUniform("sunLight.vDirection", -normalize(vSunPos));
	
	glEnable(GL_BLEND); // 开启颜色混合
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(0); // 禁止写入深度缓存（透明箱子可以透视其他物体）
	for (int i = 0; i < 3; ++i) {
		float fSign = 1.0f-float(i%2)*2.0f; // fSign = -1.0f 或 1.0f
		vec3 vPos = vec3(fSign*15.0f, 0.0f, 50.0f-float(i)*25.0f);
		mModelMatrix = translate(mat4(1.0), vPos);
		mModelMatrix = scale(mModelMatrix, vec3(8.0f, 8.0f, 8.0f));
		mModelMatrix = rotate(mModelMatrix, fGlobalAngle*0.8f+i*30.0f, vec3(1.0f, 0.0f, 0.0f));
		spDirectionalLight.setUniform("mNormalMatrix", transpose(inverse(mModelMatrix)));
		spColored.setUniform("mMVP", mViewProjectionMatrix*mModelMatrix);
		spColored.setUniform("vColor", vBoxColors[i]); // 设置不同颜色
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glDisable(GL_BLEND);
	glDepthMask(1); // 恢复深度缓存

	// 更新数据
	fGlobalAngle += cCamera.sof(100.0f);
	cCamera.update();

	// 左右方向键控制太阳位置
	if(glfwGetKey(GLFW_KEY_LEFT)) fSunAngle += cCamera.sof(45.0f);
	if(glfwGetKey(GLFW_KEY_RIGHT)) fSunAngle -= cCamera.sof(45.0f);

	glfwSwapBuffers(); // 交换前后缓存
}

void releaseScene(void)
{
	glDeleteVertexArrays(2, uiVAOs);
}