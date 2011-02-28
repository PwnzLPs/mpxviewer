// mpakdump2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//mpakdump
//by thakis
//v1.0
//v2.0 - 2009/10/06 - modified to add support for Metroid Prime 3 - by revel8n
#include <iostream>
#include <iomanip>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <direct.h> //mkdir()

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

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned __int64 u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed __int64 s64;

typedef float f32;

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


#pragma pack(push, 1)

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
	u32 unk; //0 or 1 (isCompressed flag?) //this is the first field, see metroid4.pak
	char tag[4];
	u32 id;
	u32 length;
	u32 offset; 
};
struct Entry_v3
{
	u32 unk; //0 or 1 (isCompressed flag?) //this is the first field, see metroid4.pak
	char tag[4];
	u64 id;
	u32 length;
	u32 offset; 
};

#pragma pack(pop)

void dumpFile(const std::string& name, FILE* src, u32 offset, u32 length)
{
	int t = ftell(src);
	fseek(src, offset, SEEK_SET);
	
	FILE* out = fopen(name.c_str(), "wb");
	if(out == NULL)
		return;
	
	u32 read = 0;
	u8 buff[1024];
	while(read < length)
	{
		int r = fread(buff, 1, 
			
			
			//std::min(1024u, length - read)
			1024u < length - read ? 1024u : length - read
			
			
			, src);
		fwrite(buff, 1, r, out);
		read += r;
	}
	
	fclose(out);
	fseek(src, t, SEEK_SET);
}
	
void mpakDump_v1(FILE* f, const std::string& name)
{
	int i;
	MPakHeader h;
	
	fseek(f, 0, SEEK_SET);
	
	fread(&h, sizeof(h), 1, f);
	toWORD(h.unk1);
	toWORD(h.unk2);
	toDWORD(h.unk3);
	
	assert(h.unk1 == 3);
	assert(h.unk2 == 5);
	assert(h.unk3 == 0);
	
	//read name entry header
	EntryHeader neh;
	fread(&neh, sizeof(neh), 1, f);
	toDWORD(neh.entryCount);
	
	cout << hex;
	
	//output name entries
	cout << string(70, '-') << endl
		<< neh.entryCount << " name entries: " << endl;
	for(i = 0; i < neh.entryCount; ++i)
	{
		NameEntry ne;
		fread(&ne, sizeof(ne), 1, f);
		toDWORD(ne.id);
		toDWORD(ne.nameLength);
		
		char name[1000] = {0}; fread(name, 1, ne.nameLength, f);
		
		cout << string(ne.tag, 4) << " " << ne.id << " " 
			<< string(name, ne.nameLength) << endl;
	}
	
	//read entry header
	EntryHeader eh;
	fread(&eh, sizeof(eh), 1, f);
	toDWORD(eh.entryCount);
	
	//output entries
	cout << string(70, '-') << endl
		<< eh.entryCount << " entries: " << endl;
	int count = 0;
	for(i = 0; i < eh.entryCount; ++i)
	{
		Entry e;
		fread(&e, sizeof(e), 1, f);
		toDWORD(e.unk);
		
		toDWORD(e.id);
		toDWORD(e.length);
		toDWORD(e.offset);
		
		cout << e.unk << " " << string(e.tag, 4) << " id: " << e.id
			<< " length: " << e.length
			<< " offset: " << e.offset << " " << endl;
		
		assert(e.unk == 0 || e.unk == 1);
		count += e.unk;
		
		//dump file
		std::ostringstream nameStream;
		nameStream << name << "\\" << e.unk << "_" << hex << e.id << "."
			<< string(e.tag, 4);
		dumpFile(nameStream.str(), f, e.offset, e.length);
	}
	
	cout << dec;
	cout << count << " ones, " << eh.entryCount - count << " zeroes." << endl;
	cout << eh.entryCount << " entries, " << neh.entryCount << " name entries"
		<< endl;
}

void mpakDump_v3(FILE* f, const std::string& name)
{
	int i;
	MPakHeader h;

	fseek(f, 0x00, SEEK_SET);

	fread(&h, sizeof(h), 1, f);
	toWORD(h.unk1);
	toWORD(h.unk2);
	toDWORD(h.unk3);
	
	assert(h.unk1 == 0);
	assert(h.unk2 == 2);

	fseek(f, 0x40, SEEK_SET);
	
	//read section entry header
	EntryHeader seh;
	fread(&seh, sizeof(seh), 1, f);
	toDWORD(seh.entryCount);
	
	cout << hex;
	
	vector<SectionEntry_v3> se;
	se.resize(seh.entryCount);

	//output section entries
	cout << string(70, '-') << endl
		<< seh.entryCount << " section entries: " << endl;
	for(i = 0; i < seh.entryCount; ++i)
	{
		fread(&se[i], sizeof(SectionEntry_v3), 1, f);
		toDWORD(se[i].dataLength);
		
		cout << string(se[i].tag, 4) << " " << se[i].dataLength << endl;
	}
	
	fseek(f, 0x80, SEEK_SET);
	
	//read name entry header
	EntryHeader neh;
	fread(&neh, sizeof(neh), 1, f);
	toDWORD(neh.entryCount);
	
	cout << hex;
	
	//output name entries
	cout << string(70, '-') << endl
		<< neh.entryCount << " name entries: " << endl;
	for(i = 0; i < neh.entryCount; ++i)
	{
		char name[1000] = {0}; fscanf(f, "%[^\0]", name);
		fseek(f, 0x01, SEEK_CUR);

		NameEntry_v3 ne;
		fread(&ne, sizeof(ne), 1, f);
		toQWORD(ne.id);
		
		cout << string(ne.tag, 4) << " " << ((u32*)&ne.id)[1] << ((u32*)&ne.id)[0] << " " 
			<< string(name) << endl;
	}
	
	fseek(f, 0x80 + se[0].dataLength, SEEK_SET);

	const u32 dataStart = 0x80 + se[0].dataLength + se[1].dataLength;
	
	//read entry header
	EntryHeader eh;
	fread(&eh, sizeof(eh), 1, f);
	toDWORD(eh.entryCount);
	
	//output entries
	cout << string(70, '-') << endl
		<< eh.entryCount << " entries: " << endl;
	int count = 0;
	for(i = 0; i < eh.entryCount; ++i)
	{
		Entry_v3 e;
		fread(&e, sizeof(e), 1, f);
		toDWORD(e.unk);
		
		toQWORD(e.id);
		toDWORD(e.length);
		toDWORD(e.offset);
		
		cout << e.unk << " " << string(e.tag, 4) << " id: " << ((u32*)&e.id)[1] << ((u32*)&e.id)[0]
			<< " length: " << e.length
			<< " start: " << dataStart
			<< " offset: " << e.offset << " " << endl;
		
		assert(e.unk == 0 || e.unk == 1);
		count += e.unk;
		
		//dump file
		std::ostringstream nameStream;
		nameStream << name << "\\" << e.unk << "_" << hex << ((u32*)&e.id)[1] << ((u32*)&e.id)[0] << "."
			<< string(e.tag, 4);
		dumpFile(nameStream.str(), f, dataStart + e.offset, e.length);
	}
	
	cout << dec;
	cout << count << " ones, " << eh.entryCount - count << " zeroes." << endl;
	cout << eh.entryCount << " entries, " << neh.entryCount << " name entries"
		<< endl;
}

void mpakDump(FILE* f, const std::string& name)
{
	MPakHeader h;
	
	fseek(f, 0, SEEK_SET);
	
	fread(&h, sizeof(h), 1, f);
	toWORD(h.unk1);
	toWORD(h.unk2);
	toDWORD(h.unk3);
	
	if (h.unk1 == 3 && h.unk2 == 5)
	{
		mpakDump_v1(f, name);
	}
	else if (h.unk1 == 0 && h.unk2 == 2)
	{
		mpakDump_v3(f, name);
	}
	else
	{
		cout << "Unknown pak file version!" << endl;
	}
}

int main(int argc, char* argv[])
{
	FILE* f;
	
	if(argc < 2 || (f = fopen(argv[1], "rb")) == NULL)
		return EXIT_FAILURE;
	
	std::string dirName = argv[1] + string("_dir");
	mkdir(dirName.c_str());
	mpakDump(f, dirName);
	
	fclose(f);
	system("pause");
	
	return 0;
}
