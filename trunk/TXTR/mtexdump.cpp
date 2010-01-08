//mtexdump
//by thakis
//v1.0

//converts a metroid txtr file to a bti file
//(bti was chosen as output format because
//it was the least work for me ;-) )
#include "..\main.h"
//#pragma pack(push, 1)

/*
int log2(int i)
{
  int ret = 0;
  while((i & 1) == 0)
  {
    ++ret;
    i >>= 1;
  }

  return ret;
}*/

int dumpTxtr(string srcFile, string dstFile)
{
	FILE* f=fopen(srcFile.c_str(),"r+b");
	if(f){
		BtiTextureHeader outH;
		memset(&outH, 0, sizeof(outH));

		TxtrHeader h;
		fread(&h, sizeof(h), 1, f);
		toDWORD(h.format);
		toWORD(h.w);
		toWORD(h.h);
		toDWORD(h.numMips);

		outH.width = h.w;
		outH.height = h.h;
		outH.mipmapCount = h.numMips;
		outH.dataOffset = sizeof(outH);

		//read palette header
		if(h.format == 4 || h.format == 5)
		{
			PaletteHeader ph;
			fread(&ph, sizeof(ph), 1, f);
			toDWORD(ph.format);
			toWORD(ph.unk);
			toWORD(ph.unk2);

			outH.paletteFormat = ph.format;

			//TODO...
			outH.paletteNumEntries = h.format == 4 ? 16 : 256;
			outH.paletteOffset = outH.dataOffset;

			int paletteSize = outH.paletteNumEntries*2;
			outH.dataOffset += paletteSize;
		}

		toWORD(outH.width);
		toWORD(outH.height);
		toDWORD(outH.dataOffset);
		toWORD(outH.paletteNumEntries);
		toDWORD(outH.paletteOffset);

		//set output format
		int size = 0;
		switch(h.format)
		{
		case 0:
			outH.format = 0;
			size = h.w*h.h/2;
			break;

		case 1:
			outH.format = 1;
			size = h.w*h.h;
			break;

		case 2:
			outH.format = 2;
			size = h.w*h.h;
			break;

		case 3:
			outH.format = 3;
			size = h.w*h.h*2;
			break;

		case 4: //TODO, doesn't work
			outH.format = 0;
			size = h.w*h.h/2;
			break;

		case 5:
			outH.format = 9;
			size = h.w*h.h;
			break;

		case 7:
			outH.format = 4;
			size = h.w*h.h*2;
			break;

		case 8:
			outH.format = 5;
			size = h.w*h.h*2;
			break;

		case 9:
			outH.format = 6;
			size = h.w*h.h*4;
			break;

		case 10:
			outH.format = 14;
			size = h.w*h.h/2;
			break;
		}

		//compute image data size
		int imageSize = 0;
		for(int i = 0, s = 1; i < h.numMips; ++i, s *= 2)
			imageSize += size/(s*s);

		FILE* out = fopen(dstFile.c_str(), "wb");
		if(out == NULL)
			return 1;

		fwrite(&outH, sizeof(outH), 1, out);
		while(!feof(f))
			fputc(fgetc(f), out);

		//after the dxt image data, an image footer is stored
		//in the file...let's don't care about it for now

		fclose(out);
	}else{
		return 1;//Errored
	}
	return 0;
}

long RFL(FILE* fp){//Return File Length
	long curroffset=ftell(fp);
	long size=0;
	fseek(fp,0,SEEK_END);
	size=ftell(fp);
	fseek(fp,curroffset,SEEK_SET);
	return size;
}

int dumpTxtr(string srcFile, vector<unsigned char>* Data)//Writes the bti data to Data.
{
	FILE* f=fopen(srcFile.c_str(),"r+b");
	if(f){
		BtiTextureHeader outH;
		memset(&outH, 0, sizeof(outH));

		TxtrHeader h;
		fread(&h, sizeof(h), 1, f);
		toDWORD(h.format);
		toWORD(h.w);
		toWORD(h.h);
		toDWORD(h.numMips);

		outH.width = h.w;
		outH.height = h.h;
		outH.mipmapCount = h.numMips;
		outH.dataOffset = sizeof(outH);

		//read palette header
		if(h.format == 4 || h.format == 5)
		{
			PaletteHeader ph;
			fread(&ph, sizeof(ph), 1, f);
			toDWORD(ph.format);
			toWORD(ph.unk);
			toWORD(ph.unk2);

			outH.paletteFormat = ph.format;

			//TODO...
			outH.paletteNumEntries = h.format == 4 ? 16 : 256;
			outH.paletteOffset = outH.dataOffset;

			int paletteSize = outH.paletteNumEntries*2;
			outH.dataOffset += paletteSize;
		}

		toWORD(outH.width);
		toWORD(outH.height);
		toDWORD(outH.dataOffset);
		toWORD(outH.paletteNumEntries);
		toDWORD(outH.paletteOffset);

		//set output format
		int size = 0;
		switch(h.format)
		{
		case 0:
			outH.format = 0;
			size = h.w*h.h/2;
			break;

		case 1:
			outH.format = 1;
			size = h.w*h.h;
			break;

		case 2:
			outH.format = 2;
			size = h.w*h.h;
			break;

		case 3:
			outH.format = 3;
			size = h.w*h.h*2;
			break;

		case 4: //TODO, doesn't work
			outH.format = 0;
			size = h.w*h.h/2;
			break;

		case 5:
			outH.format = 9;
			size = h.w*h.h;
			break;

		case 7:
			outH.format = 4;
			size = h.w*h.h*2;
			break;

		case 8:
			outH.format = 5;
			size = h.w*h.h*2;
			break;

		case 9:
			outH.format = 6;
			size = h.w*h.h*4;
			break;

		case 10:
			outH.format = 14;
			size = h.w*h.h/2;
			break;
		}

		//compute image data size
		int imageSize = 0;
		for(int i = 0, s = 1; i < h.numMips; ++i, s *= 2)
			imageSize += size/(s*s);

		Data->resize(RFL(f)-0xC+0x20);

		memcpy(&Data->front(),&outH,0x20);
		/* while(!feof(f))
		fputc(fgetc(f), out);*/
		fread(&Data->front()+0x20,1,Data->size()-0x20,f);

		//after the dxt image data, an image footer is stored
		//in the file...let's don't care about it for now

		fclose(f);
	}else{
		return 1;//Errored
	}
	return 0;
}
/*
int main(int argc, char* argv[])
{
  FILE* f;
  if(argc < 2 || (f = fopen(argv[1], "rb")) == NULL)
    return EXIT_FAILURE;

  string outFilename = argv[1] + string(".bti");
  dumpTxtr(f, outFilename);

  fclose(f);

  return EXIT_SUCCESS;
}*/
