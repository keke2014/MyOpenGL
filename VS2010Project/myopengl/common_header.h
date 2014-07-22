#pragma once

/*
* 通用头文件
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

#include "./glew-1.9.0/include/GL/glew.h"	// gl扩展处理库
#include "./glfw-2.7.6/include/GL/glfw.h"	// 窗口、鼠标、按键等交互管理库，必须在glew后

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "./glew-1.9.0/lib/glew32.lib") // 动态库
#pragma comment(lib, "./glfw-2.7.6/lib/GLFW.lib") // 静态库

// gl数学库，用于矩阵处理，如各种变换
#include "./glm-0.9.4.0/glm/glm.hpp"
#include "./glm-0.9.4.0/glm/gtc/matrix_transform.hpp"
#include "./glm-0.9.4.0/glm/gtx/rotate_vector.hpp"
using namespace glm;

const float PI = atan(1.0f)*4.0f;

#include "./FreeImage/Dist/FreeImage.h"
#pragma comment(lib, "./FreeImage/Dist/FreeImage.lib")
