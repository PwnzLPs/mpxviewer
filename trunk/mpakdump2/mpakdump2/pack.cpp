#include "stdafx.h"
#include "dirent.h"//Change to <> on linux?
using namespace std;
void mpakPack_v1(char* dir)
{
	int i;
	EntryHeader neh;
	neh.entryCount =0;
	MPakHeader h;
     DIR *pakdir;
    struct dirent *ent;
    vector<string> FileNames;
     string tmp;
    /* open directory stream */
    pakdir = opendir (dir);
	
	//DUMP HEADER FIRST.
    FILE* fp=fopen("header.bin","r+b");
	
	if(fp){

		fread (&h, sizeof(h), 1, fp);
		fclose(fp);
	}
	fp=fopen("newpak.pak","w+b");
	if(fp){
		fwrite(&h, sizeof(h), 1, fp);
		fclose(fp);
	}

	//Process all file names 
if (pakdir != NULL) {

      /* print all the files and directories within directory */
      while ((ent = readdir (pakdir)) != NULL) {
		  neh.entryCount++;
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


	//read name entry header
	neh.entryCount -=1;
	
	toDWORD(neh.entryCount);
	fwrite(&neh, sizeof(neh), 1, fp);
	neh.entryCount +=1;//Why? Because header.bin
	cout << hex;
	
	//output name entries
	cout << string(70, '-') << endl
		<< neh.entryCount << " name entries: " << endl;
	for(i = 0; i < neh.entryCount; ++i)
	{
		NameEntry ne;
		//ne.
		toDWORD(ne.id);
		toDWORD(ne.nameLength);
		fread(&ne, sizeof(ne), 1, fp);
		char name[1000] = {0}; fread(name, 1, ne.nameLength, fp);
		
		cout << string(ne.tag, 4) << " " << ne.id << " " 
			<< string(name, ne.nameLength) << endl;
	}
	
	//read entry header
	EntryHeader eh;
	fread(&eh, sizeof(eh), 1, fp);
	toDWORD(eh.entryCount);
	
	//output entries
	cout << string(70, '-') << endl
		<< eh.entryCount << " entries: " << endl;
	int count = 0;
	for(i = 0; i < eh.entryCount; ++i)
	{
		Entry e;
		fread(&e, sizeof(e), 1, fp);
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
		/*nameStream << name << "\\" << e.unk << "_" << hex << e.id << "."
			<< string(e.tag, 4);*/
		dumpFile(nameStream.str(), fp, e.offset, e.length);
	}
	
	cout << dec;
	cout << count << " ones, " << eh.entryCount - count << " zeroes." << endl;
	cout << eh.entryCount << " entries, " << neh.entryCount << " name entries"
		<< endl;
}
