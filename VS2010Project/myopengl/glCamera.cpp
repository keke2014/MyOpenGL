
#include "glCamera.h"

CGLCamera::CGLCamera()
{
	m_vEye = vec3(0.0f, 0.0f, 0.0f);
	m_vView = vec3(0.0f, 0.0, -1.0f);
	m_vUp = vec3(0.0f, 1.0f, 0.0f);
	m_fSpeed = 25.0f;
	m_fSensitivity = 0.1f;
}

CGLCamera::CGLCamera(vec3 vEye, vec3 vView, vec3 vUp, float fSpeed, float fSensitivity)
{
	m_vEye = vEye; m_vView = vView; m_vUp = vUp;
	m_fSpeed = fSpeed;
	m_fSensitivity = fSensitivity;
}

/* ���� Camara ��ǰ�����ҿ��Ƽ�*/
void CGLCamera::setMovingKeys(int iForw, int iBack, int iLeft, int iRight)
{
	m_iForw = iForw;
	m_iBack = iBack;
	m_iLeft = iLeft;
	m_iRight = iRight;
}

/* �������ƶ�����תCamara */
void CGLCamera::rotateWithMouse()
{
	myPoint winSize;
	glfwGetWindowSize(&winSize.x, &winSize.y);
	int iCentX = winSize.x >> 1;
	int iCentY = winSize.y >> 1;

	glfwGetMousePos(&m_pCur.x, &m_pCur.y);

	float deltaX = (float)(iCentX-m_pCur.x) * m_fSensitivity;
	float deltaY = (float)(iCentY-m_pCur.y) * m_fSensitivity;

	if(deltaX != 0.0f) { // ���ˮƽ�ƶ� ���� �� y ����ת
		m_vView -= m_vEye;
		m_vView = rotate(m_vView, deltaX, vec3(0.0f, 1.0f, 0.0f));
		m_vView += m_vEye;
	}
	if(deltaY != 0.0f) {
		vec3 vAxis = cross(m_vView-m_vEye, m_vUp);
		vAxis = normalize(vAxis);
		float fAngle = deltaY;
		float fNewAngle = fAngle + getAngleX();
		if(fNewAngle > -89.80f && fNewAngle < 89.80f) {
			m_vView -= m_vEye;
			m_vView = rotate(m_vView, deltaY, vAxis);
			m_vView += m_vEye;
		}
	}
	glfwSetMousePos(iCentX, iCentY); // ������굽��������
}

/* ��ȡˮƽ�ӽ� */
float CGLCamera::getAngleY()
{
	vec3 vDir = m_vView-m_vEye; vDir.y = 0.0f;
	normalize(vDir);
	float fAngle = acos(dot(vec3(0, 0, -1), vDir)) * (180.0f/PI);
	if(vDir.x < 0) fAngle = 360.0f-fAngle;
	return fAngle;
}

/* ��ȡ��ֱ�ӽ� */
float CGLCamera::getAngleX()
{
	vec3 vDir = m_vView-m_vEye;
	vDir = normalize(vDir);
	vec3 vDir2 = vDir; vDir2.y = 0.0f;
	vDir2 = normalize(vDir2);
	float fAngle =  acos(dot(vDir2, vDir)) * (180.0f/PI);
	if(vDir.y < 0) fAngle *= -1.0f;
	return fAngle;
}

/* sof: speed optimized float�� �����ƶ����� */
float CGLCamera::sof(float fVal)
{
	return fVal*m_fFrameInterval;
}

/* �ƶ���ת�����ƶ�*/
void CGLCamera::update()
{
	rotateWithMouse();

	// ��ȡ�ƶ�����
	vec3 vMove = m_vView-m_vEye;
	vMove = normalize(vMove);
	vMove *= m_fSpeed;

	vec3 vStrafe = cross(m_vView-m_vEye, m_vUp);
	vStrafe = normalize(vStrafe);
	vStrafe *= m_fSpeed;

	int iMove = 0;
	vec3 vMoveBy;

	if(glfwGetKey(m_iForw)) vMoveBy += vMove*sof(1.0f);
	if(glfwGetKey(m_iBack)) vMoveBy -= vMove*sof(1.0f);
	if(glfwGetKey(m_iLeft)) vMoveBy -= vStrafe*sof(1.0f);
	if(glfwGetKey(m_iRight)) vMoveBy += vStrafe*sof(1.0f);
	m_vEye += vMoveBy; m_vView += vMoveBy;
}

void CGLCamera::resetTimer()
{
	m_tLastFrame = clock();
	m_fFrameInterval = 0.0f;
}

void CGLCamera::updateTimer()
{
	clock_t tCur = clock();
	m_fFrameInterval = float(tCur-m_tLastFrame) / float(CLOCKS_PER_SEC);
	m_tLastFrame = tCur;
}

/* ���� modelview ���� */
mat4 CGLCamera::look()
{
	return lookAt(m_vEye, m_vView, m_vUp);
}