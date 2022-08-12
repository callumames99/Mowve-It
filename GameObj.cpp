


#include "GameObj.h"
#include <new>



void MOUSE_STATE::Clear()
{
	this->Buttons = 0;
}



struct QueueBlock
{
public:
	QueueBlock * pNext;
	DWORD Request[15];

	QueueBlock()
	{
		pNext = nullptr;
		memset( Request, 0, 60 );
	}
	~QueueBlock()
	{
		if( pNext ) delete pNext;
	}
};

Queue::Queue()
{
	queue = (void *)new(std::nothrow) QueueBlock();
}
Queue::~Queue()
{
	if( queue ) delete queue;
}
BOOL Queue::AddRequest( int (__stdcall *func)() )
{
	QueueBlock *& q = *(QueueBlock **)&queue;
	if( q )
	{
		for( WORD i = 0; i < 15; ++i )
		{
			if( !q->Request[i] )
			{
				q->Request[i] = (DWORD)func;
				return TRUE;
			}
		}
		if( !q->pNext )
		{
			q->pNext = new(std::nothrow) QueueBlock();
			if( !q->pNext ) return FALSE;
			q->Request[0] = (DWORD)func;
		}
	}

	return TRUE;
}
void Queue::Execute()
{
	QueueBlock * pCurrent = (QueueBlock*)this->queue;
	while( pCurrent )
	{
		for( WORD i = 0; i < 15; ++i ) {
			if( pCurrent->Request[i] ) {
				( ( int (__stdcall*)() )pCurrent->Request[i] ) ();
				pCurrent->Request[i] = 0;
			}
		} pCurrent = pCurrent->pNext;
	}
}



int GOBJ_PARENT::Create()
{
	return S_OK;
}
int GOBJ_PARENT::Destroy()
{
	delete this;

	return S_OK;
}
int GOBJ_PARENT::MsgProc(HWND,UINT,WPARAM,LPARAM)
{
	return S_OK;
}



int GOBJ_TimeTracker::GetObjId()
{
	return GOBJID_TimeTracker;
}
int GOBJ_TimeTracker::Initialise()
{
	this->Frequency = QueryPerformanceFrequency((LARGE_INTEGER*)&this->Frequency);
	this->PreviousCount = QueryPerformanceCounter((LARGE_INTEGER*)&this->PreviousCount);
	this->Elapsed = (UINT64)0;
	this->DeltaTime = (double)0.0;
	this->DesirableFPS = (DWORD)60;
	return S_OK;
}
int GOBJ_TimeTracker::Update()
{
	UINT64 CurrentCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&CurrentCount);
	this->Elapsed = this->PreviousCount - CurrentCount;
	this->DeltaTime = (double)(
		( long double(this->Elapsed) / long double(this->Frequency) ) *
		long double(this->DesirableFPS) );
	this->PreviousCount = CurrentCount;

	return S_OK;
}



int GOBJ_CONTEXT::Initialise()
{
	this->ObjectList = 0;
	this->ListSize = 0;

	return S_OK;
}
int GOBJ_CONTEXT::Destroy()
{
	if( this->ObjectList )
	{
		for( DWORD i = 0; i < this->ListSize; i++ )
		{
			if( this->ObjectList[i] )
				this->ObjectList[i]->Destroy();
		}
		delete[] this->ObjectList;
	}

	GOBJ_PARENT::Destroy();

	return S_OK;
}
int GOBJ_CONTEXT::Update()
{
	for( DWORD i = 0; i < this->ListSize; i++ ) {
		if( this->ObjectList[i] )
			this->ObjectList[i]->Update();
	} return S_OK;
}
int GOBJ_CONTEXT::Render()
{
	for( DWORD i = 0; i < this->ListSize; i++ ) {
		if( this->ObjectList[i] )
			this->ObjectList[i]->Render();
	} return S_OK;
}
int GOBJ_CONTEXT::Keyboard()
{
	for( DWORD i = 0; i < this->ListSize; i++ ) {
		if( this->ObjectList[i] )
			this->ObjectList[i]->Keyboard();
	} return S_OK;
}
int GOBJ_CONTEXT::Mouse()
{
	for( DWORD i = 0; i < this->ListSize; i++ ) {
		if( this->ObjectList[i] )
			this->ObjectList[i]->Mouse();
	} return S_OK;
}
int GOBJ_CONTEXT::RegisterObject(GOBJ_GAME* pObj)
{
	// Find empty slot to insert object handle
	for( DWORD i = 0; i < this->ListSize; i++ )
	{
		if( !this->ObjectList[i] ) {
			this->ObjectList[i] = pObj;
			return S_OK;
		} else if( this->ObjectList[i] == pObj )
			return E_ABORT;
	}
	// Expand list
	DWORD dwNewSize = this->ListSize + 32;
	GOBJ_GAME** pNewList = new(std::nothrow) GOBJ_GAME *[dwNewSize]();
	if( !pNewList ) return E_OUTOFMEMORY;
	if( this->ObjectList )
	{
		memcpy( pNewList, this->ObjectList, this->ListSize*sizeof(GOBJ_GAME *) );
		delete[] this->ObjectList;
	}
	this->ObjectList = pNewList;
	this->ObjectList[this->ListSize] = pObj;
	this->ListSize = dwNewSize;
	return S_OK;
}
int GOBJ_CONTEXT::UnregisterObject(GOBJ_GAME* pObj)
{
	for( DWORD i = 0; i < this->ListSize; i++ )
	{
		if( this->ObjectList[i] == pObj ) {
			this->ObjectList[i] = 0;
			return S_OK;
		}
	}
	return E_INVALIDARG;
}
int GOBJ_CONTEXT::DestroyObject(GOBJ_GAME* pObj)
{
	for( DWORD i = 0; i < this->ListSize; i++ )
	{
		if( this->ObjectList[i] == pObj ) {
			this->ObjectList[i]->Destroy();
			this->ObjectList[i] = 0;
			return S_OK;
		}
	}
	return E_INVALIDARG;
}
int GOBJ_CONTEXT::MsgProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	for( DWORD i = 0; i < this->ListSize; i++ )
	{
		if( this->ObjectList[i] )
			this->ObjectList[i]->MsgProc(hWnd,uiMsg,wParam,lParam);
	}
	return S_OK;
}

int GOBJ_GAME::Update()
{
	return S_OK;
}
int GOBJ_GAME::Render()
{
	return S_OK;
}
int GOBJ_GAME::Keyboard()
{
	return S_OK;
}
int GOBJ_GAME::Mouse()
{
	return S_OK;
}

