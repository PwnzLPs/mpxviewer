#ifndef MAIN_H
#define MAIN_H

#include <windows.h>
#include <stdio.h>
#include <string>
#include <gl\gl.h>
#include "glext.h"// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <stdio.h>
#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#define WIN32_LEAN_AND_MEAN
//#include "targetver.h"
#include <commctrl.h>
#include <ddraw.h> 

#include <cstdio>
#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include "arcball\arcball.h"
#include "zlib.h"
#include "DEVIL\il_wrap.h"
#include "resource.h"
#include "globals.h"
using namespace std;
typedef  unsigned __int64 uint64,u64;
typedef  unsigned long uint32, u32;
typedef  unsigned char uint8, u8;
typedef  unsigned short uint16, u16;

EXT HINSTANCE hGlobal;
EXT HWND   hwndMain;
EXT long HEADERSIZE;

struct sParents{
	std::string pName;
	HTREEITEM Parent;
};
class cParents{
public:
	cParents();
	~cParents();
	int AddParentNode(std::string pszName,HTREEITEM hParent);
	HTREEITEM*  CheckParentNode(std::string pName);//will return parent
	std::vector<sParents> nParents;
};

class sTree{
public:
	sTree();
	~sTree();
	int Init(HWND tWND);
	int AddParent( HTREEITEM sParent, HTREEITEM InsertAfter,char* text);
	int AddChild(char* text);
	int AddNode(char*Text);
	TV_ITEM tvi;
	HTREEITEM Selected;
	TV_INSERTSTRUCT tvinsert;   // struct to config out tree control
	HTREEITEM Parent;           // Tree item handle
	HTREEITEM Before;           // .......
	HTREEITEM Root;             // .......
	HIMAGELIST hImageList;      // Image list array hadle
	cParents pNodes;
	HBITMAP hBitMap;            // bitmap handler
	bool flagSelected;

	// for drag and drop
	HWND hTree;
	TVHITTESTINFO tvht; 
	HTREEITEM hitTarget;
	POINTS Pos;
	bool Dragging;

	// for lable editing
	HWND hEdit;
};
/*
void GXSetBlendMode (	GXBlendMode	type,
			GXBlendFactor	src_factor, 
			GXBlendFactor	dst_factor, 
			GXLogicOp	op );



typedef enum _GXBlendMode
{
#if ( GX_REV == 1 )
    GX_BM_NONE,
    GX_BM_BLEND,
    GX_BM_LOGIC,
    GX_MAX_BLENDMODE

#else // ( GX_REV >= 2 )
    GX_BM_NONE,
    GX_BM_BLEND,
    GX_BM_LOGIC,
    GX_BM_SUBTRACT,
    GX_MAX_BLENDMODE

#endif
} GXBlendMode;

/********************************/
/*typedef enum _GXBlendFactor
{
    GX_BL_ZERO,
    GX_BL_ONE,
    GX_BL_SRCCLR,
    GX_BL_INVSRCCLR,
    GX_BL_SRCALPHA,
    GX_BL_INVSRCALPHA,
    GX_BL_DSTALPHA,
    GX_BL_INVDSTALPHA,

    GX_BL_DSTCLR = GX_BL_SRCCLR,
    GX_BL_INVDSTCLR = GX_BL_INVSRCCLR

} GXBlendFactor;

*/
struct mAttrib{
u8 null;
u8 lum;
u8 sat;
u8 hue; 
u16 BlendNumber; //Possibly _GXBlendMode
u16 unknown0x06;
u32 unknown0x08;
u32 unknown0x0C;
//uint32 unknown0x10 ; // 0x00000004,  0x00000006,  0x000000FF
u16 padding;
u16 BlendType;//GXBlendFactor
};

struct txtrinfo{
	u32 count;
	vector<u32> ids;
};
struct txtrinfo2{
	u32 count;
	vector<u64> ids;
};
struct mGroup{
	u32 groupheader;
	txtrinfo info;
	u32 unknown0;
	u32 unknown1; 
	u32 unknown0a[2];
	u32 unknown1a;

	u32 unknownCount0;
	vector<u32> unknownData0;
	u32 unknown2[3];
	u32 AttribCount;
	vector<mAttrib> Attributes;//AttribCount
	vector<u32> unknown4;//AttribCount
	u32 LayerInfo;//It just made a little sense
	vector<GLfloat> LayerData;//LayerInfo
	u32 unknownsize;
	u32 unknown5;
	u32 unknown6;
	txtrinfo2 info2;

};
class  clsMaterialHeader{
public:
	clsMaterialHeader();
	~clsMaterialHeader();
	u32 TextureCount;
	vector<u64> TextureNames;
	u32  MaterialCount;
	vector<u32> MaterialSegmentOffsets;//new this as MaterialCount
	vector<vector<unsigned char>> MaterialSegments;//new this as MaterialCount then do the offset math for each size I left this so we don't have to read from the file again
	vector<mGroup> MaterialSegment;
	void ReadMaterialDataMP3(FILE* fp, unsigned int iSegment, std::vector<u32> & segmentSizes);
	void ReadMaterialDataMP1_2(FILE* fp, unsigned int iSegment, std::vector<u32> & segmentSizes);
	vector<GLuint> TexturesIDs;
	void ReadMaterialSegment(int index);
};
struct  suvMap{

	float U;
	float V;
};
inline void toWORD(u16& d)
{
	u8 w1 = (u8)(d & 0xff);
	u8 w2 = (u8)((d >> 8) & 0xff);
	d = (w1 << 8) | w2;
}

inline void toDWORD(u32& d)
{
	u8 w1 = (u8)(d & 0xff);
	u8 w2 = (u8)((d >> 8) & 0xff);
	u8 w3 = (u8)((d >> 16) & 0xff);
	u8 w4 = (u8)(d >> 24);
	d = (w1 << 24) | (w2 << 16) | (w3 << 8) | w4;
}
inline void toDWORD64(u64& d)
{
	/*u8 w1 =  (d        & 0xff);
	u8 w2 = ((d >>  8) & 0xff);
	u8 w3 = ((d >> 16) & 0xff);
	u8 w4 = ((d >> 24) & 0xff);
	u8 w5 = ((d >> 32) & 0xff);
    u8 w6 = ((d >> 40) & 0xff);
	u8 w7 = ((d >> 48) & 0xff);
	u8 w8 = ((d >> 56) & 0xff);*/
	
	d = 
		/* (u64)(w1 << 56) 
		|(u64)(w2 << 48) 
		| (u64)(w3 << 40)
		| (u64)(w4 << 32)
		| (u64)(w5 << 24)
		| (u64)(w6 << 16)
		| (u64)(w7 <<  8)
		| (u64)(w8      ) ;*/
		  (((uint64)(d) << 56) | \
                        (((uint64)(d) << 40) & 0xff000000000000ULL) | \
                        (((uint64)(d) << 24) & 0xff0000000000ULL) | \
                        (((uint64)(d) << 8)  & 0xff00000000ULL) | \
                        (((uint64)(d) >> 8)  & 0xff000000ULL) | \
                        (((uint64)(d) >> 24) & 0xff0000ULL) | \
                        (((uint64)(d) >> 40) & 0xff00ULL) | \
                        ((uint64)(d)  >> 56));

}
struct struct_Faces{
	static const int MAX_UV_INDICES = 20;
	u16 vertexIndex;
	u16 normalIndex;
	u16 uvIndex[MAX_UV_INDICES];
};
struct struct_SubMesh
{
	u8 primitiveFlags;
	u16 indexCount;
	vector<struct_Faces> indices;
};
struct struct_Mesh
{
	u32 GroupId;
	u32 format;
	vector<struct_SubMesh> subMeshes;
};
struct CMDL_Header{ 
	uint32 DeadBabe;
	uint32 Unknown[2];
	//Bounding Box
	float MinX;
	float MaxX;
	float MinY;
	float MaxY;
	float MinZ;
	float MaxZ;
	uint32 SegmentCount;
	uint32 MaterialHeaderCount;
};

struct tex{
	float X;
	float Y;
	float Z;
};
class cmdlContainment{
public:
	cmdlContainment();
	~cmdlContainment();
	void DeleteMe();
	CMDL_Header Header;
	int Read(char* fstr);
	int ReadHeader(FILE* fp);
	int ReadModel(FILE* fp, int segmentNum);
	int ReadUVs(FILE* fp, int segmentNum);
	int ReadNormals(FILE* fp, int segmentNum);
	int ReadTextures();
	vector<uint32>MaterialHeaderSizes;
	vector<uint32> SegmentSizes;
	vector<clsMaterialHeader> MaterialHeader;
	//vector<unsigned char>* Segment;
	vector<vector<unsigned char>> Segment;
	u32 TextNo;
	vector<string> TextureNames;
	int LoadMaterialList();
	void InitSegment();
	void ReadSegments(FILE* fp);
	u32 vertexcount;
	vector<tex>   vertexes;
	vector<suvMap> UVs;
	u32 uvcount;
	u32 normalcount;
	vector<tex>   mNormals;
	int TexturesLoaded;
	int candraw;
	int ReadMeshes(FILE* fp, int segmentNum);
	vector<struct_Mesh> Meshes;
	int LoadTextures();
	vector<vector<u8>> TextureData;
};

class sCombo{
public:
	sCombo();
	~sCombo();
	long GetListIndex();
	int cClear();
	int Clear();
	int Click();
	int SetListIndex(int Value);
	int Additem(char* iString);
	int sList(int index);
	int Init(HWND sComboHwnd);
	int Disable();
	int Enable();
	HWND thishwnd();
private:
	HWND hCombo;

};
class sList{
public:
	sList();
	~sList();
	long GetListIndex();
	int cClear();
	int Clear();
	int Click();
	int InsertString(char * iString);
	int InsertString(HWND hDlg, int id, char * iString);
	int SetListIndex(int Value);
	int Additem(char* iString);
	int Additem(const char* iString);
	int Additem(string iString);
	int sListStrings(int index);
	int Init(HWND sListHwnd);
	int Disable();
	int Enable();
	HWND thishwnd();
private:
	HWND hCombo;

};
class sChecks{
public:
	sChecks();
	~sChecks();
	int cClear();

	int SetCnt(HWND hwnd);

	int value();

	int value(u8 Val);
	int sChecks::State(bool flag);

	int sChecks::State();
private:
	HWND check;

};



class stxts{
	//Currently only support string to int
public:

	stxts();
	~stxts();
	int SetHwnd(HWND hwnd);
	int GetText();
	int SetText(char* iString);
private:
	HWND thisHwnd;

};
struct struct_joints{
	u32 index;
	u32 parent;
	tex pos;
	u32 ref;
	vector<u32> refs;


};
struct jointinf{
	u32 jointCount;
	u32 maxindex;
	vector<struct_joints> JointIndice;

};
EXT jointinf Joints;
EXT sList lstFiles;
EXT sList lstTextures;
EXT sList lstMaterials;

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag);
int DrawGLScene(GLvoid);
GLvoid KillGLWindow(GLvoid);
EXT cmdlContainment  CMDL;
EXT HWND hwndGL;
//EXT string* strFiles;
EXT string  strCurDir;
int decompFile(const char* srcName,char* dstName);
EXT float xRot;
EXT float yRot;
EXT float zRot;
EXT float xCamera;
EXT float yCamera;
EXT float zCamera;

#pragma pack(push, 1)

struct BtiTextureHeader //for output
{
	u8 format;  //seems to match tpl's format (see yagcd)
	u8 unknown;
	u16 width;
	u16 height;
	u16 unknown2; //probably padding

	u8 unknown3;
	u8 paletteFormat; //matches tpl palette format
	u16 paletteNumEntries;
	u32 paletteOffset;

	u32 unknown5;
	u16 unknown6;
	u16 unknown7;
	u8 mipmapCount;
	u8 unknown8;
	u16 unknown9;

	//this offset is relative to the TextureHeader (?)
	u32 dataOffset;

	//some of the unknown data could be render state?
	//(lod bias, min/mag filter, clamp s/t, ...)
};

struct TxtrHeader
{
	//0 - i4
	//1 - i8
	//2 - i4a4
	//3 - i8a8
	//4 - idx4
	//5 - idx8
	//(there's no file with format 6)
	//7 - r5g6b5
	//8 - rgb5a3
	//9 - r8g8b8a8
	//10 - dxt1
	u32 format;
	u16 w, h;
	u32 numMips; //could be two u16s as well...
};

struct PaletteHeader
{
	//0 - i8a8
	//1 - r5g6b5
	//2 - rgb5a3
	u32 format;

	//unk is 256 for idx8 images and 1 for idx4 images
	//unk2 is 1 for idx8 images and 16 for idx4 images
	u16 unk, unk2;
};

#pragma pack(pop)




inline u16 aWORD(u16 w)
{
	toWORD(w); return w;
}



inline void toFLOAT(float& f)
{
	toDWORD(*(u32*)&f);
}
struct TextureHeader
{
	u8 format;  //seems to match tpl's format (see yagcd)
	u8 unknown;
	u16 width;
	u16 height;
	u16 unknown2; //probably padding

	u8 unknown3;
	u8 paletteFormat; //matches tpl palette format
	u16 paletteNumEntries;
	u32 paletteOffset;


	u32 unknown5;
	u16 unknown6;
	u16 unknown7;
	u8 mipmapCount;
	u8 unknown8;
	u16 unknown9;

	//this offset is relative to the TextureHeader (?)
	u32 dataOffset;

	//some of the unknown data could be render state?
	//(lod bias, min/mag filter, clamp s/t, ...)
};


//The following structures are for dds file saving...nothing to do with the bti file :-)

struct ColorCaps
{
	u32 size;
	u32 flags;
	char fourCC[4];
	u32 rgbBitCount;
	u32 rBitMask;
	u32 gBitMask;
	u32 bBitMask;
	u32 aBitMask;
};

struct DdsHeader
{
	char type[4];
	u32 size;
	u32 flags;
	u32 height;
	u32 width;
	u32 linearSize;
	u32 depth;
	u32 numMips;
	u32 unused[11];
	ColorCaps colorCaps;
	u32 caps;
	u32 unused2[4];
};




int dumpTxtr(string srcFile, string dstFile);
int dumpBti(string srcFile, string dstFile);
int dumpBti(vector<unsigned char>* btiArray, string dstFile);
int dumpTxtr(string srcFile, vector<unsigned char>* Data);
int dumpBti(vector<unsigned char>* btiArray);
void LoadTXTR(string fileName, GLuint*  txtrno);
DdsHeader createDdsHeader(int w, int h, int numMips);
int dumpBti(vector<unsigned char>* btiArray);
// TODO: reference additional headers your program requires here

void init( void );
int lzo1x_decode(unsigned char *out, int *outlen, unsigned char *in, int *inlen);
EXT int fVersion;
#define MP1 0
#define MP2 1
#define MP3 2

EXT sTree ThisTree;
int IsSupported(char* Text);
EXT int FileType;
int LoadFile(char* File);
EXT GLuint g_textureID;
int DrawTXTR();
int LoadsTXTR(char* FileName);
EXT HDC	hDC;		// Private GDI Device Context
void ReadCINF(char* FileName);
int LoadCMDL();

EXT PFNGLMULTITEXCOORD1FARBPROC	glMultiTexCoord1fARB;//	= NULL;
EXT PFNGLMULTITEXCOORD2FARBPROC	glMultiTexCoord2fARB;//	= NULL;
EXT PFNGLMULTITEXCOORD3FARBPROC	glMultiTexCoord3fARB;//	= NULL;
EXT PFNGLMULTITEXCOORD4FARBPROC	glMultiTexCoord4fARB;//	= NULL;
EXT PFNGLACTIVETEXTUREARBPROC	glActiveTextureARB;//	= NULL;
EXT PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;//= NULL;
int LoadCINF();
long GetLong(void* thisarray);
long returnfilesize(FILE* fp);
//#define IDC_TREE1 ID_TREE_LIST

EXT ArcBallT*   ArcBall;//(640.0f, 480.0f);				                // NEW: ArcBall Instance
EXT Point2fT    MousePt;												// NEW: Current Mouse Point
EXT bool        isClicked;										// NEW: Clicking The Mouse?
EXT bool        isRClicked;										// NEW: Clicking The Right Mouse Button?
EXT bool        isDragging;					                    // NEW: Dragging The Mouse?
EXT Matrix4fT*   Transform;
EXT GLuint textnames[1024];
EXT int textcount;

#endif // MAIN_H
