#pragma once
#include <d3d11.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <math.h>

class Camera
{

private: 
	float m_x,m_y,m_z,m_dx,m_dz;
	float m_camera_rotation = 0;

	
	XMVECTOR m_position, m_lookat, m_up;


public:
	
	Camera(float x, float y, float z, float camera_rotation);
	void Rotate(float rotaiton);
	void Forward(float step);
	void Up(float step);
	XMMATRIX GetViewMatrix();
};