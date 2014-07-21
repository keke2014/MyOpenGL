#pragma once

#include "common_header.h"

typedef struct {
		int x;
		int y;
} myPoint;

class CGLCamera {
public:
	CGLCamera();
	CGLCamera(vec3 vEye, vec3 vView, vec3 vUp, float fSpeed, float fSensitivity);
	void setMovingKeys(int iForw, int iBack, int iLeft, int iRight);

	float getAngleX(), getAngleY(); // 视角获取函数
	float sof(float fVal);
	void rotateWithMouse();
	void update();

	void resetTimer();
	void updateTimer();
	mat4 look();

public:
	vec3 m_vEye, m_vView, m_vUp;
	float m_fSpeed;
	float m_fSensitivity; // 鼠标每像素旋转的度数 (0.10是一个比较好的值)

private:
	myPoint m_pCur;	// 用于鼠标旋转
	int m_iForw, m_iBack, m_iLeft, m_iRight;

	clock_t m_tLastFrame;
	float m_fFrameInterval;
};