
#pragma once

#ifndef DIRECTSOUND_VERSION
#define DIRECTSOUND_VERSION 0x1000
#endif
#include <new>
#include <d3dx9.h>
#include <dsound.h>



enum ResourceID
{
	ResourceID_Texture,
	ResourceID_Sprite,
	ResourceID_Sound,
	ResourceID_Mesh,
};



/* Resource class.
This is a base structure from which
resources can derive, inheriting
these basic properties. */
class Resource
{
public:
	friend class ResourceManager;
	/* Methods */
	Resource();
	virtual ~Resource();

	int			AddRef();
	int			Release();
	LPSTR		GetName();

protected:
	/* Properties */
	DWORD RefCount;
	LPSTR Name;
};

/* Resource manager */
class ResourceManager
{
public:
	/* Constructors and destructors */
	ResourceManager();
	~ResourceManager();

	/* Methods */
	int AddResource(
		Resource * pIn,
		LPSTR name);
		/* Unconditionally adds
		resource to reference list */

	int ReleaseResource(
		Resource * pIn);
		/* Removes resource from reference list,
		assuming the is referenced. */

	Resource * GetResourceByName(
		LPSTR name);
		/* Returns pointer to a resource whose
		name matches one of those referenced in
		the reference list. */


	int PoolExpand(
		DWORD BySize);
		/* Expands size of reference list pool
		by variable amount. */

	int PoolResize(
		DWORD NewSize);
		/* Resizes size of reference list pool
		to an absolute size. This function will
		not attempt to shrink the reference list. */
	
private:
	/* Data */
	HANDLE hAccessMutex;
	Resource ** Pool;
	DWORD PoolSize;
};



class Resource_Sound : public Resource
{
public:
	Resource_Sound();
	~Resource_Sound();

	IDirectSoundBuffer * pBuffer;
};
class Resource_Texture : public Resource
{
public:
	Resource_Texture();
	~Resource_Texture();

	IDirect3DTexture9 * pTexture;
};
class Resource_Sprite : public Resource
{
public:
	Resource_Sprite();
	~Resource_Sprite();

	void GetRect(LPRECT);
	void Update();

	IDirect3DTexture9 * pTexture;
	IDirect3DSurface9 * pSurface;
	float fImageSpeed;
	float fImageIndex;
	DWORD dwSpriteWidth;
};
class Resource_Mesh : public Resource
{
public:
	Resource_Mesh();
	~Resource_Mesh();
	int Draw();

	D3DXMESHCONTAINER * pMesh;
	Resource_Texture ** ppTextures;
};
class Resource_Light : public Resource
{
public:
	Resource_Light();

	D3DLIGHT9 Light;
};

