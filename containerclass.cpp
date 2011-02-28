/* open file;
read header;
=new int[MaterialHeaderCount];
= new int[SegmentCount-MaterialHeaderCount];
read unknown sizes;
read segment sizes;
round the file pointer to the nearest 0x20 and add the sum of all the unknown sizes;
//now you're at the vertex data =o and the rest is easy :)
*/


#include "main.h"

cmdlContainment::cmdlContainment(){
	candraw=0;
	TextNo = 0;
	vertexcount = 0;
	uvcount = 0;
	normalcount = 0;

}
void cmdlContainment::DeleteMe(){
	candraw=0;
	TextNo = 0;
	vertexcount = 0;
	uvcount = 0;
	normalcount = 0;
	//Segment.clear();
	/*if(Segment!=0){
	delete[] Segment;
	}*/
	SegmentSizes.clear();
	//	delete[] TextureData;
	MaterialHeaderSizes.clear();
	vertexes.clear();
	mNormals.clear();
	UVs.clear();
	Meshes.clear();
	MaterialHeader.clear();
}
cmdlContainment::~cmdlContainment(){
	candraw=0;
	TextNo = 0;
	vertexcount = 0;
	uvcount = 0;
	normalcount = 0;
	
}
int cmdlContainment::LoadMaterialList(){
	char buff[100]={0};
	u32 i =0;
	lstMaterials.Clear();
	for(i=0;i<Header.MaterialHeaderCount ;i++){
		sprintf(buff,"%d",i);
		lstMaterials.Additem(buff);
	}
	//Set the index to 0
	lstMaterials.SetListIndex(0);
	return 0;
}
int cmdlContainment::Read(char* fstr){
	int currentoffset=0;
	int filesize=0;
	char errnostr[256]={0};
	FILE* fp=NULL;
	fp=fopen(fstr,"rb");
	if(fp){
		fseek(fp,0,SEEK_SET);//Make sure we're at the beginning of the CMDL
		ReadHeader(fp);
		if(Header.Unknown[0]==5) fVersion=MP3;
		currentoffset=ftell(fp);

		//Mine does the same and quicker lol  currentoffset += ((0x20 - (currentoffset % 0x20)) % 0x20);
		while(currentoffset % 0x20) currentoffset++;

		HEADERSIZE=currentoffset;
		fseek(fp,currentoffset,SEEK_SET);
		MaterialHeader.resize(Header.MaterialHeaderCount);
		u32 i = 0;
		for (i = 0; i < Header.MaterialHeaderCount; ++i)
		{
			if(fVersion!=MP3){
				MaterialHeader[i].ReadMaterialDataMP1_2(fp, i, CMDL.MaterialHeaderSizes);
			}else{//Read MP3 Style
                MaterialHeader[i].ReadMaterialDataMP3(fp, i, CMDL.MaterialHeaderSizes);
			}
		}
		InitSegment();
		ReadSegments(fp);
		fclose(fp);
		LoadMaterialList();
		CMDL.ReadTextures();

		//	 vertexcount=CMDL.SegmentSizes[0]/(12);//Guessing :/
		//	vertexes=new tex[vertexcount];
		//	fseek(fp,currentoffset+CMDL.UnknownSizes[0],SEEK_SET);
		//	ReadModel(fp);
		/*normalcount=CMDL.SegmentSizes[1]/12;
		mNormals= new tex[normalcount];
		uvcount=CMDL.SegmentSizes[2]/8;//4 bytes for each one
		UVs=new suvMap[uvcount];


		currentoffset=ftell(fp);
		ReadModel(fp);
		currentoffset=ftell(fp);
		ReadNormals(fp);
		currentoffset=ftell(fp);
		ReadUVs(fp);

		fclose(fp);
		//Dump Header to a file
		/*
		int i=0;
		string newfile=fstr;
		newfile+=".txt";
		fp=fopen(newfile.c_str(),"w");
		char buff[1024]={0};
		sprintf(buff,"DEADBABE: %X\r\n",Header.DeadBabe);
		fputs(buff,fp);
		for(i=0;i<2;i++) {

		sprintf(buff,"Unknown%d:%X\r\n",i,Header.Unknown[i]);
		fputs(buff,fp);
		}

		sprintf(buff,"MinX: %f\r\n",Header.MinX);
		fputs(buff,fp);

		sprintf(buff,"MaxX: %f\r\n",Header.MaxX);
		fputs(buff,fp);

		sprintf(buff,"MinY: %f\r\n",Header.MinY);
		fputs(buff,fp);

		sprintf(buff,"MaxY: %f\r\n",Header.MaxY);
		fputs(buff,fp);

		sprintf(buff,"MinZ: %f\r\n",Header.MinZ);
		fputs(buff,fp);

		sprintf(buff,"MinZ: %f\r\n",Header.MaxZ);
		fputs(buff,fp);


		sprintf(buff,"Segment Count: %X\r\n",Header.SegmentCount);
		fputs(buff,fp);

		sprintf(buff,"MaterialHeaderCount: %X\r\n",Header.MaterialHeaderCount);
		fputs(buff,fp);

		for(i=0;i<Header.MaterialHeaderCount;i++){

		sprintf(buff,"UnknownSizes[%d]: %X\r\n",i, UnknownSizes[i]);
		fputs(buff,fp);
		}
		for(i=0;i<Header.SegmentCount-Header.MaterialHeaderCount;i++){

		sprintf(buff,"SegmentSizes[%d]: %X\r\n",i, SegmentSizes[i]);
		fputs(buff,fp);

		}
		fclose(fp);
		//Do a quick file dump to read the vertexes
		newfile=fstr;
		newfile+="VERTICES.bin";
		fp=fopen(newfile.c_str(),"wb");
		for(i=0;i<CMDL.vertexcount;i++){
		fwrite(&CMDL.vertexes[i].X, 4 ,1 ,fp); 
		fwrite(&CMDL.vertexes[i].Y, 4 ,1 ,fp); 
		fwrite(&CMDL.vertexes[i].Z, 4 ,1 ,fp); 
		}
		fclose(fp);*/
	}
	return 0;
}
long GetLong(void* thisarray){
	long mylong=0;
	memcpy(&mylong,thisarray,4);
	toDWORD((u32&)mylong);

	return  mylong;
}
int cmdlContainment::ReadTextures(){
	int i =0;
	char sprinter[1024]={0};
	FILE* test;
	string stest;
	//unsigned long TEXT;
	int TextureCount=CMDL.MaterialHeader[lstMaterials.GetListIndex()].TextureCount;
	//	delete[] TextureNames;
	TextureNames.resize(TextureCount);
	lstTextures.Clear();
	lstTextures.SetListIndex(0);
	for(i=0;i<TextureCount;i++){

		sprintf(sprinter,
			"1_%x.TXTR",
			MaterialHeader[lstMaterials.GetListIndex()].TextureNames[i]
		);
		stest=strCurDir;
		stest+="\\";
		stest+=sprinter;
		test=fopen(stest.c_str(),"r+b");
		if(test==NULL){//1_x didn't work so lets try 0_x
			sprintf(sprinter,
				"0_%x.TXTR",
				MaterialHeader[lstMaterials.GetListIndex()].TextureNames[i]
			);
			stest=strCurDir;
			stest+="\\";
			stest+=sprinter;
			test=fopen(stest.c_str(),"r+b");
			int shit = GetLastError();
			if(test==NULL){

				MessageBox(0,"Some reason a texture that was referenced doesn't exist, please inform interdpth or revl8n on the model you are trying to load.","ERROR",MB_OK);
				sprintf(sprinter,"INVALID:%s",sprinter);
				goto nextloop;

			}
		}
nextloop:
		fclose(test);
		TextureNames[i]=sprinter;
	}
	for(i=0;i<TextureCount;i++) lstTextures.Additem( TextureNames[i].c_str());
	//Call the two texture functions. 
	//fclose(stest);

	LoadTextures();
	return TextureCount;
}
void cmdlContainment::InitSegment(){
	u32 i=0;


	//Segment.resize(Header.SegmentCount);
	Segment.resize(Header.SegmentCount);
	for(i=0;i<Header.SegmentCount;i++){
		if(SegmentSizes[i]!=0){
			Segment[i].resize(SegmentSizes[i]);
			fill(Segment[i].begin(),Segment[i].end(),0);
		}else{
			Segment[i].clear();
		}
	}

}
void cmdlContainment::ReadSegments(FILE* fp){
	//FILE* fp2;
	u32 i = 0;
	//char  fn[1024]={0};
	//i=ftell(fp);


	for(i=0;i<Header.SegmentCount;i++){
		int nextoffset=ftell(fp) + SegmentSizes[i];

		if(SegmentSizes[i]!=0){
			/* sprintf(fn,"C:\\Users\\Matt\\Desktop\\MetroidPrime1Dir\\Metroid1.pak_dir\\ef18372\\Segment%X.bin",i);
			fp2=fopen(fn,"w+b");
			fwrite(&Segment[i][0],SegmentSizes[i],1,fp2);
			fclose(fp2);*/

			switch (i)
			{

			case 0:
				{
					vertexcount = SegmentSizes[i] / 0xC;
					if (vertexcount > 0)
					{
						vertexes.resize(vertexcount);
						ReadModel(fp, i);
					}
				} break;
			case 1:
				{
					if (Header.Unknown[1] == 0 ||
						Header.Unknown[1] == 5)
					{
						normalcount = SegmentSizes[i] / 0xC;
					}
					else if (Header.Unknown[1] == 6)
					{
						normalcount = SegmentSizes[i] / 0x6;
					}
					if (normalcount > 0)
					{
						mNormals.resize(normalcount);
						ReadNormals(fp, i);
					}
				} break;
			case 2:
				{
					fread(&Segment[i].front(),SegmentSizes[i],1,fp);
				}
				break;
			case 3:
				{
					uvcount = SegmentSizes[i] / 0x8;
					if (uvcount > 0)
					{
						UVs.resize(uvcount);
						ReadUVs(fp, i);
					}
				} break;
			case 4:
				{
					fread(&Segment[i].front(),SegmentSizes[i],1,fp);
				}
				break;
			case 5:
				{
					if (0 == Header.Unknown[1])
					{
						// process index information
						ReadMeshes(fp, i);
					}
					else
					{
						fread(&Segment[i].front(),SegmentSizes[i],1,fp);
					}
				}
				break;

			default:
				{
					if (i > 5)
					{
						// process index information
						ReadMeshes(fp, i);
					}
					else
					{
						fread(&Segment[i].front(),SegmentSizes[i],1,fp);
					}
				}
			}
		}
		fseek(fp,nextoffset,SEEK_SET);
	}
}
int cmdlContainment::ReadNormals(FILE* fp, int segmentNum){
	u32 i=0;
	for(i=0;i<normalcount;i++){
		if (Header.Unknown[1] == 0 ||
			Header.Unknown[1] == 5)
		{
			fread(&mNormals[i].X,sizeof(float),1,fp);
			fread(&mNormals[i].Y,sizeof(float),1,fp);
			fread(&mNormals[i].Z,sizeof(float),1,fp);
			toDWORD((u32&)mNormals[i].X);
			toDWORD((u32&)mNormals[i].Y);
			toDWORD((u32&)mNormals[i].Z);
		}
		else if (Header.Unknown[1] == 6)
		{
			signed short x, y, z;
			fread(&x,sizeof(signed short),1,fp);
			fread(&y,sizeof(signed short),1,fp);
			fread(&z,sizeof(signed short),1,fp);
			toWORD((u16&)x);
			toWORD((u16&)y);
			toWORD((u16&)z);

			mNormals[i].X = float(x) / 16834.0f;
			mNormals[i].Y = float(y) / 16834.0f;
			mNormals[i].Z = float(z) / 16834.0f;
		}
	}
	return 0;
}
int cmdlContainment::ReadModel(FILE* fp, int segmentNum){
	u32 i=0;
	for(i=0;i<vertexcount;i++){
		fread(&vertexes[i].X,sizeof(float),1,fp);
		fread(&vertexes[i].Y,sizeof(float),1,fp);
		fread(&vertexes[i].Z,sizeof(float),1,fp);
		toDWORD((u32&)vertexes[i].X);
		toDWORD((u32&)vertexes[i].Y);
		toDWORD((u32&)vertexes[i].Z);
	}


	return 0;
}
int cmdlContainment::ReadUVs(FILE* fp, int segmentNum){
	float minU = 9999999.0f, maxU = -9999999.0f;
	float minV = 9999999.0f, maxV = -9999999.0f;
	u32 i=0;
	for(i=0;i< uvcount;i++){
		fread(&UVs[i].U,sizeof(float),1,fp);
		fread(&UVs[i].V,sizeof(float),1,fp);

		toDWORD((u32&)UVs[i].U);
		toDWORD((u32&)UVs[i].V);

		if (minU > UVs[i].U)
		{
			minU = UVs[i].U;
		}
		if (maxU < UVs[i].U)
		{
			maxU = UVs[i].U;
		}
		if (minV > UVs[i].V)
		{
			minV = UVs[i].V;
		}
		if (maxV < UVs[i].V)
		{
			maxV = UVs[i].V;
		}
	}

	char buff[256] = {0};
	sprintf(buff, "Min U: %4f - Max U: %4f - Min V: %4f - Max V: %4f\n", minU, maxU, minV, maxV);
	OutputDebugStr(buff);

	return 0;
}/*
 int cmdlContainment::ReadMeshes(FILE* fp, int segmentNum)
 {
 int dataFormat = 0;
 int startPos = ftell(fp);

 fseek(fp,startPos + 0x12,SEEK_SET);

 u16 dataSize = 0;
 fread(&dataSize,sizeof(u16),1,fp);
 toWORD((u16&)dataSize);

 fseek(fp,startPos + 0x40,SEEK_SET);

 uint32 readBytes = 0;

 uint32 meshIndex = 0;
 while (readBytes < dataSize)
 {
 meshIndex = Meshes.size();

 Meshes.push_back(struct_Mesh());

 struct_Mesh & mesh = Meshes.back();

 fread(&(mesh.primitiveFlags), sizeof(unsigned char), 1, fp);
 readBytes += sizeof(unsigned char);

 mesh.indexCount = 0;

 if (mesh.primitiveFlags == 0 ||
 mesh.primitiveFlags < 0x80 ||
 mesh.primitiveFlags > 0xBF ||
 readBytes >= dataSize)
 {
 mesh.primitiveFlags = 0;
 break;
 }

 fread(&(mesh.indexCount), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 toWORD((u16&)mesh.indexCount);

 mesh.indices.resize(mesh.indexCount);

 struct_Faces * pFaces = &mesh.indices.front();

 uint32 startReadBytes = readBytes;
 int currentPos = ftell(fp);

 bool bValid = ((0 == dataFormat) || (3 == dataFormat));

 int i=0;
 if (bValid || (3 == dataFormat))//Default?
 {
 dataFormat = 3;

 for(i=0; (readBytes < dataSize) && (i < mesh.indexCount); i++)
 {
 fread(&(pFaces[i].vertexIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].normalIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);

 toWORD((u16&)pFaces[i].vertexIndex);
 toWORD((u16&)pFaces[i].normalIndex);

 if (pFaces[i].vertexIndex >= vertexcount ||
 readBytes > dataSize)
 {
 bValid = false;
 break;
 }
 }

 unsigned char peekByte = 0;
 unsigned short peekShort = 0;
 fread(&(peekByte), sizeof(unsigned char), 1, fp);
 fread(&(peekShort), sizeof(unsigned short), 1, fp);
 fseek(fp, ftell(fp) - 3, SEEK_SET);

 if ((readBytes + 3) < dataSize && ((peekByte == 0 && peekShort != 0) ||
 (peekByte != 0 && (peekByte < 0x80 ||
 peekByte > 0xBF || peekShort == 0))))
 {
 bValid = false;
 }

 if (!bValid)
 {
 dataFormat = 0;
 }
 }

 if (!bValid || (1 == dataFormat))
 {
 dataFormat = 1;

 bValid = true;
 fseek(fp,currentPos,SEEK_SET);
 readBytes = startReadBytes;

 for (i=0; (readBytes < dataSize) && (i < mesh.indexCount); i++)
 {
 fread(&(pFaces[i].vertexIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].normalIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[0]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);

 toWORD((u16&)pFaces[i].vertexIndex);
 toWORD((u16&)pFaces[i].normalIndex);
 toWORD((u16&)pFaces[i].uvIndex[0]);

 if (pFaces[i].vertexIndex >= vertexcount ||
 readBytes > dataSize)
 {
 bValid = false;
 break;
 }
 }

 unsigned char peekByte = 0;
 unsigned short peekShort = 0;
 fread(&(peekByte), sizeof(unsigned char), 1, fp);
 fread(&(peekShort), sizeof(unsigned short), 1, fp);
 fseek(fp, ftell(fp) - 3, SEEK_SET);

 if ((readBytes + 3) < dataSize && ((peekByte == 0 && peekShort != 0) ||
 (peekByte != 0 && (peekByte < 0x80 ||
 peekByte > 0xBF || peekShort == 0))))
 {
 bValid = false;
 }

 if (!bValid)
 {
 dataFormat = 0;
 }
 }

 if (!bValid || (2 == dataFormat))
 {
 dataFormat = 2;

 bValid = true;
 fseek(fp,currentPos,SEEK_SET);
 readBytes = startReadBytes;

 for(i=0; (readBytes < dataSize) && (i < mesh.indexCount); i++)
 {
 fread(&(pFaces[i].vertexIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].normalIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[0]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[1]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);

 toWORD((u16&)pFaces[i].vertexIndex);
 toWORD((u16&)pFaces[i].normalIndex);
 toWORD((u16&)pFaces[i].uvIndex[0]);
 toWORD((u16&)pFaces[i].uvIndex[1]);

 if (pFaces[i].vertexIndex >= vertexcount ||
 readBytes > dataSize){
 bValid = false;
 break;
 }
 }

 unsigned char peekByte = 0;
 unsigned short peekShort = 0;
 fread(&(peekByte), sizeof(unsigned char), 1, fp);
 fread(&(peekShort), sizeof(unsigned short), 1, fp);
 fseek(fp, ftell(fp) - 3, SEEK_SET);

 if ((readBytes + 3) < dataSize && ((peekByte == 0 && peekShort != 0) ||
 (peekByte != 0 && (peekByte < 0x80 ||
 peekByte > 0xBF || peekShort == 0))))
 {
 bValid = false;
 }

 if (!bValid)
 {
 dataFormat = 0;
 }
 }

 if (!bValid || (4 == dataFormat))
 {
 dataFormat = 4;

 bValid = true;
 fseek(fp,currentPos,SEEK_SET);
 readBytes = startReadBytes;

 for(i=0; (readBytes < dataSize) && (i < mesh.indexCount); i++)
 {
 fread(&(pFaces[i].vertexIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].normalIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[0]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[1]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[2]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);

 toWORD((u16&)pFaces[i].vertexIndex);
 toWORD((u16&)pFaces[i].normalIndex);
 toWORD((u16&)pFaces[i].uvIndex[0]);
 toWORD((u16&)pFaces[i].uvIndex[1]);
 toWORD((u16&)pFaces[i].uvIndex[2]);

 if (pFaces[i].vertexIndex >= vertexcount ||
 readBytes > dataSize)
 {
 bValid = false;
 break;
 }
 }

 unsigned char peekByte = 0;
 unsigned short peekShort = 0;
 fread(&(peekByte), sizeof(unsigned char), 1, fp);
 fread(&(peekShort), sizeof(unsigned short), 1, fp);
 fseek(fp, ftell(fp) - 3, SEEK_SET);

 if ((readBytes + 3) < dataSize && ((peekByte == 0 && peekShort != 0) ||
 (peekByte != 0 && (peekByte < 0x80 ||
 peekByte > 0xBF || peekShort == 0))))
 {
 bValid = false;
 }

 if (!bValid)
 {
 dataFormat = 0;
 }
 }

 if (!bValid || (5 == dataFormat))
 {
 dataFormat = 5;

 bValid = true;
 fseek(fp,currentPos,SEEK_SET);
 readBytes = startReadBytes;

 for(i=0; (readBytes < dataSize) && (i < mesh.indexCount); i++)
 {
 fread(&(pFaces[i].vertexIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].normalIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[0]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[1]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[2]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[3]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);

 toWORD((u16&)pFaces[i].vertexIndex);
 toWORD((u16&)pFaces[i].normalIndex);
 toWORD((u16&)pFaces[i].uvIndex[0]);
 toWORD((u16&)pFaces[i].uvIndex[1]);
 toWORD((u16&)pFaces[i].uvIndex[2]);
 toWORD((u16&)pFaces[i].uvIndex[3]);

 if (pFaces[i].vertexIndex >= vertexcount ||
 readBytes > dataSize)
 {
 bValid = false;
 break;
 }
 }

 unsigned char peekByte = 0;
 unsigned short peekShort = 0;
 fread(&(peekByte), sizeof(unsigned char), 1, fp);
 fread(&(peekShort), sizeof(unsigned short), 1, fp);
 fseek(fp, ftell(fp) - 3, SEEK_SET);

 if ((readBytes + 3) < dataSize && ((peekByte == 0 && peekShort != 0) ||
 (peekByte != 0 && (peekByte < 0x80 ||
 peekByte > 0xBF || peekShort == 0))))
 {
 bValid = false;
 }

 if (!bValid)
 {
 dataFormat = 0;
 }
 }

 if (!bValid || (6 == dataFormat))
 {
 dataFormat = 6;

 bValid = true;
 fseek(fp,currentPos,SEEK_SET);
 readBytes = startReadBytes;

 for(i=0; (readBytes < dataSize) && (i < mesh.indexCount); i++)
 {
 fread(&(pFaces[i].vertexIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].normalIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[0]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[1]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[2]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[3]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[4]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);

 toWORD((u16&)pFaces[i].vertexIndex);
 toWORD((u16&)pFaces[i].normalIndex);
 toWORD((u16&)pFaces[i].uvIndex[0]);
 toWORD((u16&)pFaces[i].uvIndex[1]);
 toWORD((u16&)pFaces[i].uvIndex[2]);
 toWORD((u16&)pFaces[i].uvIndex[3]);
 toWORD((u16&)pFaces[i].uvIndex[4]);

 if (pFaces[i].vertexIndex >= vertexcount ||
 readBytes > dataSize)
 {
 bValid = false;
 break;
 }
 }

 unsigned char peekByte = 0;
 unsigned short peekShort = 0;
 fread(&(peekByte), sizeof(unsigned char), 1, fp);
 fread(&(peekShort), sizeof(unsigned short), 1, fp);
 fseek(fp, ftell(fp) - 3, SEEK_SET);

 if ((readBytes + 3) < dataSize && ((peekByte == 0 && peekShort != 0) ||
 (peekByte != 0 && (peekByte < 0x80 ||
 peekByte > 0xBF || peekShort == 0))))
 {
 bValid = false;
 }

 if (!bValid)
 {
 dataFormat = 0;
 }
 }

 if (!bValid || (7 == dataFormat))
 {
 dataFormat = 7;

 bValid = true;
 fseek(fp,currentPos,SEEK_SET);
 readBytes = startReadBytes;

 for(i=0; (readBytes < dataSize) && (i < mesh.indexCount); i++)
 {
 fread(&(pFaces[i].vertexIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].normalIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[0]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[1]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[2]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[3]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[4]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[5]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);

 toWORD((u16&)pFaces[i].vertexIndex);
 toWORD((u16&)pFaces[i].normalIndex);
 toWORD((u16&)pFaces[i].uvIndex[0]);
 toWORD((u16&)pFaces[i].uvIndex[1]);
 toWORD((u16&)pFaces[i].uvIndex[2]);
 toWORD((u16&)pFaces[i].uvIndex[3]);
 toWORD((u16&)pFaces[i].uvIndex[4]);
 toWORD((u16&)pFaces[i].uvIndex[5]);

 if (pFaces[i].vertexIndex >= vertexcount ||
 readBytes > dataSize)
 {
 bValid = false;
 break;
 }
 }

 unsigned char peekByte = 0;
 unsigned short peekShort = 0;
 fread(&(peekByte), sizeof(unsigned char), 1, fp);
 fread(&(peekShort), sizeof(unsigned short), 1, fp);
 fseek(fp, ftell(fp) - 3, SEEK_SET);

 if ((readBytes + 3) < dataSize && ((peekByte == 0 && peekShort != 0) ||
 (peekByte != 0 && (peekByte < 0x80 ||
 peekByte > 0xBF || peekShort == 0))))
 {
 bValid = false;
 }

 if (!bValid)
 {
 dataFormat = 0;
 }
 }

 if (!bValid || (8 == dataFormat))
 {
 dataFormat = 8;

 bValid = true;
 fseek(fp,currentPos,SEEK_SET);
 readBytes = startReadBytes;

 for(i=0; (readBytes < dataSize) && (i < mesh.indexCount); i++)
 {
 fread(&(pFaces[i].vertexIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].normalIndex), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[0]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[1]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[2]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[3]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[4]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[5]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);
 fread(&(pFaces[i].uvIndex[6]), sizeof(unsigned short), 1, fp);
 readBytes += sizeof(unsigned short);

 toWORD((u16&)pFaces[i].vertexIndex);
 toWORD((u16&)pFaces[i].normalIndex);
 toWORD((u16&)pFaces[i].uvIndex[0]);
 toWORD((u16&)pFaces[i].uvIndex[1]);
 toWORD((u16&)pFaces[i].uvIndex[2]);
 toWORD((u16&)pFaces[i].uvIndex[3]);
 toWORD((u16&)pFaces[i].uvIndex[4]);
 toWORD((u16&)pFaces[i].uvIndex[5]);
 toWORD((u16&)pFaces[i].uvIndex[6]);

 if (pFaces[i].vertexIndex >= vertexcount ||
 readBytes > dataSize)
 {
 bValid = false;
 break;
 }
 }

 unsigned char peekByte = 0;
 unsigned short peekShort = 0;
 fread(&(peekByte), sizeof(unsigned char), 1, fp);
 fread(&(peekShort), sizeof(unsigned short), 1, fp);
 fseek(fp, ftell(fp) - 3, SEEK_SET);

 if ((readBytes + 3) < dataSize && ((peekByte == 0 && peekShort != 0) ||
 (peekByte != 0 && (peekByte < 0x80 ||
 peekByte > 0xBF || peekShort == 0))))
 {
 bValid = false;
 }

 if (!bValid)
 {
 dataFormat = 0;
 }
 }

 if (!bValid)
 {
 dataFormat = 0;
 readBytes = dataSize;
 mesh.primitiveFlags = 0;
 mesh.indexCount = 0;
 break;
 }
 }

 fseek(fp, startPos + dataSize, SEEK_SET);

 return 0;
 }*/
int cmdlContainment::ReadHeader(FILE* fp){
	u32 i=0;

	fread(&Header.DeadBabe,4,1,fp);
	toDWORD(Header.DeadBabe);
	if(Header.DeadBabe!=0xDEADBABE){
		MessageBox(0,"HOUSTON DEADBABE IS SRSLY DEAD","NONE",MB_OK);
	}
	for(i=0;i<2;i++) {
		fread(&Header.Unknown[i],4,1,fp);
		toDWORD(Header.Unknown[i]);
	}
	// min bounds
	fread(&Header.MinX,sizeof(float),1,fp);
	fread(&Header.MinY,sizeof(float),1,fp);
	fread(&Header.MinZ,sizeof(float),1,fp);
	// max bounds
	fread(&Header.MaxX,sizeof(float),1,fp);
	fread(&Header.MaxY,sizeof(float),1,fp);
	fread(&Header.MaxZ,sizeof(float),1,fp);
	// segment counts
	fread(&Header.SegmentCount,sizeof(uint32),1,fp);
	fread(&Header.MaterialHeaderCount,sizeof(uint32),1,fp);

	toDWORD((u32&)Header.MinX);
	toDWORD((u32&)Header.MinY);
	toDWORD((u32&)Header.MinZ);
	toDWORD((u32&)Header.MaxX);
	toDWORD((u32&)Header.MaxY);
	toDWORD((u32&)Header.MaxZ);

	toDWORD((u32&)Header.SegmentCount);
	toDWORD((u32&)Header.MaterialHeaderCount);

	Header.SegmentCount -= Header.MaterialHeaderCount;

	SegmentSizes.resize(Header.SegmentCount);
	MaterialHeaderSizes.resize(Header.MaterialHeaderCount);

	for(i=0;i<Header.MaterialHeaderCount;i++){
		fread(&MaterialHeaderSizes[i],4,1,fp);
		toDWORD((u32&)MaterialHeaderSizes[i]);
	}
	for(i=0;i<Header.SegmentCount;i++){
		fread(&SegmentSizes[i],4,1,fp);
		toDWORD((u32&)SegmentSizes[i]);
	}

	candraw=1;
	return 0;
}
void ReadFace(FILE* fp, struct_Faces * Face, int format){
	int i=0;

	fread(&(Face->vertexIndex), sizeof(unsigned short), 1, fp);
	fread(&(Face->normalIndex), sizeof(unsigned short), 1, fp);
	toWORD((u16&)Face->vertexIndex);
	toWORD((u16&)Face->normalIndex);

	for(i=0;i<format-2;i++){
		fread(     &(Face->uvIndex[i]), sizeof(unsigned short), 1, fp);
		toWORD((u16&)Face->uvIndex[i]);
	}

}
int cmdlContainment::ReadMeshes(FILE* fp, int segmentNum)//Adjusted o
{
	unsigned int maxVert = 0;
	unsigned int maxNorm = 0;
	unsigned int maxUV = 0;


	int startPos = ftell(fp);
	long GroupId=0;
	fseek(fp,startPos + 0xC,SEEK_SET);
	fread(&GroupId,1,4,fp);
	toDWORD((u32&)GroupId);
	int dataFormat =0;// MaterialHeader[0].MaterialSegment[GroupId].info.count-1;
	fseek(fp,startPos + 0x12,SEEK_SET);

	u16 dataSize = 0;
	fread(&dataSize,sizeof(u16),1,fp);
	toWORD((u16&)dataSize);

	fseek(fp,startPos + 0x40,SEEK_SET);

	Meshes.push_back(struct_Mesh());
	
	struct_Mesh & mesh = Meshes.back();
	mesh.GroupId = GroupId;
	mesh.format = 0;

	uint32 readBytes = 0;
	while (readBytes < dataSize)
	{
		mesh.subMeshes.push_back(struct_SubMesh());

		struct_SubMesh & submesh = mesh.subMeshes.back();

		fread(&(submesh.primitiveFlags), sizeof(unsigned char), 1, fp);
		readBytes += sizeof(unsigned char);

		submesh.indexCount = 0;

		if (submesh.primitiveFlags == 0 ||
			submesh.primitiveFlags < 0x80 ||
			submesh.primitiveFlags > 0xBF ||
			(readBytes + 2) >= dataSize)
		{
			submesh.primitiveFlags = 0;
			break;
		}

		fread(&(submesh.indexCount), sizeof(unsigned short), 1, fp);
		readBytes += sizeof(unsigned short);
		toWORD((u16&)submesh.indexCount);

		if (submesh.indexCount <= 0 ||
			readBytes >= dataSize)
		{
			submesh.primitiveFlags = 0;
			break;
		}

		submesh.indices.resize(submesh.indexCount);

		struct_Faces * pFaces = &submesh.indices.front();

		uint32 startReadBytes = readBytes;
		int currentPos = ftell(fp);

		bool bValid = false; //((0 == dataFormat) || (3 == dataFormat));

		int i=0;
		while (!bValid && (struct_Faces::MAX_UV_INDICES >= dataFormat))
		{
			fseek(fp, currentPos, SEEK_SET);
			readBytes = startReadBytes;

			bValid = true;

			for (i=0; (readBytes < dataSize) && (i < submesh.indexCount); i++)
			{
				fread(&(pFaces[i].vertexIndex), sizeof(unsigned short), 1, fp);
				readBytes += sizeof(unsigned short);
				fread(&(pFaces[i].normalIndex), sizeof(unsigned short), 1, fp);
				readBytes += sizeof(unsigned short);

				toWORD((u16&)pFaces[i].vertexIndex);
				toWORD((u16&)pFaces[i].normalIndex);

				if (maxVert < pFaces[i].vertexIndex)
				{
					maxVert = pFaces[i].vertexIndex;
				}
				if (maxNorm < pFaces[i].normalIndex)
				{
					maxNorm = pFaces[i].normalIndex;
				}

				for (int iUV = 0; iUV < dataFormat; ++iUV)
				{
					fread(&(pFaces[i].uvIndex[iUV]), sizeof(unsigned short), 1, fp);
					readBytes += sizeof(unsigned short);

					toWORD((u16&)pFaces[i].uvIndex[iUV]);

					if (maxUV < pFaces[i].uvIndex[iUV])
					{
						maxUV = pFaces[i].uvIndex[iUV];
					}
				}

				if (pFaces[i].vertexIndex >= vertexcount ||
					pFaces[i].normalIndex >= normalcount ||
					readBytes > dataSize
					)
				{
					bValid = false;
					break;
				}
			}

			if (bValid)
			{
				unsigned char peekByte = 0;
				unsigned short peekShort = 0;
				fread(&(peekByte), sizeof(unsigned char), 1, fp);
				fread(&(peekShort), sizeof(unsigned short), 1, fp);
				fseek(fp, ftell(fp) - 3, SEEK_SET);

				if (((readBytes + 3) < dataSize && ((peekByte == 0 && peekShort != 0) ||
					(peekByte != 0 && (peekByte < 0x80 ||
					peekByte > 0xBF || peekShort == 0)))) ||
					(readBytes < (dataSize - 32) && peekByte == 0 && peekShort == 0))
				{
					bValid = false;
				}
			}

			if (!bValid)
			{
				++dataFormat;
				maxVert = 0;
				maxNorm = 0;
				maxUV = 0;
			}
		}

		if (!bValid)
		{
			dataFormat = 0;
			readBytes = dataSize;
			submesh.primitiveFlags = 0;
			submesh.indexCount = 0;
			break;
		}

		mesh.format = (dataFormat & 0x7FFF);
	}

	fseek(fp, startPos + dataSize, SEEK_SET);

	char buff[256] = {0};
	sprintf(buff, "Max Vertex Index: %4d - Max Normal Index: %4d - Max UV Index: %4d\n", maxVert, maxNorm, maxUV);
	OutputDebugStr(buff);

	return 0;
}



long returnfilesize(FILE* fp){
	long size=0;
	GLfloat xnear = 0.3f;
	long curroffset=ftell(fp);
	fseek(fp,0,SEEK_END);
	size=ftell(fp);
	fseek(fp,curroffset,SEEK_SET);

	return size;
}


int cmdlContainment::LoadTextures(){
	/*
	Get texture decompress 
	pass to loadtxtr
	delete files. 
	*/

	u32 i=0;
	char Dir[2048]={0};
	char buff[2048]={0};
//	TxtrHeader txtr;

	string CurTexture;
	GetWindowText(GetDlgItem(hwndMain,ID_txtFolders),Dir,2048);

	for(i=0;i<MaterialHeader[lstMaterials.GetListIndex()].TextureCount;i++){
		CurTexture=Dir;
		CurTexture+="\\"+TextureNames[i];

		memset(buff,0,2048);
		if(TextureNames[i].c_str()[0] == '1'){
			decompFile(CurTexture.c_str(),buff);
		}else{
			sprintf(buff,"%s",CurTexture.c_str());
		}
		LoadTXTR(std::string(buff),&MaterialHeader[lstMaterials.GetListIndex()].TexturesIDs[i]);//Should be loaded?
		//DeleteFile(buff);
	}

	return 0;
}
