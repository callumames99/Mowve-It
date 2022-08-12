


#include "CStruct.h"



CString::CString()
{
	this->hHeap = GetProcessHeap();
	this->string = nullptr;
}
CString::CString(LPSTR string)
{
	this->hHeap = GetProcessHeap();
	DWORD dwLen = strlen(string)+1;
	this->string = (LPSTR)HeapAlloc( this->hHeap, 0, dwLen );
	memcpy( this->string, string, dwLen );
}
CString::~CString()
{
	if( this->string )
		HeapFree( this->hHeap, 0, this->string );
}
LPSTR CString::operator = (LPSTR string)
{
	DWORD dwLen = strlen(string)+1;
	if( this->string )
		HeapFree( this->hHeap, 0, this->string );
	this->string = (LPSTR)HeapAlloc( this->hHeap, 0, dwLen );
	if( this->string ) {
		memcpy( this->string, string, dwLen );
		return this->string;
	} else return nullptr;
}
char CString::operator [] (DWORD member)
{
	DWORD dwLen = strlen(this->string);
	if( member > dwLen ) return 0;
	else return this->string[member];
}
LPSTR CString::GetString()
{
	return this->string;
}



CMutex::CMutex()
{
	this->_lock = 0;
}
void CMutex::Lock()
{
	while( InterlockedExchange( &this->_lock, 1 ) == 1 );
}
bool CMutex::Try()
{
	if( InterlockedExchange( &this->_lock, 1 ) == 0 ) return true;
	else return false;
}
void CMutex::Unlock()
{
	this->_lock = 0;
}
bool CMutex::IsLocked()
{
	return (bool)this->_lock;
}



CCamera::CCamera()
{
	this->vecPosAnimStart = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	this->vecPosAnimEnd = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
	this->vecFocAnimStart = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	this->vecFocAnimEnd = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
	this->vecFocus = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	this->vecPosition = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
	this->fAnimPos = 1.0f;
	this->fAnimSpeed = 0.0f;
}
void CCamera::GetPosition(D3DXVECTOR3 *pOut)
{
	*pOut = this->vecPosition;
}
void CCamera::SetPosition(D3DXVECTOR3 *pPos)
{
	this->vecPosAnimEnd = *pPos;
}
void CCamera::SetPosition(float X, float Y, float Z)
{
	this->vecPosAnimEnd.x = X;
	this->vecPosAnimEnd.y = Y;
	this->vecPosAnimEnd.z = Z;
}
void CCamera::SetFocus(D3DXVECTOR3 *pPoint)
{
	this->vecFocAnimEnd = *pPoint;
}
void CCamera::SetFocus(float X, float Y, float Z)
{
	this->vecFocAnimEnd.x = X;
	this->vecFocAnimEnd.y = Y;
	this->vecFocAnimEnd.z = Z;
}
void CCamera::BeginAnimate(float Speed)
{
	this->vecPosAnimStart = this->vecPosition;
	this->vecFocAnimStart = this->vecFocus;
	this->fAnimPos = 0.0f;
	this->fAnimSpeed = Speed;
}
void CCamera::EndAnimate()
{
	this->fAnimPos = 1.0f;
	this->fAnimSpeed = 0.0f;
	this->vecPosition = this->vecPosAnimEnd;
	this->vecFocus = this->vecFocAnimEnd;
}
void CCamera::Update()
{
	if( this->fAnimSpeed != 0.0f )
	{
		this->fAnimPos += fAnimSpeed;
		if( this->fAnimPos > 1.0f )
		{
			this->fAnimPos = 1.0f;
			this->fAnimSpeed = 0.0f;
			this->vecPosition = this->vecPosAnimEnd;
			this->vecFocus = this->vecFocAnimEnd;
		}
		else
		{
			this->vecPosition.x = this->vecPosAnimStart.x +
				((this->vecPosAnimEnd.x-this->vecPosAnimStart.x)*fAnimPos);
			this->vecPosition.y = this->vecPosAnimStart.y +
				((this->vecPosAnimEnd.y-this->vecPosAnimStart.y)*fAnimPos);
			this->vecPosition.z = this->vecPosAnimStart.z +
				((this->vecPosAnimEnd.z-this->vecPosAnimStart.z)*fAnimPos);

			this->vecFocus.x = this->vecFocAnimStart.x +
				((this->vecFocAnimEnd.x-this->vecFocAnimStart.x)*fAnimPos);
			this->vecFocus.y = this->vecFocAnimStart.y +
				((this->vecFocAnimEnd.y-this->vecFocAnimStart.y)*fAnimPos);
			this->vecFocus.z = this->vecFocAnimStart.z +
				((this->vecFocAnimEnd.z-this->vecFocAnimStart.z)*fAnimPos);
		}
	}
	else
	{
		this->vecPosition = this->vecPosAnimEnd;
		this->vecFocus = this->vecFocAnimEnd;
	}
}
void CCamera::BuildViewMatrix(D3DXMATRIX *pOut)
{
	D3DXVECTOR3 pUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( pOut,
		&this->vecPosition,
		&this->vecFocus,
		&pUp );
}

