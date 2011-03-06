// mpakdump2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sys/stat.h>
#include "dirent.h"
//mpakdump
//by thakis
//v1.0
//v2.0 - 2009/10/06 - modified to add support for Metroid Prime 3 - by revel8n



int isDir(const char *dname) {
    DIR *pakdir;
    struct dirent *ent;
    pakdir = opendir (dname);
    int val=0;
	if(pakdir!=NULL) val=1;
	return val;


}
#pragma pack(push, 1)



#pragma pack(pop)
void MakeMP12Head(char* dir){
MPakHeader h;
string f=dir;

	//DUMP HEADER FIRST.
  
	h.unk1 = 3;
	h.unk2 = 5;
	h.unk3 = 0;
    toWORD(h.unk1);
	toWORD(h.unk2);
 	toDWORD(h.unk3);
    f+="header.bin";
	FILE* fp=fopen(f.c_str(),"w+b");
	if(fp){

		fwrite (&h, sizeof(h), 1, fp);
		fclose(fp);
	}
}
void MakeMP3Head(char* dir){
MPakHeader h;
string f=dir;
    h.unk3=0;
	//DUMP HEADER FIRST.
    h.unk1 = 0;
	h.unk2 = 2;
    toWORD(h.unk1);
	toWORD(h.unk2);
 	toDWORD(h.unk3);
    f+="header.bin";
	FILE* fp=fopen(f.c_str(),"w+b");
	if(fp){

		fwrite (&h, sizeof(h), 1, fp);
		fclose(fp);
	}
}

void mpakPak(char* dir)
{
	MPakHeader h;
	std::string name=dir;
	name+="header.bin";
	int t;
	FILE*fp=fopen(name.c_str(),"r+b");
	if(fp){
		fseek(fp, 0, SEEK_SET);
		
		fread(&h, sizeof(h), 1, fp);
		toWORD(h.unk1);
		toWORD(h.unk2);
		toDWORD(h.unk3);
		
		if (h.unk1 == 3 && h.unk2 == 5)
		{
			mpakPack_v1(dir);
		}
		else if (h.unk1 == 0 && h.unk2 == 2)
		{
			//mpakDump_v3(fp, name);
		}
	}else{
          printf("No header file was detected.\nPlease choose a pak type below.\nType 1 for MP1 and 2 paks\nType 2 for MP3 Paks\n");
		  scanf("%d",t);
		  if(t&2){
				MakeMP3Head(dir);
			  //mpakPack_v2(dir)
		  }else{
			  mpakPack_v1(dir);
			 //  MakeMP12Head(dir);
		  }

	}
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
int findextension(char* str){
	string file=str;
	return file.find(".");
    

}
int main(int argc, char* argv[])
{
	FILE* f;
	if(!isDir(argv[1])){
	if(argc < 2 || (f = fopen(argv[1], "rb")) == NULL)
		return EXIT_FAILURE;
	
		std::string dirName = argv[1] + string("_dir");
		mkdir(dirName.c_str());
		
		mpakDump(f, dirName);
		
		fclose(f);
	}else{
		mpakPak(argv[1]);
	}
	system("pause");
	
	return 0;
}
