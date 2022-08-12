


#include "GameResource.h"



ResourceManager::ResourceManager()
{
	this->Pool = nullptr;
	this->PoolSize = 0;
}
ResourceManager::~ResourceManager()
{
	if( this->Pool )
	{
		for( DWORD i = 0; i < this->PoolSize; i++ )
		{
			if( this->Pool[i] )
			{
				this->Pool[i]->Release();
				this->Pool[i] = nullptr;
			}
		}
		delete[] this->Pool;
		Pool = nullptr;
	}
	PoolSize = 0;
}
int ResourceManager::AddResource(Resource * resource, LPSTR name)
{
	if( !this->Pool )
	{
		this->Pool = new(std::nothrow) Resource *[32]();
		if( !this->Pool )
		{
			return E_OUTOFMEMORY;
		}
		this->PoolSize = 32;
		this->Pool[0] = resource;
		DWORD len = strlen( name ) + 1;
		LPSTR NewString = new char[len];
		memcpy(NewString,name,len);
		resource->Name = NewString;
		resource->AddRef();
		return S_OK;
	}

	for( DWORD i = 0; i < this->PoolSize; i++ )
	{
		if( !this->Pool[i] )
		{
			this->Pool[i] = resource;
			DWORD len = strlen( name ) + 1;
			LPSTR NewString = new char[len];
			memcpy(NewString,name,len);
			resource->Name = NewString;
			resource->AddRef();
			return S_OK;
		}
	}

	{
		DWORD OldSize = this->PoolSize;
		if( FAILED( this->PoolExpand( 32 ) ) )
		{
			return E_OUTOFMEMORY;
		}
		this->Pool[OldSize] = resource;
		DWORD len = strlen( name ) + 1;
		LPSTR NewString = new char[len];
		memcpy(NewString,name,len);
		resource->Name = NewString;
		resource->Name = NewString;
		resource->AddRef();

		return S_OK;
	}
}
Resource * ResourceManager::GetResourceByName(LPSTR name)
{
	if( this->Pool && name )
	{
		for( DWORD i = 0; i < this->PoolSize; i++ )
		{
			if( this->Pool[i] )
				if( this->Pool[i]->Name )
					if( strcmp( name, this->Pool[i]->GetName() ) == 0 )
						return this->Pool[i];
		}
	}
	return nullptr;
}
int ResourceManager::ReleaseResource(Resource * resource)
{
	if( this->Pool )
	{
		for( DWORD i = 0; i < this->PoolSize; i++ )
		{
			if( this->Pool[i] == resource )
			{
				this->Pool[i] = nullptr;
				resource->Release();
				return S_OK;
			}
		}
	}

	return S_OK;
}
int ResourceManager::PoolExpand(DWORD BySize)
{
	Resource ** pNewList;
	DWORD dwNewSize = this->PoolSize + BySize;
	pNewList = new(std::nothrow) Resource *[dwNewSize]();
	if( !pNewList ) return E_OUTOFMEMORY;

	if( this->Pool )
	{
		memcpy( pNewList, this->Pool, this->PoolSize * sizeof(Resource *) );
		delete[] this->Pool;
	}

	this->Pool = pNewList;
	this->PoolSize = dwNewSize;

	return S_OK;
}
int ResourceManager::PoolResize(DWORD NewSize)
{
	int diff = NewSize - this->PoolSize;

	if( diff > 0 )
	{
		PoolExpand( diff );
	}

	return S_OK;
}



Resource::Resource()
{
	this->RefCount = 1;
	this->Name = nullptr;
}
Resource::~Resource()
{
}
int Resource::AddRef()
{
	this->RefCount ++;
	return this->RefCount;
}
int Resource::Release()
{
	DWORD NewRefCount = -- this->RefCount;
	if( this->RefCount == 0 )
	{
		delete[] this->Name;
		delete this;
	}
	return NewRefCount;
}
LPSTR Resource::GetName()
{
	return this->Name;
}

