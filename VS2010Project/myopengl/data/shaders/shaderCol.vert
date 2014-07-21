#version 330

uniform mat4 mNormalMatrix;
uniform mat4 mMVP;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

smooth out vec3 vNormal;

void main()
{
	gl_Position = mMVP*vec4(inPosition, 1.0);
	vec4 vRes = mNormalMatrix*vec4(inNormal, 0.0);
	vNormal = vRes.xyz;
}