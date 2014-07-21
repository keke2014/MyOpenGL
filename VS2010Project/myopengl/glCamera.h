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

	float getAngleX(), getAngleY(); // �ӽǻ�ȡ����
	float sof(float fVal);
	void rotateWithMouse();
	void update();

	void resetTimer();
	void updateTimer();
	mat4 look();

public:
	vec3 m_vEye, m_vView, m_vUp;
	float m_fSpeed;
	float m_fSensitivity; // ���ÿ������ת�Ķ��� (0.10��һ���ȽϺõ�ֵ)

private:
	myPoint m_pCur;	// ���������ת
	int m_iForw, m_iBack, m_iLeft, m_iRight;

	clock_t m_tLastFrame;
	float m_fFrameInterval;
};