#include <d3d11.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <math.h>
#include "camera.h"

 Camera::Camera(float x,float y,float z,float camera_rotation)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_camera_rotation = XMConvertToRadians(camera_rotation);

	
	
}
void Camera::Rotate(float rotation)
{
	

	m_camera_rotation = m_camera_rotation + rotation;
}

void Camera::Forward(float Step)		
{										
	m_z += Step*m_dz;
}
void Camera::Right(float step)			//seems to be working untill you rotate
{										// instead of still moving toward where the cmaera is moveing
	m_x += step;						//it will still move on its original path. 
}
void Camera::Up(float step)
{
	m_y += step;
}
XMMATRIX Camera::GetViewMatrix()
{

	m_dx = sin(XMConvertToRadians((m_camera_rotation)* (XM_PI / 180.00)));
	m_dz = cos(XMConvertToRadians((m_camera_rotation)* (XM_PI / 180.00)));

	XMVECTOR positioin = XMVectorSet(m_x, m_y, m_z, 0.0);
	XMVECTOR lookat = XMVectorSet(m_x + m_dx, m_y, m_z + m_dz, 0.0);
	XMVECTOR up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

	XMMATRIX view = XMMatrixLookAtLH(positioin, lookat, up);
	return view;
}