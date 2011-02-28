
//version 2.0 (20061001)
//by thakis

//decompresses compressed metroid prime files.
//metroid prime 1 uses zlib compression,
//metroid prime 2 uses segmented LZO1x (thanks to SkippyJr
// for pointing this out)

//zlib decompression is handled by http://www.zlib.net/,
//LZO code (lzo.h/c) was taken from ffmpegs libavcodec.

#include "main.h"
#include "zlib.h"
#include "lzo.h"


//vector<u8> src(segmentSize);

int mp3=0;
char* fname;
vector<u8> dst;
string dstF;

TxtrHeader txtr;


int uncompSize;
u32 fversion;
u32 header1;
u32 header2;
u32 header3;



#pragma pack(push, 1)

struct HeaderMP1_2
{
	u32 uncompSize;
	//mp1 has 0x78da (zlib stream with maximal compression)
	//mp2 has something else :-P (lzo doesn't seem to have a header)
	u16 compressionMethod;
};

struct HeaderMP3
{
	char id[4];
	u32 version; // version maybe
};

struct HeaderMP3_v1
{
	char id[4];
	u32 version; // version maybe
	u32 compSize;
	u32 uncompSize;
};

struct HeaderMP3_v2
{
	char id[4];
	u32 version; // version
	u32 unk2; // always 0x0000000c
	u32 unk3; // ^

	struct
	{
		u32 compSize;
		u32 uncompSize;
	} dataInfo[1];

	TxtrHeader texHeader;
};

struct HeaderMP3_v3
{
	char id[4];
	u32 version; // version
	u32 unk2; // always 0x0000000c
	u32 unk3; // ^

	struct
	{
		u32 compSize;
		u32 uncompSize;
	} dataInfo[2];

	TxtrHeader texHeader;

	// palette info header
	u32 unk4;
	u32 unk5; // possibly 2 u16's, seems to always be 0x01000001
};

union
{
	HeaderMP3_v1 headerMP3_v1;
	HeaderMP3_v2 headerMP3_v2;
	HeaderMP3_v3 headerMP3_v3;
} headerMP3;

#pragma pack(pop)

int decompressZLib(u8* source, int sourceSize, u8* dest, int dstSize)
{
	int ret;
	z_stream strm;

	// allocate decompression state
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = sourceSize;
	strm.next_in = source;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	//decompress input
	strm.avail_out = dstSize;
	strm.next_out = dest;
	ret = inflate(&strm, Z_NO_FLUSH);
	assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
	switch (ret)
	{
	case Z_NEED_DICT:
		ret = Z_DATA_ERROR;     /* and fall through */
	case Z_DATA_ERROR:
	case Z_MEM_ERROR:
		(void)inflateEnd(&strm);
		return ret;
	}

	assert(strm.avail_out == 0);
	assert(ret == Z_STREAM_END);

	// clean up and return
	/*(void)*/inflateEnd(&strm);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int decompressLZO(u8* source, int sourceSize, u8* dest, u32& dstSize)
{
	int size = dstSize;
	int result = lzo1x_decode(dest, &size, source, &sourceSize);
	dstSize = size;
	return result;
}

void WriteFile(FILE * inFile, int size = 0)
{
	if(size==-1){
		 
		 fread(&dst[0], 1, uncompSize, inFile);
    }
    else if (size == 0)
	{
		u32 bytesLeft = uncompSize;
		cout << bytesLeft << endl;
		while (bytesLeft > 0)
		{
			cout << bytesLeft << endl;
			u16 segmentSize;
			fread(&segmentSize, sizeof(segmentSize), 1, inFile);
			toWORD(segmentSize);
			vector<u8> src(segmentSize);

			fread(&src[0], 1, segmentSize, inFile);

			int result = decompressLZO(&src[0], segmentSize,
				&dst[uncompSize - bytesLeft], bytesLeft);

			if ((result & LZO_ERROR) != 0)
			{
				cerr << "Failed to decompress \'" << fname << "\'" << endl;
				system("pause");
				return;
			}
		}
	}
	else
	{
		vector<u8> src(size - 4);
		fread(&src[0], 1, size - 4, inFile);
		fclose(inFile);

		if (decompressZLib(&src[0], size - 4, &dst[0], uncompSize) != Z_OK)
		{
			cerr << "Failed to decompress \'" << fname << "\'" << endl;
			return;
		}
	}

	//write buffer to output file
	string inName = fname, outName;
	string::size_type pos = inName.rfind('\\');
	if (pos != string::npos)
		outName = inName.substr(0, pos + 1) + "0" + inName.substr(pos + 1);
	else
		outName = "0" + inName;
    dstF=outName.c_str();
	FILE* outFile = fopen(outName.c_str(), "wb");
	if (outFile == NULL)
		return;
	fseek(outFile,0,SEEK_SET);
	if (fversion == 2 || fversion == 3)
	{
		fwrite(&txtr, sizeof(txtr), 1, outFile);
		if (fversion == 3)
		{
			fwrite(&headerMP3.headerMP3_v3.unk4, sizeof(headerMP3.headerMP3_v3.unk4), 1, outFile);
			fwrite(&headerMP3.headerMP3_v3.unk5, sizeof(headerMP3.headerMP3_v3.unk5), 1, outFile);
		}
	}
	fwrite(&dst[0], uncompSize, 1, outFile);
	fclose(outFile);
}

void decompressMP1_2()
{
	FILE* inFile = fopen(fname, "rb");

	//get file size
	fseek(inFile, 0, SEEK_END);
	int size = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);
fVersion=MP1;
	//read file
	HeaderMP1_2 h;
	fread(&h, sizeof(h), 1, inFile);
	toDWORD(h.uncompSize);
	toWORD(h.compressionMethod);
	fseek(inFile, -2, SEEK_CUR);
	uncompSize = h.uncompSize;
	//decompress to buffer
	dst.resize(uncompSize);
	if (h.compressionMethod == 0x78da)
		WriteFile(inFile, size);
	else
		WriteFile(inFile);
}

void decompressMP3(int version)
{
	fVersion=MP3;
	FILE* inFile = fopen(fname, "rb");

	//get file size
	fseek(inFile, 0, SEEK_END);
	int size = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);
    int comptype=0;
	if (version ==1)
	{
		HeaderMP3_v1 &h = headerMP3.headerMP3_v1;
		fread(&h, sizeof(h), 1, inFile);
		toDWORD(h.version);
		toDWORD(h.compSize);
		toDWORD(h.uncompSize);
		uncompSize = h.uncompSize;
	}
	else if(version==2)
	{
		//return (void)(cerr << "unsupported format" << endl);

		HeaderMP3_v2 &h = headerMP3.headerMP3_v2;
		fread(&h, sizeof(h), 1, inFile);
		toDWORD(h.version);
		toDWORD(h.unk2);
		toDWORD(h.unk3);

		toDWORD(h.dataInfo[0].compSize);
		toDWORD(h.dataInfo[0].uncompSize);
        h.dataInfo[0].compSize&=0xFFFFFF;
        h.dataInfo[0].uncompSize&=0xFFFFFF;
		uncompSize = h.dataInfo[0].uncompSize;

		//fread(&txtr,1,sizeof(txtr),inFile);
		memcpy(&txtr, &h.texHeader, sizeof(txtr));

		//int lame=ftell(inFile);
		if(h.dataInfo[0].compSize==h.dataInfo[0].uncompSize){
          comptype=-1;
		}
	}
	else if(version==3)
	{
		HeaderMP3_v3 &h = headerMP3.headerMP3_v3;
		fread(&h, sizeof(h), 1, inFile);
		toDWORD(h.version);
		toDWORD(h.unk2);
		toDWORD(h.unk3);

		toDWORD(h.dataInfo[0].compSize);
		toDWORD(h.dataInfo[0].uncompSize);

		toDWORD(h.dataInfo[1].compSize);
		toDWORD(h.dataInfo[1].uncompSize);

		uncompSize = h.dataInfo[0].uncompSize + h.dataInfo[1].uncompSize;

		//fread(&txtr,1,sizeof(txtr),inFile);
		memcpy(&txtr, &h.texHeader, sizeof(txtr));

		int lame=ftell(inFile);
	}
	dst.resize(uncompSize);
	WriteFile(inFile, comptype);
}
int decompFile(const char* srcName,char* dstName)//int main(int argc, char* argv[])
{
	//if (argc < 2)
	//     return EXIT_FAILURE;
	/*int file=0;
	char* Files[3]={"C:\\Documents and Settings\\Administrator\\Desktop\\mdecomp3\\mdecomp3\\1_e5a8dfed24b2f64b.TXTR",
	"C:\\Documents and Settings\\Administrator\\Desktop\\mdecomp3\\mdecomp3\\1_1f1348cc4ab1f639.TXTR",
	"C:\\Documents and Settings\\Administrator\\Desktop\\mdecomp3\\mdecomp3\\1_2d0ac63f1ab6f16.TXTR"};
	*/char* File=(char*)srcName;//"C:\\Documents and Settings\\Administrator\\Desktop\\mdecomp3\\debug\\1_b61b7f67bbb9fff.TXTR";
	//char* File="..\\debug\\1_b61b7f67bbb9fff.TXTR";  
	FILE* inFile = fopen(File,"r+b");//fopen(argv[1], "rb");
	if (inFile == NULL)
		return EXIT_FAILURE;
	fname =File;//argv[1];//Files[file];//"C:\\Documents and Settings\\Administrator\\Desktop\\mdecomp3\\mdecomp3\\1_e5a8dfed24b2f64b.TXTR";// argv[1];
	u32 tmp;

	// attempt to read MP3 file tag 'CMPD'
	fread(&tmp, sizeof(tmp), 1, inFile);
	toDWORD(tmp);

	// attempt to read MP3 file version
	fread(&fversion, sizeof(fversion), 1, inFile);
	toDWORD(fversion);

	fclose(inFile);

	switch (tmp)
	{
	case 0x434d5044:
		decompressMP3(fversion);
		break;
	default:
		mp3=0;
		decompressMP1_2();
		break;
	}
    //dstName=&dstF;
	sprintf(dstName,"%s",dstF.c_str());
	return EXIT_SUCCESS;
}
/*
 * LZO 1x decompression
 * Copyright (c) 2006 Reimar Doeffinger
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

//! avoid e.g. MPlayers fast_memcpy, it slows things down here
#undef memcpy
#include <string.h>
#include "lzo.h"

//! define if we may write up to 12 bytes beyond the output buffer
//#define OUTBUF_PADDED 1
//! define if we may read up to 4 bytes beyond the input buffer
//#define INBUF_PADDED 1
typedef struct LZOContext {
    uint8_t *in, *in_end;
    uint8_t *out_start, *out, *out_end;
    int error;
} LZOContext;

/**
 * \brief read one byte from input buffer, avoiding overrun
 * \return byte read
 */
static int get_byte(LZOContext *c) {
    if (c->in < c->in_end)
        return *c->in++;
    c->error |= LZO_INPUT_DEPLETED;
    return 1;
}

/**
 * \brief decode a length value in the coding used by lzo
 * \param x previous byte value
 * \param mask bits used from x
 * \return decoded length value
 */
static int get_len(LZOContext *c, int x, int mask) {
    int cnt = x & mask;
    if (!cnt) {
        while (!(x = get_byte(c))) cnt += 255;
        cnt += mask + x;
    }
    return cnt;
}

/**
 * \brief copy bytes from input to output buffer with checking
 * \param cnt number of bytes to copy, must be > 0
 */
static void copy(LZOContext *c, int cnt) {
    register uint8_t *src = c->in;
    register uint8_t *dst = c->out;
    if (src + cnt > c->in_end) {
        cnt = c->in_end - src;
        c->error |= LZO_INPUT_DEPLETED;
    }
    if (dst + cnt > c->out_end) {
        cnt = c->out_end - dst;
        c->error |= LZO_OUTPUT_FULL;
    }
#if defined(INBUF_PADDED) && defined(OUTBUF_PADDED)
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    src += 4;
    dst += 4;
    cnt -= 4;
    if (cnt > 0)
#endif
        memcpy(dst, src, cnt);
    c->in = src + cnt;
    c->out = dst + cnt;
}

/**
 * \brief copy previously decoded bytes to current position
 * \param back how many bytes back we start
 * \param cnt number of bytes to copy, must be > 0
 *
 * cnt > back is valid, this will copy the bytes we just copied,
 * thus creating a repeating pattern with a period length of back.
 */
static void copy_backptr(LZOContext *c, int back, int cnt) {
    register uint8_t *src = &c->out[-back];
    register uint8_t *dst = c->out;
    if (src < c->out_start) {
        c->error |= LZO_INVALID_BACKPTR;
        return;
    }
    if (dst + cnt > c->out_end) {
        cnt = c->out_end - dst;
        c->error |= LZO_OUTPUT_FULL;
    }
    if (back == 1) {
        memset(dst, *src, cnt);
        dst += cnt;
    } else {
#ifdef OUTBUF_PADDED
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        src += 4;
        dst += 4;
        cnt -= 4;
        if (cnt > 0) {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[3] = src[3];
            dst[4] = src[4];
            dst[5] = src[5];
            dst[6] = src[6];
            dst[7] = src[7];
            src += 8;
            dst += 8;
            cnt -= 8;
        }
#endif
        if (cnt > 0) {
            int blocklen = back;
            while (cnt > blocklen) {
                memcpy(dst, src, blocklen);
                dst += blocklen;
                cnt -= blocklen;
                blocklen <<= 1;
            }
            memcpy(dst, src, cnt);
        }
        dst += cnt;
    }
    c->out = dst;
}

/**
 * \brief decode LZO 1x compressed data
 * \param out output buffer
 * \param outlen size of output buffer, number of bytes left are returned here
 * \param in input buffer
 * \param inlen size of input buffer, number of bytes left are returned here
 * \return 0 on success, otherwise error flags, see lzo.h
 *
 * make sure all buffers are appropriately padded, in must provide
 * LZO_INPUT_PADDING, out must provide LZO_OUTPUT_PADDING additional bytes
 */
int lzo1x_decode(uint8_t *out, int *outlen, uint8_t *in, int *inlen) {
    enum {COPY, BACKPTR} state = COPY;
    int x;
    LZOContext c;
    c.in = in;
    c.in_end = in + *inlen;
    c.out = c.out_start = out;
    c.out_end = out + * outlen;
    c.error = 0;
    x = get_byte(&c);
    if (x > 17) {
        copy(&c, x - 17);
        x = get_byte(&c);
        if (x < 16) c.error |= LZO_ERROR;
    }
    while (!c.error) {
        int cnt, back;
        if (x >> 4) {
            if (x >> 6) {
                cnt = (x >> 5) - 1;
                back = (get_byte(&c) << 3) + ((x >> 2) & 7) + 1;
            } else if (x >> 5) {
                cnt = get_len(&c, x, 31);
                x = get_byte(&c);
                back = (get_byte(&c) << 6) + (x >> 2) + 1;
            } else {
                cnt = get_len(&c, x, 7);
                back = (1 << 14) + ((x & 8) << 11);
                x = get_byte(&c);
                back += (get_byte(&c) << 6) + (x >> 2);
                if (back == (1 << 14)) {
                    if (cnt != 1)
                        c.error |= LZO_ERROR;
                    break;
                }
            }
        } else
        switch (state) {
            case COPY:
                cnt = get_len(&c, x, 15);
                copy(&c, cnt + 3);
                x = get_byte(&c);
                if (x >> 4)
                    continue;
                cnt = 1;
                back = (1 << 11) + (get_byte(&c) << 2) + (x >> 2) + 1;
                break;
            case BACKPTR:
                cnt = 0;
                back = (get_byte(&c) << 2) + (x >> 2) + 1;
                break;
        }
        copy_backptr(&c, back, cnt + 2);
        cnt = x & 3;
        state = cnt ? BACKPTR : COPY;
        if (cnt)
            copy(&c, cnt);
        x = get_byte(&c);
    }
    *inlen = c.in_end - c.in;
    *outlen = c.out_end - c.out;
    return c.error;
}
