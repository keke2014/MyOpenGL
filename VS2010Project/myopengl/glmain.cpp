
#include "common_header.h"

#include "shader.h"	// ����������ɫ������
#include "texture.h"
#include "vertexBufferObject.h"

#include "renderScene.h"

int main(void)
{
	if( !glfwInit() ) {	// ��ʼ�� GLFW
		fprintf(stderr, "Failed to initialize GLFW. \n");
		return -1;
	}

	// ���ڲ�������
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ��ȡ��ǰ��������
	GLFWvidmode cur_vidmode;
	glfwGetDesktopMode(&cur_vidmode);

	// �򿪴��ڣ�����openGL������
	if( !glfwOpenWindow( cur_vidmode.Width, cur_vidmode.Height, 
		cur_vidmode.RedBits, cur_vidmode.GreenBits, cur_vidmode.BlueBits, 0,
		32,0, GLFW_FULLSCREEN ) ) {
		fprintf(stderr, "Failed to open GLFW window. \n");
		glfwTerminate();
		return -1;
	}

	glfwSetWindowTitle( "my openGL program" );	// ���ô��ڱ���
	glfwSetMousePos(cur_vidmode.Width/2, cur_vidmode.Height/2); // ����ô�������

	GLenum res = glewInit();
	if (res != GLEW_OK) {	// ��ʼ�� GLEW
		fprintf(stderr, "Failed to initialize GLEW: '%s' \n", glewGetErrorString(res));
		return -1;
	}
	printf("GL version: %s \n\n", glGetString(GL_VERSION));

	// ����͸��ͶӰ����
	mat4 mProjectionMatrix = perspective(45.0f, float(cur_vidmode.Width)/float(cur_vidmode.Height), 0.1f, 1000.0f);
	
	initScene();
	cCamera.resetTimer();	// ���������Ⱦѭ�������߻ᵼ�µ�һ�� updateTimer ʱ fFrameInterval ���������岻��ת
	while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED)){ // ����Ⱦѭ��
		cCamera.updateTimer();
		renderScene(mProjectionMatrix);
	}
	releaseScene();

	glfwTerminate();
	return 0;
}

