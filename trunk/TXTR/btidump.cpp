//by thakis

//converts a bti file to a dds file
//(dds was chosen as output format because
//dds files support mipmaps, an alpha channel
//and dxt3 compression)

//changed 20050710:
// - type 0 images (i4) were not decoded correctly
//   (fix8x8Expand() was broken)
// - type 8 images (pal4) were not decoded correctly
//   (palette indices shouldn't be expanded to 8 bit
//   when they're copied to 8 bit...)


#include "..\main.h"


//#pragma pack(push, 1)




DdsHeader createDdsHeader(int w, int h, int numMips)
{
	DdsHeader ret;
	memset(&ret, 0, sizeof(ret));

	strncpy(ret.type, "DDS ", 4);
	ret.size = 124;
	ret.flags = 0x21007; //mipmapcount + pixelformat + width + height + caps
	ret.width = w;
	ret.height = h;
	ret.numMips = numMips;
	ret.colorCaps.size = 32;
	ret.caps = 0x401000; //mipmaps + texture
	return ret;
}

void s3tc1ReverseByte(u8& b)
{
	u8 b1 = b & 0x3;
	u8 b2 = b & 0xc;
	u8 b3 = b & 0x30;
	u8 b4 = b & 0xc0;
	b = (b1 << 6) | (b2 << 2) | (b3 >> 2) | (b4 >> 6);
}

int getBufferSize(u8 format, int w, int h, u8 paletteFormat, ColorCaps& cc);

void getPaletteFormat(ColorCaps& cc, u8 paletteFormat)
{
	//palettized images are converted to truecolor
	//images
	switch(paletteFormat)
	{
	case 0: //ia8
		getBufferSize(3, 0, 0, 0, cc);
		break;
	case 1: //r5g6b5
		getBufferSize(4, 0, 0, 0, cc);
		break;
	case 2: //rgb5a3
		getBufferSize(5, 0, 0, 0, cc);
		break;
	}
}

//returns the size of the image as stored in file
int getBufferSize(u8 format, int w, int h, u8 paletteFormat, /*out*/ ColorCaps& cc)
{
	//pad to 8 and 4 pixels:
	int w8 = w + (8 - w%8)%8;
	int w4 = w + (4 - w%4)%4;
	int h8 = h + (8 - h%8)%8;
	int h4 = h + (4 - h%4)%4;

	switch(format)
	{
	case 0: //i4
		//dds files don't support i4 - we convert to i8
		cc.flags = 0x20000; //luminance
		cc.rgbBitCount = 8;
		cc.rBitMask = 0xff;
		return w8*h8/2; //data is i4 in read buffer nevertheless

	case 1: //i8
		cc.flags = 0x20000; //luminance
		cc.rgbBitCount = 8;
		cc.rBitMask = 0xff;
		return w8*h4;
	case 2: //i4a4
		cc.flags = 0x20001; //alpha + luminance
		cc.rgbBitCount = 8;
		cc.rBitMask = 0xf;
		cc.aBitMask = 0xf0;
		return w8*h4;
	case 3: //i8a8
		cc.flags = 0x20001; //alpha + luminance
		cc.rgbBitCount = 16;
		cc.rBitMask = 0xff;
		cc.aBitMask = 0xff00;
		return w4*h4*2;
	case 4: //r5g6b5
		cc.flags = 0x40; //rgb
		cc.rgbBitCount = 16;
		cc.rBitMask = 0xf800;
		cc.gBitMask = 0x7e0;
		cc.bBitMask = 0x1f;
		return w4*h4*2;
	case 5: //gc homebrewn (rgb5a3)
		//this is a waste of memory, but there's no better
		//way to expand this format...
		cc.flags = 0x41; //rgb + alpha
		cc.rgbBitCount = 32;
		cc.rBitMask = 0xff0000;
		cc.gBitMask = 0xff00;
		cc.bBitMask = 0xff;
		cc.aBitMask = 0xff000000;
		return w4*h4*2; //data is rgb5a2 in read buffer nevertheless

	case 6: //r8g8b8a8
		cc.flags = 0x41; //rgb + alpha
		cc.rgbBitCount = 32;
		cc.rBitMask = 0xff0000;
		cc.gBitMask = 0xff00;
		cc.bBitMask = 0xff;
		cc.aBitMask = 0xff000000;
		return w4*h4*4;

	case 8: //index4
		getPaletteFormat(cc, paletteFormat);
		return w8*h8/2;

	case 9: //index8
		getPaletteFormat(cc, paletteFormat);
		return w8*h4;

	case 0xa: //index14x2
		getPaletteFormat(cc, paletteFormat);
		return w4*h4*2;

	case 0xe: //s3tc1
		cc.flags = 0x4; //fourcc
		strncpy(cc.fourCC, "DXT1", 4);
		return w4*h4/2;

	default:
		return -1;
	}
}


#if 0
//this function is old and buggy
void fix8x8Expand(u8* dest, const u8* src, int w, int h)
{
	//convert to i8 during block swapping
	int si = 0;
	for(int y = 0; y < h; y += 8)
		for(int x = 0; x < w; x += 8)
			for(int dy = 0; dy < 8; ++dy)
				for(int dx = 0; dx < 8; ++dx, ++si)
					if(x + dx < w && y + dy < h)
					{
						//http://www.mindcontrol.org/~hplus/graphics/expand-bits.html
						u8 t = (src[si/2] & 0xf);
						dest[w*(y + dy) + x + dx] = (t << 4) | t;
						t = src[si/2] & 0xf0;
						dest[w*(y + dy) + x + dx] = t | (t >> 4);
					}
}
#else
//new, fixed version
void fix8x8Expand(u8* dest, const u8* src, int w, int h)
{
	//convert to i8 during block swapping
	int si = 0;
	for(int y = 0; y < h; y += 8)
		for(int x = 0; x < w; x += 8)
			for(int dy = 0; dy < 8; ++dy)
				for(int dx = 0; dx < 8; dx += 2, ++si)
					if(x + dx < w && y + dy < h)
					{
						//http://www.mindcontrol.org/~hplus/graphics/expand-bits.html
						u8 t = src[si] & 0xf0;
						dest[w*(y + dy) + x + dx] = t | (t >> 4);
						t = (src[si] & 0xf);
						dest[w*(y + dy) + x + dx + 1] = (t << 4) | t;
					}
}
#endif

void fix8x8NoExpand(u8* dest, const u8* src, int w, int h)
{
	//convert to i8 during block swapping
	int si = 0;
	for(int y = 0; y < h; y += 8)
		for(int x = 0; x < w; x += 8)
			for(int dy = 0; dy < 8; ++dy)
				for(int dx = 0; dx < 8; dx += 2, ++si)
					if(x + dx < w && y + dy < h)
					{
						//http://www.mindcontrol.org/~hplus/graphics/expand-bits.html
						u8 t = src[si] & 0xf0;
						dest[w*(y + dy) + x + dx] = (t >> 4);
						t = (src[si] & 0xf);
						dest[w*(y + dy) + x + dx + 1] = t;
					}
}

void fix8x4(u8* dest, const u8* src, int w, int h)
{
	int si = 0;
	for(int y = 0; y < h; y += 4)
		for(int x = 0; x < w; x += 8)
			for(int dy = 0; dy < 4; ++dy)
				for(int dx = 0; dx < 8; ++dx, ++si)
					if(x + dx < w && y + dy < h)
						dest[w*(y + dy) + x + dx] = src[si];
}

void fix4x4(u16* dest, const u16* src, int w, int h)
{
	int si = 0;
	for(int y = 0; y < h; y += 4)
		for(int x = 0; x < w; x += 4)
			for(int dy = 0; dy < 4; ++dy)
				for(int dx = 0; dx < 4; ++dx, ++si)
					if(x + dx < w && y + dy < h)
						dest[w*(y + dy) + x + dx] = src[si];
	for(int i = 0; i < w*h; ++i)
		toWORD(dest[i]);
}

void fixRGBA8(u32* dest, const u16* src, int w, int h)
{
	//2 4x4 input tiles per 4x4 output tile, first stores AR, second GB
	int si = 0;
	for(int y = 0; y < h; y += 4)
		for(int x = 0; x < w; x += 4)
		{
			int dy;

			for(dy = 0; dy < 4; ++dy)
				for(int dx = 0; dx < 4; ++dx, ++si)
					if(x + dx < w && y + dy < h)
						dest[w*(y + dy) + x + dx] = aWORD(src[si]) << 16;

			for(dy = 0; dy < 4; ++dy)
				for(int dx = 0; dx < 4; ++dx, ++si)
					if(x + dx < w && y + dy < h)
						dest[w*(y + dy) + x + dx] |= aWORD(src[si]);
		}
}

u32 rgb5a3ToRgba8(u16 srcPixel)
{
	u8 r, g, b, a;

	//http://www.mindcontrol.org/~hplus/graphics/expand-bits.html
	if((srcPixel & 0x8000) == 0x8000) //rgb5
	{
		a = 0xff;

		r = (srcPixel & 0x7c00) >> 10;
		r = (r << (8-5)) | (r >> (10-8));

		g = (srcPixel & 0x3e0) >> 5;
		g = (g << (8-5)) | (g >> (10-8));

		b = srcPixel & 0x1f;
		b = (b << (8-5)) | (b >> (10-8));
	}
	else //rgb4a3
	{
		a = (srcPixel & 0x7000) >> 12;
		a = (a << (8-3)) | (a << (8-6)) | (a >> (9-8));

		r = (srcPixel & 0xf00) >> 8;
		r = (r << (8-4)) | r;

		g = (srcPixel & 0xf0) >> 4;
		g = (g << (8-4)) | g;

		b = srcPixel & 0xf;
		b = (b << (8-4)) | b;
	}

	return (a << 24) | (r << 16) | (g << 8) | b;
}

void fixRgb5A3(u32* dest, const u16* src, int w, int h)
{
	//convert to rgba8 during block swapping
	//4x4 tiles
	int si = 0;
	for(int y = 0; y < h; y += 4)
		for(int x = 0; x < w; x += 4)
			for(int dy = 0; dy < 4; ++dy)
				for(int dx = 0; dx < 4; ++dx, ++si)
					if(x + dx < w && y + dy < h)
					{
						u16 srcPixel = src[si];
						toWORD(srcPixel);
						dest[w*(y + dy) + x + dx] = rgb5a3ToRgba8(srcPixel);
					}
}

void fixS3TC1(u8* dest, const u8* src, int w, int h)
{
	int s = 0;

	for(int y = 0; y < h/4; y += 2)
		for(int x = 0; x < w/4; x += 2)
			for(int dy = 0; dy < 2; ++dy)
				for(int dx = 0; dx < 2; ++dx, s += 8)
					if(x + dx < w && y + dy < h)
						memcpy(&dest[8*((y + dy)*w/4 + x + dx)], &src[s], 8);

	for(int k = 0; k < w*h/2; k += 8)
	{
		toWORD(*(u16*)&dest[k]);
		toWORD(*(u16*)&dest[k+2]);

		s3tc1ReverseByte(dest[k+4]);
		s3tc1ReverseByte(dest[k+5]);
		s3tc1ReverseByte(dest[k+6]);
		s3tc1ReverseByte(dest[k+7]);
	}
}

int getUnpackedPixSize(u8 paletteFormat)
{
	int r = 2;
	if(paletteFormat == 2)
		r *= 2;
	return r;
}

void unpackPixel(int index, u8* dest, u16* palette, u8 paletteFormat)
{
	switch(paletteFormat)
	{
	case 0:
	case 1:
		memcpy(dest, &palette[index], 2);
		break;
	case 2:
		{
			u32 val = rgb5a3ToRgba8(palette[index]);
			memcpy(dest, &val, 4);
		}break;
	}
}

int unpack8(vector<u8>& data, int w, int h, u16* palette, u8 paletteFormat)
{
	int pixSize = getUnpackedPixSize(paletteFormat);
	int outSize = pixSize*w*h;
	u8* newData = new u8[outSize];
	u8* runner = newData;

	for(int y = 0; y < h; ++y)
		for(int x = 0; x < w; ++x, runner += pixSize)
			unpackPixel(data[y*w + x], runner, palette, paletteFormat);

	data.resize(outSize);
	memcpy(&data.front(), newData, outSize);
	delete [] newData;
	return outSize;
}

int unpack16(vector<u8>& data, int w, int h, u16* palette, u8 paletteFormat)
{
	int pixSize = getUnpackedPixSize(paletteFormat);
	int outSize = pixSize*w*h;
	u8* newData = new u8[outSize];
	u8* runner = newData;

	for(int y = 0; y < h; ++y)
		for(int x = 0; x < w; ++x, runner += pixSize)
			unpackPixel(((u16*)&data.front())[y*w + x] & 0x3fff, runner, palette, paletteFormat);

	data.resize(outSize);
	memcpy(&data.front(), newData, outSize);
	delete [] newData;
	return outSize;
}

void writeData(u8 format, int w, int h, u8* src, int size, FILE* f, u16* palette, u8 paletteFormat)
{
	//destSize stores size of "fixed" image
	int destSize = size;
	if(format == 0)
		destSize *= 2; //we have to convert from i4 to i8...
	else if(format == 5)
		destSize *= 2; //we have to convert from r5g5b5a3 to rgba8
	else if(format == 8)
		destSize *= 2; //we have to convert from ind4 to ind8...

	//u8* dest = new u8[destSize*2];
	vector<u8> dest2;
	dest2.resize(destSize*4);
	u8* dest=&dest2.front();
	//convert palettized images to "normal" images:
	switch(format)
	{
	case 0:
		fix8x8Expand(dest, src, w, h);
		break;

	case 1:
	case 2:
		fix8x4(dest, src, w, h);
		break;

	case 3:
	case 4:
		fix4x4((u16*)dest, (u16*)src, w, h);
		break;

	case 5:
		fixRgb5A3((u32*)dest, (u16*)src, w, h);
		break;

	case 6:
		fixRGBA8((u32*)dest, (u16*)src, w, h);
		break;


		//palette formats are unpacked because dds files
		//don't really support palettes
	case 8:
		fix8x8NoExpand(dest, src, w, h); //DON'T expand values
		destSize = unpack8(dest2, w, h, palette, paletteFormat);
		break;

	case 9:
		fix8x4(dest, src, w, h);
		destSize = unpack8(dest2, w, h, palette, paletteFormat);
		break;

	case 0xa:
		fix4x4((u16*)dest, (u16*)src, w, h);
		destSize = unpack16(dest2, w, h, palette, paletteFormat);
		break;



	case 0xe:
		fixS3TC1(dest, src, w, h);
		break;

	default:
		break;
	}

	fwrite(dest, destSize, 1, f);
	// delete [] dest;
}
/*
void doTexSave(const char* filename, const DdsHeader& ddsHead, int s,
const TextureHeader& tx, FILE* f, u16* palette, u8 paletteFormat)
{
FILE* outF = fopen(filename, "wb");
fwrite(&ddsHead, sizeof(ddsHead), 1, outF);

//image data
u8* buff0 = new u8[s];

int fac = 1;
for(int j = 0; j < ddsHead.numMips; ++j)
{
fread(buff0, 1, s/(fac*fac), f);
writeData(tx.format, ddsHead.width/fac, ddsHead.height/fac, buff0, s/(fac*fac), outF, palette, paletteFormat);
fac *= 2;
}

delete [] buff0;
fclose(outF);
}*/
void doTexSave(const char* filename, const DdsHeader& ddsHead, int s,
			   const TextureHeader& tx, unsigned char* pnt, u16* palette, u8 paletteFormat)
{
	FILE* outF = fopen(filename, "wb");
	if (outF)
	{
		fwrite(&ddsHead, sizeof(ddsHead), 1, outF);

		//image data
		u8* buff0 = new u8[s];

		int fac = 1;
		for(int j = 0; j < ddsHead.numMips; ++j)
		{
			// fread(buff0, 1, s/(fac*fac), f);
			memcpy(buff0,pnt,s/(fac*fac));
			writeData(tx.format, ddsHead.width/fac, ddsHead.height/fac, buff0, s/(fac*fac), outF, palette, paletteFormat);
			fac *= 2;
		}

		delete [] buff0;
		fclose(outF);
	}
}

void doTexSave(const char* filename, const DdsHeader& ddsHead, int s,
			   const TextureHeader& tx,FILE* f, u16* palette, u8 paletteFormat)
{
	FILE* outF = fopen(filename, "wb");
	if (outF)
	{
		fwrite(&ddsHead, sizeof(ddsHead), 1, outF);

		//image data
		u8* buff0 = new u8[s];

		int fac = 1;
		for(int j = 0; j < ddsHead.numMips; ++j)
		{
			fread(buff0, 1, s/(fac*fac), f);

			writeData(tx.format, ddsHead.width/fac, ddsHead.height/fac, buff0, s/(fac*fac), outF, palette, paletteFormat);
			fac *= 2;
		}

		delete [] buff0;
		fclose(outF);
	}
}

int dumpBti(string srcFile, string dstFile)
{
	FILE* f=fopen(srcFile.c_str(),"r+b");
	if(f){
		int tex1Offset = ftell(f);

		fseek(f, tex1Offset, SEEK_SET);

		TextureHeader tx;
		fread(&tx, sizeof(tx), 1, f);
		toWORD(tx.width);
		toWORD(tx.height);
		toWORD(tx.unknown2);
		toWORD(tx.paletteNumEntries);
		toDWORD(tx.paletteOffset);
		toDWORD(tx.unknown5);
		toWORD(tx.unknown6);
		toWORD(tx.unknown7);
		toWORD(tx.unknown9);
		toDWORD(tx.dataOffset);

		// cout << "type " << (int)tx.format << ", " << tx.width << "x" << tx.height << endl;

		u16* palette = NULL;
		if(tx.paletteNumEntries != 0)
		{
			/*  cout << "Found palette: " << tx.paletteNumEntries << " entries, format "
			<< (int)tx.paletteFormat << ", offset, " << tx.paletteOffset
			<< endl;

			assert(tx.paletteFormat == 0 || tx.paletteFormat == 1 || tx.paletteFormat == 2);*/

			palette = new u16[tx.paletteNumEntries];
			fseek(f, tx.paletteOffset, SEEK_SET);
			fread(palette, 2, tx.paletteNumEntries, f);
			for(int k = 0; k < tx.paletteNumEntries; ++k)
				toWORD(palette[k]);
		}

		DdsHeader ddsHead = createDdsHeader(tx.width, tx.height, tx.mipmapCount);
		int s = getBufferSize(tx.format, tx.width, tx.height, tx.paletteFormat, ddsHead.colorCaps);

		if(s == -1) //unsupported format
		{
			//cout << endl << "UNSUPPORTED FORMAT " << (int)tx.format << "!!!" << endl << endl;
			return 1 ;
		}

		fseek(f, tx.dataOffset, SEEK_SET);

		char filename[1000]={0};
		sprintf(filename, dstFile.c_str());
		doTexSave(filename, ddsHead, s, tx, f, palette, tx.paletteFormat);

		if(palette != NULL)
			delete [] palette;
	}else{
		return 1;
	}
	return 0;
}
int dumpBti(vector<unsigned char>* btiArray, string dstFile)
{
	// FILE* f=fopen(srcFile.c_str(),"r+b");



	TextureHeader tx;
	//fread(&tx, sizeof(tx), 1, f);
	memcpy(&tx,&btiArray->front(),0x20);
	toWORD(tx.width);
	toWORD(tx.height);
	toWORD(tx.unknown2);
	toWORD(tx.paletteNumEntries);
	toDWORD(tx.paletteOffset);
	toDWORD(tx.unknown5);
	toWORD(tx.unknown6);
	toWORD(tx.unknown7);
	toWORD(tx.unknown9);
	toDWORD(tx.dataOffset);

	// cout << "type " << (int)tx.format << ", " << tx.width << "x" << tx.height << endl;

	u16* palette = NULL;
	if(tx.paletteNumEntries != 0)
	{
		/*  cout << "Found palette: " << tx.paletteNumEntries << " entries, format "
		<< (int)tx.paletteFormat << ", offset, " << tx.paletteOffset
		<< endl;

		assert(tx.paletteFormat == 0 || tx.paletteFormat == 1 || tx.paletteFormat == 2);*/

		palette = new u16[tx.paletteNumEntries];
		// fseek(f, tx.paletteOffset, SEEK_SET);
		//  fread(palette, 2, tx.paletteNumEntries, f);
		memcpy(palette,&btiArray[tx.paletteOffset],tx.paletteNumEntries*2);
		for(int k = 0; k < tx.paletteNumEntries; ++k)
			toWORD(palette[k]);
	}

	DdsHeader ddsHead = createDdsHeader(tx.width, tx.height, tx.mipmapCount);
	int s = getBufferSize(tx.format, tx.width, tx.height, tx.paletteFormat, ddsHead.colorCaps);

	if(s == -1) //unsupported format
	{
		//cout << endl << "UNSUPPORTED FORMAT " << (int)tx.format << "!!!" << endl << endl;
		return 1 ;
	}



	char filename[1000]={0};
	sprintf(filename, dstFile.c_str());
	doTexSave(filename, ddsHead, s, tx, &btiArray->front() + 0x20, palette, tx.paletteFormat);

	if(palette != NULL)
		delete [] palette;

	return 0;
}

/*
int main(int argc, char* argv[])
{
FILE* f;
if(argc < 2 || (f = fopen(argv[1], "rb")) == NULL)
return EXIT_FAILURE;

string outFilename = argv[1] + string(".dds");
dumpBti(f, outFilename);

fclose(f);

//system("start out.dds");
//system("pause");

return EXIT_SUCCESS;
}*/