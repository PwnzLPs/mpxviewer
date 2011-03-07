#include "stdafx.h"

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
	string tmp;
	EntryHeader eh;
	fseek(f, 0, SEEK_SET);
	fread(&h, sizeof(h), 1, f);
	char sname[1000] = {0};
	char tmp2[256]={0};
	toWORD(h.unk1);
	toWORD(h.unk2);
	toDWORD(h.unk3);
	//DUMP HEADER FIRST.
	tmp=name+"/header.bin";
	FILE* fp=fopen(tmp.c_str(),"w+b");
	if(fp){

		fwrite (&h, sizeof(h), 1, fp);
		fclose(fp);
	}
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
		memset(sname,0,sizeof(name));
		fread(&ne, sizeof(ne), 1, f);
		toDWORD(ne.id);
		toDWORD(ne.nameLength);
		
		fread(sname, 1, ne.nameLength, f);
		char tmp3[6]={0};
		//sprintf(tmp3,"%s",ne.tag);
		memcpy(tmp3,ne.tag,4);
		sprintf(tmp2,"/%X.%s.name.txt",ne.id,tmp3);
		tmp=name+tmp2;
		FILE* fp=fopen(tmp.c_str(),"w");
		fseek(fp,0,SEEK_SET);
		if(fp){
			
			fprintf(fp,"%s",sname);
			fclose(fp);
			
		}
		cout << string(ne.tag, 4) << " " << ne.id << " " 
		<< string(name, ne.nameLength) << endl;
	}
	
	//read entry header
	
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
		toDWORD(e.isCompressed);
		
		toDWORD(e.id);
		toDWORD(e.length);
		toDWORD(e.offset);
		
		cout << e.isCompressed << " " << string(e.tag, 4) << " id: " << e.id
		<< " length: " << e.length
		<< " offset: " << e.offset << " " << endl;
		
		assert(e.isCompressed == 0 || e.isCompressed == 1);
		count += e.isCompressed;
		
		//dump file
		std::ostringstream nameStream;
		nameStream << name << "\\" << e.isCompressed << "_" << hex << e.id << "."
		<< string(e.tag, 4);
		dumpFile(nameStream.str(), f, e.offset, e.length);
	}
	
	cout << dec;
	cout << count << " ones, " << eh.entryCount - count << " zeroes." << endl;
	cout << eh.entryCount << " entries, " << neh.entryCount << " name entries"
	<< endl;
}
//FIX _v3 later
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
		toDWORD(e.isCompressed);
		
		toQWORD(e.id);
		toDWORD(e.length);
		toDWORD(e.offset);
		
		cout << e.isCompressed << " " << string(e.tag, 4) << " id: " << ((u32*)&e.id)[1] << ((u32*)&e.id)[0]
		<< " length: " << e.length
		<< " start: " << dataStart
		<< " offset: " << e.offset << " " << endl;
		
		assert(e.isCompressed == 0 || e.isCompressed == 1);
		count += e.isCompressed;
		
		//dump file
		std::ostringstream nameStream;
		nameStream << name << "\\" << e.isCompressed << "_" << hex << ((u32*)&e.id)[1] << ((u32*)&e.id)[0] << "."
		<< string(e.tag, 4);
		dumpFile(nameStream.str(), f, dataStart + e.offset, e.length);
	}
	
	cout << dec;
	cout << count << " ones, " << eh.entryCount - count << " zeroes." << endl;
	cout << eh.entryCount << " entries, " << neh.entryCount << " name entries"
	<< endl;
}
