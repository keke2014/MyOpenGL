#pragma once

/*
* ͨ��ͷ�ļ�
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;

#include "./glew-1.9.0/include/GL/glew.h"	// gl��չ�����
#include "./glfw-2.7.6/include/GL/glfw.h"	// ���ڡ���ꡢ�����Ƚ�������⣬������glew��

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "./glew-1.9.0/lib/glew32.lib") // ��̬��
#pragma comment(lib, "./glfw-2.7.6/lib/GLFW.lib") // ��̬��

// gl��ѧ�⣬���ھ���������ֱ任
#include "./glm-0.9.4.0/glm/glm.hpp"
#include "./glm-0.9.4.0/glm/gtc/matrix_transform.hpp"
#include "./glm-0.9.4.0/glm/gtx/rotate_vector.hpp"
using namespace glm;

const float PI = atan(1.0f)*4.0f;

#include "./FreeImage/Dist/FreeImage.h"
#pragma comment(lib, "./FreeImage/Dist/FreeImage.lib")
