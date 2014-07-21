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

#include <GL/glew.h>	// gl扩展处理库
#include <GL/glfw.h>	// 窗口、鼠标、按键等交互管理库，必须在glew后

// gl数学库，用于矩阵处理，如各种变换
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

const float PI = atan(1.0f)*4.0f;