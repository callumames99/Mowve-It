
#pragma once

#include "GameResource.h"
#include "CStruct.h"



/* Basic structures */
struct PIXEL {
	union {
		struct {
			BYTE r;
			BYTE g;
			BYTE b;
			BYTE a;
		};
		DWORD rgba;
	};
};
struct VERTEX
{
#define D3DFVF_VERTEX (D3DFVF_TEX1|D3DFVF_NORMAL|D3DFVF_XYZ)
	float Position[3];
	float Normal[3];
	float TexCoord[2];
};
struct WaveFileHeader
{
	char id[4]; // Should be 'RIFF'
	DWORD dwFileSize;
	char header[4]; // Should be 'WAVE'
	char assert[4]; // Should be "fmt "
	DWORD dwLenFmtData; // Should be 16
	WORD wFmtType;
	WORD dwNumChannels;
	DWORD dwSampleRate;
	DWORD dwAvgBytesPerSec;
	WORD wBlockAlign;
	WORD dwBitsPerSample;
};
struct WaveDataChunk
{
	char ChunkName[4];
	DWORD dwSizeOfChunk;
};



/* State structures */
struct MOUSE_STATE
{
	DWORD Buttons;
	POINT Position;
	POINT PrevPos;

	void Clear();
};
struct KEYBOARD_STATE
{
	BYTE Keys[256];
};



/* Queue class.
This structure handles requests that are queued. */
class Queue
{
private:
	void * queue;

public:
	Queue();
	~Queue();

	BOOL AddRequest( int (__stdcall *func)() );
	void Execute();
};



/* This is an enumeration of object IDs. */
enum GOBJID
{
	GOBJID_Null,
	// Time tracker
	GOBJID_TimeTracker,
	// Game contexts
	GOBJID_CONTEXT_MainMenu,
	GOBJID_CONTEXT_PauseMenu,
	GOBJID_CONTEXT_HelpScreen,
	GOBJID_CONTEXT_OptionsMenu,
	GOBJID_CONTEXT_MainGame,
	// User interface objects
	GOBJID_GAME_UI_StartGame,
	GOBJID_GAME_UI_HowToPlay,
	GOBJID_GAME_UI_Options,
	GOBJID_GAME_UI_Exit,
	// Mowers
	GOBJID_GAME_MowerMonster,
	GOBJID_GAME_MowerMover,
	GOBJID_GAME_MowerMini,
	// Others
	GOBJID_GAME_GrassTile,
	GOBJID_GAME_Gnome,
	GOBJID_GAME_StoneOrnament,
	GOBJID_GAME_Tree,
	GOBJID_GAME_TreeBeehive,
	GOBJID_GAME_Cat,
	GOBJID_GAME_Dog,
	GOBJID_GAME_RabbitHelper,
	GOBJID_GAME_MoleHill,
};



/* The following are declarations of structures. Eventually
they will represent the structure of blocks of memory allocated
in the RAM (Random Access Memory). The first structure is an
abstract structure, which serves as a base structure from which
other structures derive from, known as a 'parent-child'
relationship.
All derivative structures will contain four basic methods:
- GetObjId() (returns to the calling thread the ID of the object
as defined in the enumeration above)
- Initialise() (Initialises all members of the structure to zero)
- Create() (this method initialises the structure for use later)
- Destroy() (this method prepares the structure for deletion and
then deletes the structure from memory) */
struct GOBJ_PARENT
{
	/* Base structure */
	virtual int GetObjId()		= 0;
	virtual int Initialise()	= 0;
	virtual int Create();
	virtual int Destroy();
	virtual int MsgProc(HWND,UINT,WPARAM,LPARAM);	
};



/* Time tracker object. Used for high resolution time tracking. */
struct GOBJ_TimeTracker : GOBJ_PARENT
{
	int GetObjId();
	int Initialise();
	int Update();

	UINT64 Frequency;
	UINT64 PreviousCount;
	UINT64 Elapsed;
	double DeltaTime;
	DWORD DesirableFPS;
};



/* GOBJ_GAME is an abstract structure which
game objects derive from. */
struct GOBJ_GAME : GOBJ_PARENT
{
	virtual int Update();
	virtual int Render();
	virtual int Keyboard();
	virtual int Mouse();
};



/* GOBJ_CONTEXT is an abstract structure which
game contexts derive from. Game contexts handle
what happens in a game. */
struct GOBJ_CONTEXT : GOBJ_PARENT
{
	virtual int Initialise();
	virtual int Destroy();
	virtual int Update();
	virtual int Render();
	virtual int MsgProc(HWND,UINT,WPARAM,LPARAM);
	virtual int Keyboard();
	virtual int Mouse();
	int RegisterObject(GOBJ_GAME*);
	int UnregisterObject(GOBJ_GAME*);
	int DestroyObject(GOBJ_GAME*);

	GOBJ_GAME** ObjectList;
	DWORD		ListSize;
};
/* GOBJ_CONTEXT_MainMenu is a structure which handles
what goes on when the Main Menu screen is active. */
struct GOBJ_CONTEXT_MainMenu : GOBJ_CONTEXT
{
	int GetObjId();
	int Create();

	int MsgProc(HWND,UINT,WPARAM,LPARAM);
	int Render();
};
/* GOBJ_CONTEXT_HelpScreen is a structure which handles
what goes on when the help screen is active. */
struct GOBJ_CONTEXT_HelpScreen : GOBJ_CONTEXT
{
	int GetObjId();
	int Initialise();

	int MsgProc(HWND,UINT,WPARAM,LPARAM);
	int Render();

	WORD page;
};
/* GOBJ_CONTEXT_OptionsMenu is a structure which handles
what goes on when the options screen is active. */
struct GOBJ_CONTEXT_OptionsMenu : GOBJ_CONTEXT
{
	int GetObjId();
	int Create();

	int MsgProc(HWND,UINT,WPARAM,LPARAM);
	int Render();
};
/* GOBJ_CONTEXT_PauseMenu is a structure which handles
what goes on when the game becomes paused and a menu
appears. */
struct GOBJ_CONTEXT_PauseMenu : GOBJ_CONTEXT
{
	int GetObjId();
	int Destroy();

	int MsgProc(HWND,UINT,WPARAM,LPARAM);
	int Render();

	GOBJ_CONTEXT * pPrevious;
};
/* GOBJ_CONTEXT_MainGame is a structure which handles
what goes on when the 'New Game' button is selected. */
struct GOBJ_CONTEXT_MainGame : GOBJ_CONTEXT
{
	int GetObjId();
	int Create();

	int MsgProc(HWND,UINT,WPARAM,LPARAM);
	int Update();
	int Render();

	void Congratulations();
	void TimeoutGameover();
	void NoLivesGameover();

	int (__stdcall *OnLevelCompletion)();
	long score;
	DWORD TileWidth; // Width extent of tiles from centre
	DWORD TileHeight; // Height extent of tiles from centre
	float fGrassCut;
	DWORD dwTimer; // Frames left before timeout
	DWORD dwLives; // Lives left
	D3DXVECTOR3 vecFarView; // Level-specific view position
};



/* GOBJ_BUTTON is a base structure from which button
objects will inherit. */
struct GOBJ_BUTTON : GOBJ_GAME
{
	virtual int Initialise();
	virtual int Create();
	virtual int Update();
	virtual int Render();

	RECT Position;
	IDirect3DTexture9 *pFace;
	bool IsAvailable;
};
/* GOBJ_BUTTON_StartGame is the structure which
will become the Start Game button in the main menu. */
struct GOBJ_BUTTON_StartGame : GOBJ_BUTTON
{
	int GetObjId();
	int MsgProc(HWND,UINT,WPARAM,LPARAM);
	int Render();
};
/* GOBJ_BUTTON_Options is the structure which
will become the Options button in the main menu. */
struct GOBJ_BUTTON_Options : GOBJ_BUTTON
{
	int GetObjId();
	int MsgProc(HWND,UINT,WPARAM,LPARAM);
	int Render();
};
/* GOBJ_BUTTON_Exit is the structure which
will become the Exit button in the main menu. */
struct GOBJ_BUTTON_Exit : GOBJ_BUTTON
{
	int GetObjId();
	int MsgProc(HWND,UINT,WPARAM,LPARAM);
	int Render();
};
/* GOBJ_BUTTON_Help is the structure which
will become the help button in the main menu. */
struct GOBJ_BUTTON_Help : GOBJ_BUTTON
{
	int GetObjId();
	int MsgProc(HWND,UINT,WPARAM,LPARAM);
	int Render();
};



/* GOBJ_SLIDER is a base structure from which slider
user interface objects will derive. */
struct GOBJ_SLIDER : GOBJ_GAME
{
	virtual int Initialise();
	virtual int MsgProc(HWND,UINT,WPARAM,LPARAM);
	virtual int Update();
	virtual int Render();

	virtual int OnDrag();
	virtual int OnRelease();

	DWORD BackColour;
	DWORD FrontColour;
	RECT Position;
	float fSetting;
	bool IsDragged;
};
/* GOBJ_SLIDER_MusicVolume is the structure which
will become the music volume adjuster. */
struct GOBJ_SLIDER_MusicVolume : GOBJ_SLIDER
{
	int GetObjId();
	int Create();

	int OnDrag();
};
/* GOBJ_SLIDER_GrassDensity is the structure which
will become the grass density adjuster. */
struct GOBJ_SLIDER_GrassDensity : GOBJ_SLIDER
{
	int GetObjId();
	int Create();

	int OnDrag();
	int OnRelease();
};



/* GOBJ_FloatingText is the structure which
will become text which makes an appearance
on screen before shortly disappearing. */
struct GOBJ_FloatingText : GOBJ_GAME
{
	int GetObjId();
	int Initialise();
	int Create();
	int Destroy();
	int Update();
	int Render();

	ID3DXFont *pFont;
	CString TextString;
	DWORD dwColour;
	DWORD dwInitFrameCount;
	DWORD dwFrameCount;
	DWORD dwAnimStage;
	float Position[3];
	float Velocity[3];
};



/* GOBJ_GAME_GrassTile is the structure which
will represent a single tile of grass. */
struct GOBJ_GAME_GrassTile : GOBJ_GAME
{
	int GetObjId();
	int Initialise();
	int Create();
	int Destroy();
	int Render();

	Resource_Mesh * pMesh;
	float Position[3];
	bool IsMowed;
};



/* GOBJ_GAME_MOWER is a base structure from
which mowers will derive. */
struct GOBJ_GAME_MOWER : GOBJ_GAME
{
	virtual int Initialise();
	virtual int Destroy();
	virtual int Update();
	virtual int Render();

	virtual float GetAxialExtents() = 0; // 'Axial Extent' = extent from center x 2
	virtual float GetAxialAcceleration() = 0;

	Resource_Mesh * pMesh;
	float Position[3];
	float Velocity[3];
	float Facing[3];
};
/* GOBJ_GAME_MowerMini is the structure which
will represent the Mini Mower. */
struct GOBJ_GAME_MowerMini : GOBJ_GAME_MOWER
{
	int GetObjId();
	int Create();
	int MsgProc(HWND,UINT,WPARAM,LPARAM);

	float GetAxialExtents();
	float GetAxialAcceleration();
};
/* GOBJ_GAME_MowerMover is the structure which
will represent the Mover mower. */
struct GOBJ_GAME_MowerMover : GOBJ_GAME_MOWER
{
	int GetObjId();
	int Create();
	int MsgProc(HWND,UINT,WPARAM,LPARAM);

	float GetAxialExtents();
	float GetAxialAcceleration();
};
/* GOBJ_GAME_MowerMonster is the structure which
will represent the Monster mower. */
struct GOBJ_GAME_MowerMonster : GOBJ_GAME_MOWER
{
	int GetObjId();
	int Create();
	int MsgProc(HWND,UINT,WPARAM,LPARAM);

	float GetAxialExtents();
	float GetAxialAcceleration();
};



/* GOBJ_GAME_Gnome is the structure which
will represent a gnome. */
struct GOBJ_GAME_Gnome : GOBJ_GAME
{
	int GetObjId();
	int Initialise();
	int Create();
	int Destroy();
	int Update();
	int Render();

	Resource_Mesh * pMesh;
	DWORD FrameCount;
	float Position[3];
	float Velocity[3];
	bool IsSmashed;
};
/* GOBJ_GAME_StoneOrnament is the structure which
will represent a stone ornament. */
struct GOBJ_GAME_StoneOrnament : GOBJ_GAME
{
	int GetObjId();
	int Initialise();
	int Create();
	int Destroy();
	int Update();
	int Render();

	Resource_Mesh * pMesh;
	DWORD FrameCount;
	float Position[3];
	float Velocity[3];
	bool IsSmashed;
};
/* GOBJ_GAME_Tree is the structure which
will represent a tree obstacle. */
struct GOBJ_GAME_Tree : GOBJ_GAME
{
	int GetObjId();
	int Initialise();
	int Create();
	int Destroy();
	int Update();
	int Render();

	Resource_Mesh * pMesh;
	DWORD FrameCount;
	float Position[3];
};
/* GOBJ_GAME_TreeBeehive is the structure which
will represent a tree obstacle. */
struct GOBJ_GAME_TreeBeehive : GOBJ_GAME
{
	int GetObjId();
	int Initialise();
	int Create();
	int Destroy();
	int Update();
	int Render();

	Resource_Mesh * pMesh;
	DWORD FrameCount;
	float Position[3];
	bool IsDisturbed;
};
/* GOBJ_GAME_Cat is the structure which
will represent an obstacle. */
struct GOBJ_GAME_Cat : GOBJ_GAME
{
	int GetObjId();
	int Initialise();
	int Create();
	int Destroy();
	int Update();
	int Render();

	Resource_Mesh * pMesh;
	DWORD FrameCount;
	float Position[3];
	float Velocity[3];
	bool IsDisturbed;
};
/* GOBJ_GAME_Dog is the structure which
will represent an obstacle. */
struct GOBJ_GAME_Dog : GOBJ_GAME
{
	int GetObjId();
	int Initialise();
	int Create();
	int Destroy();
	int Update();
	int Render();

	Resource_Mesh * pMesh;
	DWORD FrameCount;
	float Position[3];
	float Velocity[3];
	bool IsDisturbed;
};
/* GOBJ_GAME_RabbitHelper is the structure which
will represent a rabbit. */
struct GOBJ_GAME_RabbitHelper : GOBJ_GAME
{
	int GetObjId();
	int Initialise();
	int Create();
	int Destroy();
	int Update();
	int Render();

	Resource_Mesh * pMesh;
	DWORD FrameCount;
	float Position[3];
	float Velocity[3];
	float Facing[3];
};
/* GOBJ_GAME_MoleHill is the structure which
will represent a molehill. */
struct GOBJ_GAME_MoleHill : GOBJ_GAME
{
	int GetObjId();
	int Initialise();
	int Create();
	int Destroy();
	int Update();
	int Render();

	Resource_Mesh * pMesh;
	DWORD FrameCount;
	float Position[3];
	float SquashScale;
};

