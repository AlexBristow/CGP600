
#pragma once
#include <d3d11.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <math.h>
#include "objfilemodel.h"

class Model
{
private:

	ID3D11Device*				m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;
	
	
	ObjFileModel*				m_pObject;
	ID3D11VertexShader*			m_pVShader;
	ID3D11PixelShader*			m_pPShader;
	ID3D11InputLayout*			m_pInputlayout;
	ID3D11Buffer*				m_pConstantBuffer;

	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;

public:

	Model(ID3D11Device* device, ID3D11DeviceContext* immediateContext);
	int LoadObjModel(char* filename);
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	void SetX(float num);
	void SetY(float num);
	void SetZ(float num);
	void SetXAngle(float num);
	void SetYAngle(float num);
	void SetZAngle(float num);
	void SetScale(float num);
	float GetX(void);
	float GetY(void);
	float GetZ(void);
	float GetXAngle(void);
	float GetYAngle(void);
	float GetZAngle(void);
	float GetScale(void);
	void  ChangeXPos(float increment);
	void  ChangeYPos(float increment);
	void  ChangeZPos(float increment);
	void  ChangeXAngle(float Angle);
	void  ChangeYAngle(float Angle);
	void  ChangeZAngle(float Angle);
	void  ChangeScale(float scale);
	~Model();





};