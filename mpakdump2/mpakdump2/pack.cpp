#include "stdafx.h"
#include "dirent.h"//Change to <> on linux?
using namespace std;

long insertFile(std::string name, FILE* dst)
{
	//int t = ftell(src);
	//fseek(src, offset, SEEK_SET);
	
	//FILE* out = fopen(name.c_str(), "wb");
	//if(out == NULL)
	//	return;
	
	u32 read = 0;
	u8 buff[1024];
	FILE* fp=fopen(name.c_str(),"r+b");
	int size=0;
	vector<unsigned char> mFile;
	if(fp){
		//read file
		fseek(fp,0,SEEK_END);
		size=ftell(fp);
		fseek(fp,0, SEEK_SET);
		mFile.resize(size);
		fread(&mFile[0],1,size,fp);
		fwrite(&mFile[0],1,size,dst);
		while(ftell(dst)%4) fputc(0xFF, dst);

	}
	fclose(fp);
	return size;
}

inline u32 FromString(const std::string& s)

{

	u32 o;

	std::stringstream stream;

	stream << hex;

	stream << s;

	stream >> o;

	return o;

}
void FillEntry(Entry* e, char* FileN){
	string name;
	char tmp[256]={0};
	int i=0;
	for(i=strlen(FileN);i>0;i--){
		if(FileN[i]=='\\'||FileN[i]=='/') break;


	}
	name = &FileN[i];
	if(name.c_str()[0]=='0'){
		e->isCompressed = 0;
	}else{
		e->isCompressed = 1;
	}
	int start = name.find("_")+1;
	int end = name.find(".");
	memcpy(tmp,&name.c_str()[start],end-start);
	e->id = FromString(string(tmp));
	memcpy(e->tag, &name.c_str()[end+1],4);
	name=name;
}
void PackEntries(char* dir, FILE* mainfile, vector<string> FileNames){
	int i;
	EntryHeader neh;
	neh.entryCount =0;
	MPakHeader h;
	size_t found;
	char names[1000];
	DIR *pakdir;
	struct dirent *ent;
	FILE* fdata;

	
	vector<string> FileEntries;
	;
	string tmp;
	
	for(i=0;i<FileNames.size();i++){
		
		if(FileNames[i].find(".name.txt")!=string::npos) continue;
		if(FileNames[i]=="header.bin") continue;
		FileEntries.push_back(FileNames[i]);
		



	}
	//read entry header
	EntryHeader eh;
	eh.entryCount = FileEntries.size();
	toDWORD(eh.entryCount);
	fwrite(&eh, sizeof(eh), 1,  mainfile);


	
	//output entries
	cout << string(70, '-') << endl
	<< FileEntries.size() << " entries: " << endl;
	int count = 0;
	int startpos=ftell(mainfile);
	fdata=fopen("data.bin","w+b");
	fseek(fdata,0,SEEK_SET);
	for(i = 0; i < FileEntries.size(); ++i)
	{
		Entry e;
		FillEntry(&e,(char*)FileEntries[i].c_str());
		int loc=ftell(fdata);
		e.offset = startpos + sizeof(e)*FileEntries.size() + loc;
		e.length = insertFile(string(dir)+string("/")+FileEntries[i],fdata);
		
		toDWORD(e.isCompressed);
		
		toDWORD(e.id);
		
		toDWORD(e.length);
		toDWORD(e.offset);
		fwrite(&e, sizeof(e), 1,  mainfile);
		cout << e.isCompressed << " " << string(e.tag, 4) << " id: " << e.id
		<< " length: " << e.length
		<< " offset: " << e.offset << " " << endl;
		
		assert(e.isCompressed == 0 || e.isCompressed == 1);
		count += e.isCompressed;
		
		//dump file
		std::ostringstream nameStream;
		nameStream << names << "\\" << e.isCompressed << "_" << hex << e.id << "."
		<< string(e.tag, 4);
		
	}
	fseek(fdata,0,SEEK_END);
	int mysize=ftell(fdata);
	vector<unsigned char> data;
	data.resize(mysize);
	fseek(fdata,0,SEEK_SET);
	fread(&data[0],1,data.size(),fdata);
	fwrite(&data[0],1,data.size(),mainfile);
	fclose(fdata);
	remove("data.bin");
	
}
void PackNames(char* dir, FILE* mainfile, vector<string> FileNames){
	int i;
	EntryHeader neh;
	neh.entryCount =0;
	MPakHeader h;
	size_t found;
	char names[1000];
	DIR *pakdir;
	string tmp;
	struct dirent *ent;
	vector<string> NameEntries;
	for(i=0;i<FileNames.size();i++){
		found=FileNames[i].find(".name.txt");
		if(found!=string::npos){
			NameEntries.push_back(FileNames[i]);
		}

	}

	//read name entry header
	neh.entryCount = NameEntries.size();
	toDWORD(neh.entryCount);
	fwrite(&neh, sizeof(neh), 1, mainfile);
	
	cout << hex;
	
	//output name entries
	cout << string(70, '-') << endl
	<< neh.entryCount << " name entries: " << endl;
	for(i = 0; i < NameEntries.size(); ++i)
	{
		int j = 0;
		NameEntry ne;
		char idd[1000]={0};
		memcpy(idd,NameEntries[i].c_str(),NameEntries[i].length());
		for(;j<strlen(idd);j++){
			if(idd[j]=='.'){
				idd[j]='\0';//end the string
			}
		}
		//ne.
		//READ THE STRING
		tmp=dir;
		tmp+="/";
		tmp+=NameEntries[i];
		FILE* fp2=fopen(tmp.c_str(),"r");
		if(fp2){
			fscanf(fp2,"%s",names);
			fclose(fp2);
			ne.nameLength = strlen(names);
			ne.id=strtol(idd,NULL,16);
			cout <<"Packing " << string(ne.tag, 4) << " " << ne.id << " " 
			<< string(names, ne.nameLength) << endl;
			toDWORD(ne.id);
			toDWORD(ne.nameLength);
			memcpy(ne.tag,&NameEntries[i].c_str()[j],4);
			fwrite(&ne, sizeof(ne), 1, mainfile);
			fwrite(names, 1, strlen(names),  mainfile);
		}

	}


}
void mpakPack_v1(char* dir)
{
	int i;
	EntryHeader neh;
	neh.entryCount =0;
	MPakHeader h;
	size_t found;
	char names[1000];
	DIR *pakdir;
	struct dirent *ent;
	vector<string> FileNames;
	
	vector<string> FileEntries;
	;
	string tmp;
	/* open directory stream */
	pakdir = opendir (dir);
	
	//DUMP HEADER FIRST.
	tmp=string(dir)+string("/header.bin");
	FILE* fp=fopen(tmp.c_str(),"r+b");
	
	if(fp){

		fread (&h, sizeof(h), 1, fp);
		fclose(fp);
	}
	toWORD(h.unk1);
	toWORD(h.unk2);
	toDWORD(h.unk3);
	fp=fopen("newpak.pak","w+b");
	if(fp){
		fwrite(&h, sizeof(h), 1, fp);
		
	}

	//Process all file names 
	if (pakdir != NULL) {

		/* print all the files and directories within directory */
		while ((ent = readdir (pakdir)) != NULL) {
			
			switch (ent->d_type) {
			case DT_REG:
				tmp=ent->d_name;
				FileNames.push_back(tmp);

				break;

				
			}
		}

		closedir (pakdir);
	} else {
		/* could not open directory */
		perror ("");
		return;
	}
	//Determine number of name entry headers.
	PackNames(dir, fp, FileNames);
	PackEntries(dir, fp, FileNames);
	fclose(fp);
	cout << dec;
	//cout << count << " ones, " << eh.entryCount - count << " zeroes." << endl;
	//cout << eh.entryCount << " entries, " << neh.entryCount << " name entries"
	//	<< endl;
}
