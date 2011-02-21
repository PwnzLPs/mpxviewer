#include "main.h"

clsMaterialHeader::clsMaterialHeader(){

	TextureCount=0;

	MaterialCount=0;
	MaterialSegmentOffsets.clear();//new this as MaterialCount
	//MaterialSegments = NULL;
}


clsMaterialHeader::~clsMaterialHeader(){

	TextureCount=0;

	MaterialCount=0;
	MaterialSegmentOffsets.clear();//new this as MaterialCount
	//	delete[] MaterialSegments;
	//	MaterialSegments = NULL;
	if (!TexturesIDs.empty())
	{
		glDeleteTextures((GLsizei)TexturesIDs.size(), &TexturesIDs.front());
	}
	TexturesIDs.clear();
}
void clsMaterialHeader::ReadMaterialDataMP1_2(FILE* fp, unsigned int iSegment, std::vector<u32> & segmentSizes){
	u32 i=0;
	u32 startoffset=ftell(fp);
	u32 curroffset=0;
	fread(&TextureCount,4,1,fp);
	toDWORD((u32&)TextureCount);
	TextureNames.resize(TextureCount);

	if (!TexturesIDs.empty())
	{
		glDeleteTextures((GLsizei)TexturesIDs.size(), &TexturesIDs.front());
	}
	TexturesIDs.resize(TextureCount);
	if (!TexturesIDs.empty())
	{
		memset(&TexturesIDs.front(), 0, TexturesIDs.size() * sizeof(GLuint));
	}

	for(i=0;i<TextureCount;i++){
		fread(&TextureNames[i],4,1,fp);
		toDWORD((u32&)TextureNames[i]);
	}

	fread(&MaterialCount,4,1,fp);
	toDWORD((u32)MaterialCount);
	MaterialSegmentOffsets.resize(MaterialCount);
	MaterialSegment.resize(MaterialCount);
	for(i=0;i<MaterialCount;i++){
		fread(&MaterialSegmentOffsets[i],4,1,fp);
		toDWORD((u32&)MaterialSegmentOffsets[i]);
	} 
	curroffset=ftell(fp);
	//MaterialSegments=new vector<unsigned char>[MaterialCount];
	MaterialSegments.resize(MaterialCount);
	for(i=0;i<MaterialCount;i++){
		int segmentSize = MaterialSegmentOffsets[i];
		// calculate sizes from offsets
		if (i > 0)
		{
			segmentSize -= MaterialSegmentOffsets[i-1];
		}
		MaterialSegments[i].resize(segmentSize);
	}  

	for(i=0;i<MaterialCount;i++){
		if(i!=0){ 
			fseek(fp,curroffset+MaterialSegmentOffsets[i-1],SEEK_SET);
		}else{
			fseek(fp,curroffset,SEEK_SET);
		}

		fread(&MaterialSegments[i].front(),1,MaterialSegments[i].size(),fp);
		ReadMaterialSegment(i);

	}

	fseek(fp, startoffset+segmentSizes[iSegment], SEEK_SET);
}
void clsMaterialHeader::ReadMaterialDataMP3(FILE* fp, unsigned int iSegment, std::vector<u32> & segmentSizes){
	u32 i=0;
	u32 startoffset=ftell(fp);
	u32 curroffset=0;
/*
	unsigned long group;
	fread(&group,4,1,fp);
	toDWORD(group);
	MaterialSegment.resize(group);
	
	int groups;//Lost my mind here supposed to load the textures per group. Good luck
	
	



		//Find out how many textures we have

	if (!TexturesIDs.empty())
	{
		glDeleteTextures((GLsizei)TexturesIDs.size(), &TexturesIDs.front());
	}
	TexturesIDs.resize(TextureCount);
	if (!TexturesIDs.empty())
	{
		memset(&TexturesIDs.front(), 0, TexturesIDs.size() * sizeof(GLuint));
	}
	int cntTex=0;
	for(i=0;i<2;i++){
   u32 strt=ftell(fp);

									 while(sz > 0){
										  uint32 scriptCommand=0;
										  fread(&scriptCommand,1,4,fp);
										  toDWORD(scriptCommand);
											sz -=4;
										   if(scriptCommand==0x50415353)
										   {//PASS
											uint32 passValue;
											fread(&passValue,1,4,fp);
											sz-=4;
										   }
										   if(scriptCommand==0x434c5220)
										   {//CLR
											 uint32 clrValue;
											 fread(&clrValue,1,4,fp);
											 toDWORD(clrValue);
											  sz-=4;
											 if(clrValue==scriptCommand){
												 uint32 colorRGB;
												 fread(&colorRGB,1,4,fp);
											     sz-=4;
											  }else{
												//uint64 texture<format=hex>;
												if(i==0){
													TextureCount++;
												}else{
                                                     fread(&TextureNames[cntTex],1,sizeof(u64),fp);
													 toDWORD64(TextureNames[cntTex]);
													cntTex++;
												}
                                                uint64 filler;
												fread(&filler,1,4,fp);
												  sz-=16;
												}
											}
											if(scriptCommand==0x454e4420)
											{//END
												sz-=4;
										   
											}
										    if(scriptCommand==0x494e4341)
											{//INCA
												uint32 Unknown1;
												fread(&Unknown1,4,1,fp);
												//uint64 Texture<format=hex>;;
												if(i==0){
													TextureCount++;
												}else{
                                                     fread(&TextureNames[cntTex],1,sizeof(u64),fp);
													 toDWORD64(TextureNames[cntTex]);
													cntTex++;
												}
												uint64 Unknown2;
											   fread(&Unknown2,4,1,fp);
											    sz-=20;
											}
											if(scriptCommand==0x52464c44)
											{//RFLD
											    uint32 Unknown1;
												fread(&Unknown1,4,1,fp);
												if(i==0){
													TextureCount++;
												}else{
                                                     fread(&TextureNames[cntTex],1,sizeof(u64),fp);
													 toDWORD64(TextureNames[cntTex]);
													cntTex++;
												}
												//uint32 Filler<format=hex>;
												fread(&Unknown1,4,1,fp);
												//uint32 Unknown2;
												fread(&Unknown1,4,1,fp);
												//uint16 Unknown3;
												//uint16 Unknown4;
												fread(&Unknown1,4,1,fp);
												//uint32 Filler2;
												fread(&Unknown1,4,1,fp);
												sz-=0x1C;
											
											}
											if(scriptCommand==0x52464c56)
											{//RFLV
											    uint32 Unknown1;
												fread(&Unknown1,4,1,fp);
												if(i==0){
													TextureCount++;
												}else{
                                                     fread(&TextureNames[cntTex],1,sizeof(u64),fp);
													 toDWORD64(TextureNames[cntTex]);
													cntTex++;
												}
												//uint64 Unknown2;
												fread(&Unknown1,4,1,fp);
												fread(&Unknown1,4,1,fp);
											    sz-=20;
											}	
											if(scriptCommand==0x494e5420)
											{//RFLV
											    uint32 Value;
												fread(&Value,4,1,fp);
												//uint32 filler;
												fread(&Value,4,1,fp);
											    sz-=8;
											}
                                            if(scriptCommand==0x58524159)
											{//XRAY
											    uint32 Unknown1;
												fread(&Unknown1,4,1,fp);
												if(i==0){
													TextureCount++;
												}else{
                                                     fread(&TextureNames[cntTex],1,sizeof(u64),fp);
													 toDWORD64(TextureNames[cntTex]);
													cntTex++;
												}
												//uint32 Filler<format=hex>;
												fread(&Unknown1,4,1,fp);
												//uint32 Unknown2;
												fread(&Unknown1,4,1,fp);
												//uint16 Unknown3;
												//uint16 Unknown4;
												fread(&Unknown1,4,1,fp);
												//uint32 Filler2;
												fread(&Unknown1,4,1,fp);
												sz-=0x1C;
											
											}											
										
									 }
    if (!TexturesIDs.empty())
	{
		glDeleteTextures((GLsizei)TexturesIDs.size(), &TexturesIDs.front());
	}
	TexturesIDs.resize(TextureCount);
	TextureNames.resize(TextureCount);
	if (!TexturesIDs.empty())
	{
		memset(&TexturesIDs.front(), 0, TexturesIDs.size() * sizeof(GLuint));
	}
	
	}
	fseek(fp, startoffset+segmentSizes[iSegment], SEEK_SET);*/
}
long ReadTextureInfo(txtrinfo* info, unsigned char* pnt){
	u32 i=0;



	memcpy(&info->count,pnt,4);//Read Texture information for this segment
	toDWORD((u32&)info->count);
	info->ids.resize(info->count);
	for(i=0;i<info->count;i++){
		memcpy(&info->ids[i],&pnt[4+i*4],4);
		toDWORD((u32&)info->ids[i]);
	}
	return 4+info->count*4;
}

void clsMaterialHeader::ReadMaterialSegment(int index){
	u32 ReadBytes=0;
	u32 i=0;

	unsigned char* pnt=&MaterialSegments[index].front();
	mGroup* ThisSegment=&MaterialSegment[index];
	memcpy(&ThisSegment->groupheader,&pnt[ReadBytes],4);
	toDWORD((u32&)ThisSegment->groupheader);
	ReadBytes+=4;
	ReadBytes+=ReadTextureInfo(&ThisSegment->info,&pnt[ReadBytes]);
	if(fVersion==MP1){	
		memcpy(&ThisSegment->unknown0,&pnt[ReadBytes],4);
		toDWORD((u32&)ThisSegment->unknown0);
		ReadBytes+=4;
		memcpy(&ThisSegment->unknown1,&pnt[ReadBytes],4);
		toDWORD((u32&)ThisSegment->unknown1);
		ReadBytes+=4;
		/*								if ((unknown0x00 & 0xF) == 0xB)
		{

		}  leaving this segment from the bt out and just adding the sizes together to the current function
		you can implement if you want
		*/

		if ((ThisSegment->groupheader & 0x4000) != 0x0000)
			{
			
				memcpy(&ThisSegment->unknown0a[0],&pnt[ReadBytes],4);
		        toDWORD((u32&)ThisSegment->unknown1);
				ReadBytes+=4;
				memcpy(&ThisSegment->unknown0a[1],&pnt[ReadBytes],4);
		       toDWORD((u32&)ThisSegment->unknown1);
				ReadBytes+=4;
			}
		if ((ThisSegment->groupheader & 0xD00) == 0xD00)
			{
			
				memcpy(&ThisSegment->unknown1a,&pnt[ReadBytes],4);
		        toDWORD((u32&)ThisSegment->unknown1);
				ReadBytes+=4;
			}
								// unsure, this is just a hack for now
								// need to determine if this is present under any other circumstances

		if ((ThisSegment->groupheader & 0x08) != 0x00){

			memcpy(&ThisSegment->unknownCount0,&pnt[ReadBytes],4);
			toDWORD((u32&)ThisSegment->unknownCount0);
			ThisSegment->unknownData0.resize(ThisSegment->unknownCount0);
			ReadBytes+=4;
			for(i=0;i<ThisSegment->unknownCount0;i++){
				memcpy(&ThisSegment->unknownData0[i],&pnt[ReadBytes],4);
				toDWORD((u32&)ThisSegment->unknownData0[i]);
				ReadBytes+=4;
			}
		}
		for(i=0;i<3;i++){
			memcpy(&ThisSegment->unknown2[i],&pnt[ReadBytes],4);
			toDWORD((u32&)ThisSegment->unknown2[i]);
			ReadBytes+=4;

		}
		//Read Attribute info
		memcpy(&ThisSegment->AttribCount,&pnt[ReadBytes],4);
		toDWORD((u32&)ThisSegment->AttribCount);
		ReadBytes+=4;
		ThisSegment->Attributes.resize(ThisSegment->AttribCount);
		if(ThisSegment->AttribCount<10)ThisSegment->unknown4.resize(ThisSegment->AttribCount);
		for(i=0;i<ThisSegment->AttribCount;i++){
			memcpy(&ThisSegment->Attributes[i],&pnt[ReadBytes],20);//Struct size is 20 bytes
			//u8  Unknown0;
			//u8  Blend;
			//u8  Unknown1;
			//u8  Unknown2;
			toWORD((u16&)ThisSegment->Attributes[i].BlendNumber);
			toWORD((u16&)ThisSegment->Attributes[i].BlendType);
			ReadBytes+=20;


		}
		for(i=0;i<ThisSegment->AttribCount;i++){
			memcpy(&ThisSegment->unknown4[i],&pnt[ReadBytes],4);
			ReadBytes+=4;

		}
		memcpy(&ThisSegment->LayerInfo,&pnt[ReadBytes],4);
		ReadBytes+=4;
		toDWORD((u32&)ThisSegment->LayerInfo);
		ThisSegment->LayerData.resize(ThisSegment->LayerInfo);
		for(i=0;i<ThisSegment->LayerInfo;i++){
			memcpy(&ThisSegment->LayerData[i],&pnt[ReadBytes],4);
			toDWORD((u32&)ThisSegment->LayerData[i]);
			ReadBytes+=4;


		}
		memcpy(&ThisSegment->unknownsize,&pnt[ReadBytes],4);
		toDWORD((u32&)ThisSegment->unknownsize);
		ReadBytes+=4;
		memcpy(&ThisSegment->unknown5,&pnt[ReadBytes],4);
		toDWORD((u32&)ThisSegment->unknown5);
		ReadBytes+=4;
		memcpy(&ThisSegment->unknown6,&pnt[ReadBytes],4);
		toDWORD((u32&)ThisSegment->unknown6);

	}
	return;
}
