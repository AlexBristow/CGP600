#include "Model.h"
#pragma once
#include <d3d11.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <math.h>
#include "objfilemodel.h"

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection;	//64bytes
};

Model::Model(ID3D11Device* device, ID3D11DeviceContext* immediateContext)
{
	m_pD3DDevice = device;
	m_pImmediateContext = immediateContext;
	m_x = 0.0f, 
	m_y = 0.0f,
	m_z = 0.0f;
	m_xangle = 0.0f,
	m_zangle = 0.0f,
	m_yangle = 0.0f;
	m_scale = 1.0f;

}

void Model::SetX(float x)
{
	m_x = x;
}

void Model::SetY(float y)
{
	m_y = y;
}

void Model::SetZ(float z)
{
	m_z = z;
}

void Model::SetXAngle(float xAngle)
{
	m_xangle = xAngle;
}

void Model::SetYAngle(float yAngle)
{
	m_yangle = yAngle;
}

void Model::SetZAngle(float zAngle)
{
	m_zangle = zAngle;
}

void Model::SetScale(float scale)
{
	m_scale = scale;
}
float Model::GetX(void)
{
	return m_x;
}

float Model::GetY(void)
{
	return m_y;
}

float Model::GetZ(void)
{
	return m_z;
}

float Model::GetXAngle(void)
{
	return m_xangle;
}
float Model::GetYAngle(void)
{
	return m_yangle;
}

float Model::GetZAngle(void)
{
	return m_zangle;
}

float Model::GetScale(void)
{
	return m_scale;
}

void Model::ChangeXPos(float step)
{
	m_x += step;
}

void Model::ChangeYPos(float step)
{
	m_y += step;
}

void Model::ChangeZPos(float step)
{
	m_z += step;
}

void Model::ChangeXAngle(float angle)
{
	m_xangle += angle;
}

void Model::ChangeYAngle(float angle)
{
	m_yangle += angle;
}

void Model::ChangeZAngle(float angle)
{
	m_zangle += angle;
}

void Model::ChangeScale(float scale)
{
	m_scale += scale;
}

int Model::LoadObjModel(char* filename)
{	
	HRESULT hr = S_OK;
	m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);
	if (m_pObject->filename == "FILE NOT LOADED") return S_FALSE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ID3D10Blob *VSModel, *PSModel, *errorModel;
	
	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VSModel, &errorModel, 0);
	if (errorModel != 0) //check for shader compliation error 
	{
		OutputDebugStringA((char*)errorModel->GetBufferPointer());
		errorModel->Release();
		//Error handling 
		if (FAILED(hr)) return hr;
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PSModel, &errorModel, 0);
	if (errorModel != 0) //check for shader compliation error 
	{
		OutputDebugStringA((char*)errorModel->GetBufferPointer());
		errorModel->Release();
			//Error handling 
			if (FAILED(hr)) return hr;
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//create shader objects 
	hr = m_pD3DDevice->CreateVertexShader(VSModel->GetBufferPointer(), VSModel->GetBufferSize(), NULL, &m_pVShader);
	//Error handling 
	if (FAILED(hr)) return hr;
	hr = m_pD3DDevice->CreatePixelShader(PSModel->GetBufferPointer(), PSModel->GetBufferSize(), NULL, &m_pPShader);
	//Error handling 
	if (FAILED(hr)) return hr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Create and set the input layout Object
	D3D11_INPUT_ELEMENT_DESC iedescModel[] =
	{
		{ "POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};
	hr = m_pD3DDevice->CreateInputLayout(iedescModel, ARRAYSIZE(iedescModel), VSModel->GetBufferPointer(), VSModel->GetBufferSize(), &m_pInputlayout);
	if (FAILED(hr)) return hr;
			
// creation of the constant buffer 
	D3D11_BUFFER_DESC Model_constant_buffer_desc;
	ZeroMemory(&Model_constant_buffer_desc, sizeof(Model_constant_buffer_desc));
	Model_constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; // can use updatesubresroces() to update
	Model_constant_buffer_desc.ByteWidth = 64; // MUSt be a mutilple of 16, calculate from the CB struct
	Model_constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // use as a constant buffer
	hr = m_pD3DDevice->CreateBuffer(&Model_constant_buffer_desc, NULL, &m_pConstantBuffer);
	//error handling 
	if (FAILED(hr)) return hr;
	
}

void Model::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	XMMATRIX World;
	float m_rotation;
	float m_scale = 1.0f;
	float m_x = 0.0f;
	float m_y = 0.0f;
	float m_z = 0.0f ;

	World = XMMatrixScaling(m_scale, m_scale, m_scale);
	World *= XMMatrixRotationX(XMConvertToRadians(m_xangle));
	World *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	World *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));
	World *= XMMatrixTranslation(m_x, m_y, m_x);
	

	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = World*(*view)*(*projection);
																			//Problem with ithis here is that i cant figure out why the immiadiate context is null 
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);	//Set the shader object as active 
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputlayout);

	m_pObject->Draw();
}
Model::~Model()
{
	if (m_pObject)			delete m_pObject;
	if (m_pConstantBuffer)	m_pConstantBuffer->Release();
	if (m_pVShader)			m_pVShader->Release();
	if (m_pPShader)			m_pPShader->Release();

}