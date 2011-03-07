// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>


#include <iostream>
#include <iomanip>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <direct.h> 
#include "dirent.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned __int64 u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed __int64 s64;

typedef float f32;
void mpakDump_v3(FILE* f, const std::string& name);
void mpakDump_v1(FILE* f, const std::string& name);
void dumpFile(const std::string& name, FILE* src, u32 offset, u32 length);
void mpakPack_v1(char* dir);
struct MPakHeader
{
	u16 unk1; //3 or 0 (version?)
	u16 unk2; //5 or 2 (version as well?)
	u32 unk3; //0 or headerSize/sectionInfoSize
	//u32 nameEntryCount;
};

struct EntryHeader
{
	u32 entryCount;
};

struct SectionEntry_v3
{
	char tag[4];
	u32 dataLength;
};

struct NameEntry
{
	char tag[4];
	u32 id;
	u32 nameLength;
	//nameLength bytes name
};

struct NameEntry_v3
{
	// bytes for name
	char tag[4];
	u64 id;
};

//after all name entries:

//struct EntryHeader;

struct Entry
{
	u32 isCompressed; //0 or 1 (isCompressed flag?) //this is the first field, see metroid4.pak
	char tag[4];
	u32 id;
	u32 length;
	u32 offset; 
};
struct Entry_v3
{
	u32 isCompressed; //0 or 1 (isCompressed flag?) //this is the first field, see metroid4.pak
	char tag[4];
	u64 id;
	u32 length;
	u32 offset; 
};
#ifdef _MSC_VER
#undef min
namespace std
{
	template<class T>
	T min(T a, T b)
	{
		return a < b ? a : b;
	}

}
#endif

using namespace std;

#ifndef GCCOMMON_H
#define GCCOMMON_H GCCOMMON_H



inline void toWORD(u16& w)
{
	u8 w1 = w & 0xff;
	u8 w2 = w >> 8;
	w = (w1 << 8) | w2;
}

inline void toSHORT(s16& w)
{
	toWORD(*(u16*)&w);
}

inline u16 aWORD(u16 w)
{
	toWORD(w); return w;
}

inline void toDWORD(u32& d)
{
	u8 w1 = d & 0xff;
	u8 w2 = (d >> 8) & 0xff;
	u8 w3 = (d >> 16) & 0xff;
	u8 w4 = d >> 24;
	d = (w1 << 24) | (w2 << 16) | (w3 << 8) | w4;
}

inline void toQWORD(u64& d)
{
	u32* w0 = (u32*)(&d);
	u32 w1 = w0[1];
	w0[1] = w0[0];
	w0[0] = w1;

	toDWORD(w0[0]);
	toDWORD(w0[1]);
}

inline void toFLOAT(f32& f)
{
	toDWORD(*(u32*)&f);
}

inline u32 aDWORD(u32 d)
{
	toDWORD(d); return d;
}

inline s16 aSHORT(s16 s)
{
	return aWORD(*(u16*)&s);
}

inline u64 aQWORD(u64 q)
{
	toQWORD(q); return q;
}

#endif //GCCOMMON_H
// TODO: reference additional headers your program requires here
