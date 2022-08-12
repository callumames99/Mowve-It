
#pragma once

#include <Windows.h>
#include <d3dx9.h>



class CString
{
public:
	CString();
	CString(LPSTR);
	~CString();

	LPSTR GetString();

	LPSTR operator = (LPSTR);
	char operator [] (DWORD);

private:
	HANDLE hHeap;
	LPSTR string;
};



class CMutex
{
public:
	CMutex();

	void Lock();
	void Unlock();
	bool Try();
	bool IsLocked();

private:
	volatile DWORD _lock;
};



class CCamera
{
public:
	CCamera();

	void GetPosition(D3DXVECTOR3 *pOut);
	void SetPosition(D3DXVECTOR3 *pPos);
	void SetPosition(float X, float Y, float Z);
	void SetFocus(D3DXVECTOR3 *pPoint);
	void SetFocus(float X, float Y, float Z);

	void BeginAnimate(float Speed);
	void EndAnimate();

	void Update();

	void BuildViewMatrix(D3DXMATRIX *pOut);

private:
	D3DXVECTOR3 vecPosAnimStart;
	D3DXVECTOR3 vecPosAnimEnd;
	D3DXVECTOR3 vecPosition;
	D3DXVECTOR3 vecFocAnimStart;
	D3DXVECTOR3 vecFocAnimEnd;
	D3DXVECTOR3 vecFocus;
	float fAnimSpeed;
	float fAnimPos;
};

