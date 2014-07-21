
#include "common_header.h"

#include "shader.h"	// 用于载入着色器代码
#include "texture.h"
#include "vertexBufferObject.h"

#include "renderScene.h"

int main(void)
{
	if( !glfwInit() ) {	// 初始化 GLFW
		fprintf(stderr, "Failed to initialize GLFW. \n");
		return -1;
	}

	// 窗口参数设置
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 获取当前桌面设置
	GLFWvidmode cur_vidmode;
	glfwGetDesktopMode(&cur_vidmode);

	// 打开窗口，建立openGL上下文
	if( !glfwOpenWindow( cur_vidmode.Width, cur_vidmode.Height, 
		cur_vidmode.RedBits, cur_vidmode.GreenBits, cur_vidmode.BlueBits, 0,
		32,0, GLFW_FULLSCREEN ) ) {
		fprintf(stderr, "Failed to open GLFW window. \n");
		glfwTerminate();
		return -1;
	}

	glfwSetWindowTitle( "my openGL program" );	// 设置窗口标题
	glfwSetMousePos(cur_vidmode.Width/2, cur_vidmode.Height/2); // 鼠标置窗口中央

	GLenum res = glewInit();
	if (res != GLEW_OK) {	// 初始化 GLEW
		fprintf(stderr, "Failed to initialize GLEW: '%s' \n", glewGetErrorString(res));
		return -1;
	}
	printf("GL version: %s \n\n", glGetString(GL_VERSION));

	// 设置透视投影矩阵
	mat4 mProjectionMatrix = perspective(45.0f, float(cur_vidmode.Width)/float(cur_vidmode.Height), 0.1f, 1000.0f);
	
	initScene();
	cCamera.resetTimer();	// 必须紧靠渲染循环，否者会导致第一次 updateTimer 时 fFrameInterval 过大，立方体不旋转
	while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED)){ // 主渲染循环
		cCamera.updateTimer();
		renderScene(mProjectionMatrix);
	}
	releaseScene();

	glfwTerminate();
	return 0;
}

