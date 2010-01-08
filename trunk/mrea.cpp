#include "mrea.h"
#include <climits>
#include <cfloat>

MREAData::MREAData()
{

}
MREAData::~MREAData()
{
	m_materialInfo.~clsMaterialHeader();
	m_sectionSizes.clear();

	for (u32 iMesh = 0; iMesh < m_meshes.size(); ++iMesh)
	{
		if (glIsList(m_meshes[iMesh].glList))
		{
			glDeleteLists(m_meshes[iMesh].glList, 1);
		}
	}
	m_meshes.clear();
	m_sectionIndex = 0;

	memset(&m_fileHeader, 0, sizeof(m_fileHeader));
}

int MREAData::Read(char* fstr)
{
	FILE* fp = NULL;
	fp=fopen(fstr, "r+b");

	// invalidate bounding box
	minBounds.X = FLT_MAX;
	minBounds.Y = FLT_MAX;
	minBounds.Z = FLT_MAX;

	maxBounds.X = FLT_MIN;
	maxBounds.Y = FLT_MIN;
	maxBounds.Z = FLT_MIN;

	m_sectionIndex = 0;

	if (fp)
	{
		fseek(fp, 0, SEEK_SET); //Make sure we're at the beginning of the MREA

		ReadHeader(fp);

		m_materialInfo.ReadMaterialDataMP1_2(fp, m_sectionIndex, m_sectionSizes);

		++m_sectionIndex;

		m_meshes.resize(m_fileHeader.meshCount);

		for (u32 iMesh = 0; iMesh < m_fileHeader.meshCount; ++iMesh)
		{
			m_sectionIndex = ReadMesh(fp, iMesh, m_sectionIndex);
		}

		fclose(fp);

		ReadTextures();

		center.X = (maxBounds.X + minBounds.X) * 0.5f;
		center.Y = (maxBounds.Y + minBounds.Y) * 0.5f;
		center.Z = (maxBounds.Z + minBounds.Z) * 0.5f;

		tex dir;
		dir.X = (maxBounds.X - center.X);
		dir.Y = (maxBounds.Y - center.Y);
		dir.Z = (maxBounds.Z - center.Z);

		float distSq = ((dir.X * dir.X) + (dir.Y * dir.Y) + (dir.Z * dir.Z));
		float dist = sqrtf(distSq);
		float distCu = distSq * dist;

		xCamera = center.X;
		yCamera = center.Y;
		zCamera = center.Z - (dist * 2.0f);
	}

	return m_sectionIndex;
}
bool MREAData::ReadHeader(FILE* fp)
{
	bool bResult = false;

	fread(&m_fileHeader.fileID, 4, 1, fp);
	toDWORD(m_fileHeader.fileID);
	if (m_fileHeader.fileID != 0xDEADBEEF)
	{
		MessageBox(0,"HOUSTON DEADBEEF IS SRSLY DEAD","NONE",MB_OK);
		return false;
	}

	fread(&m_fileHeader.fileVersion, 4, 1, fp);
	toDWORD(m_fileHeader.fileVersion);

	fread(m_fileHeader.transformMatrix, 4, 12, fp);
	for (u32 i = 0; i < 12; ++i)
	{
		toDWORD((u32&)m_fileHeader.transformMatrix[i]);
	}

	fread(&m_fileHeader.meshCount, 4, 1, fp);
	toDWORD(m_fileHeader.meshCount);

	fread(&m_fileHeader.sectionCount, 4, 1, fp);
	toDWORD(m_fileHeader.sectionCount);

	fread(m_fileHeader.unknown0x40, 4, 8, fp);
	for (u32 i = 0; i < 8; ++i)
	{
		toDWORD(m_fileHeader.unknown0x40[i]);
	}

	m_sectionSizes.resize(m_fileHeader.sectionCount);

	fread(&m_sectionSizes.front(), 4, m_fileHeader.sectionCount, fp);
	for (u32 i = 0; i < m_fileHeader.sectionCount; ++i)
	{
		toDWORD(m_sectionSizes[i]);
	}

	u32 currentOffset = ftell(fp);

	currentOffset = ((0x20 - (currentOffset & 0x1F)) & 0x1F);

	fseek(fp, currentOffset, SEEK_CUR);

	return bResult;
}
int MREAData::ReadMesh(FILE* fp, int index, int segmentNum)
{
	MeshData & mesh = m_meshes[index];

	mesh.glList = 0;

	u32 nextOffset = ftell(fp) + m_sectionSizes[segmentNum];

	fread(&mesh.header.unknown0x00, 4, 1, fp);
	toDWORD(mesh.header.unknown0x00);

	fread(mesh.header.transformMatrix, 4, 12, fp);
	for (u32 i = 0; i < 12; ++i)
	{
		toDWORD((u32&)mesh.header.transformMatrix[i]);
	}

	fread(mesh.header.boundingBox, 4, 6, fp);
	for (u32 i = 0; i < 6; ++i)
	{
		toDWORD((u32&)mesh.header.boundingBox[i]);
	}

	if (0 == index)
	{
		// minimum
		if (mesh.header.boundingBox[0] < minBounds.X)
		{
			minBounds.X = mesh.header.boundingBox[0];
		}
		if (mesh.header.boundingBox[1] < minBounds.Y)
		{
			minBounds.Y = mesh.header.boundingBox[1];
		}
		if (mesh.header.boundingBox[2] < minBounds.Z)
		{
			minBounds.Z = mesh.header.boundingBox[2];
		}

		// maximum bounds
		if (mesh.header.boundingBox[3] > maxBounds.X)
		{
			maxBounds.X = mesh.header.boundingBox[3];
		}
		if (mesh.header.boundingBox[4] > maxBounds.Y)
		{
			maxBounds.Y = mesh.header.boundingBox[4];
		}
		if (mesh.header.boundingBox[5] > maxBounds.Z)
		{
			maxBounds.Z = mesh.header.boundingBox[5];
		}
	}

	// move to next section data
	fseek(fp, nextOffset, SEEK_SET);
	++segmentNum;
	nextOffset += m_sectionSizes[segmentNum];

	u32 dataCount = m_sectionSizes[segmentNum] / 0x0C;
	u32 dataNum = 0;

	mesh.vertices.resize(dataCount);
	for(dataNum=0; dataNum<dataCount; ++dataNum)
	{
		fread(&mesh.vertices[dataNum].X, sizeof(float), 1, fp);
		fread(&mesh.vertices[dataNum].Y, sizeof(float), 1, fp);
		fread(&mesh.vertices[dataNum].Z, sizeof(float), 1, fp);
		toDWORD((u32&)mesh.vertices[dataNum].X);
		toDWORD((u32&)mesh.vertices[dataNum].Y);
		toDWORD((u32&)mesh.vertices[dataNum].Z);

		const tex vert = mesh.vertices[dataNum];
		if (0) // == index)
		{
			// minimum bounds
			if (vert.X < minBounds.X)
			{
				minBounds.X = vert.X;
			}
			if (vert.Y < minBounds.Y)
			{
				minBounds.Y = vert.Y;
			}
			if (vert.Z < minBounds.Z)
			{
				minBounds.Z = vert.Z;
			}

			// maximum bounds
			if (vert.X > maxBounds.X)
			{
				maxBounds.X = vert.X;
			}
			if (vert.Y > maxBounds.Y)
			{
				maxBounds.Y = vert.Y;
			}
			if (vert.Z > maxBounds.Z)
			{
				maxBounds.Z = vert.Z;
			}
		}
	}

	// move to next section data
	fseek(fp, nextOffset, SEEK_SET);
	++segmentNum;
	nextOffset += m_sectionSizes[segmentNum];

	signed short x, y, z;
	dataCount = m_sectionSizes[segmentNum] / 0x06;
	mesh.normals.resize(dataCount);
	for(dataNum=0; dataNum<dataCount; ++dataNum)
	{
		fread(&x,sizeof(signed short),1,fp);
		fread(&y,sizeof(signed short),1,fp);
		fread(&z,sizeof(signed short),1,fp);
		toWORD((u16&)x);
		toWORD((u16&)y);
		toWORD((u16&)z);

		mesh.normals[dataNum].X = float(x) / 16834.0f;
		mesh.normals[dataNum].Y = float(y) / 16834.0f;
		mesh.normals[dataNum].Z = float(z) / 16834.0f;
	}

	// move to next section data
	fseek(fp, nextOffset, SEEK_SET);
	++segmentNum;
	nextOffset += m_sectionSizes[segmentNum];

	// read zero data

	// move to next section data
	fseek(fp, nextOffset, SEEK_SET);
	++segmentNum;
	nextOffset += m_sectionSizes[segmentNum];

	// read texture coordinate data
	dataCount = m_sectionSizes[segmentNum] / 0x08;
	mesh.uvs.resize(dataCount);
	for(dataNum=0; dataNum<dataCount; ++dataNum)
	{
		fread(&mesh.uvs[dataNum].U, sizeof(float), 1, fp);
		fread(&mesh.uvs[dataNum].V, sizeof(float), 1, fp);
		toDWORD((u32&)mesh.uvs[dataNum].U);
		toDWORD((u32&)mesh.uvs[dataNum].V);
	}

	// move to next section data
	fseek(fp, nextOffset, SEEK_SET);
	++segmentNum;
	nextOffset += m_sectionSizes[segmentNum];

	// read unknown data

	// move to next section data
	fseek(fp, nextOffset, SEEK_SET);
	++segmentNum;
	nextOffset += m_sectionSizes[segmentNum];

	// read sub mesh info data
	fread(&mesh.subMeshInfo.subMeshCount, 4, 1, fp);
	toDWORD(mesh.subMeshInfo.subMeshCount);

	mesh.subMeshInfo.subMeshOffsets.resize(mesh.subMeshInfo.subMeshCount);
	fread(&mesh.subMeshInfo.subMeshOffsets.front(), 4, mesh.subMeshInfo.subMeshCount, fp);
	for (u32 i = 0; i < mesh.subMeshInfo.subMeshCount; ++i)
	{
		toDWORD(mesh.subMeshInfo.subMeshOffsets[i]);
	}

	// move to next section data
	fseek(fp, nextOffset, SEEK_SET);
	++segmentNum;
	nextOffset += m_sectionSizes[segmentNum];

	// read sub mesh data
	for (u32 i = 0; i < mesh.subMeshInfo.subMeshCount; ++i)
	{
		ReadSubMesh(fp, mesh);

		// move to next section data
		fseek(fp, nextOffset, SEEK_SET);
		++segmentNum;
		nextOffset += m_sectionSizes[segmentNum];
	}

	return segmentNum;
}
int MREAData::ReadSubMesh(FILE* fp, MeshData & meshData)
{
	unsigned int maxVert = 0;
	unsigned int maxNorm = 0;
	unsigned int maxUV = 0;


	int startPos = ftell(fp);
	long GroupId = 0;
	fseek(fp,startPos + 0xC,SEEK_SET);
	fread(&GroupId,1,4,fp);
	toDWORD((u32&)GroupId);
	int dataFormat = 0;// MaterialHeader[0].MaterialSegment[GroupId].info.count-1;
	fseek(fp,startPos + 0x12,SEEK_SET);

	u16 dataSize = 0;
	fread(&dataSize,sizeof(u16),1,fp);
	toWORD((u16&)dataSize);

	fseek(fp,startPos + 0x60,SEEK_SET);

	meshData.subMeshes.push_back(struct_Mesh());

	struct_Mesh & mesh = meshData.subMeshes.back();
	mesh.GroupId = GroupId;
	mesh.format = 0;

	u32 const vertexcount = (u32)meshData.vertices.size();
	u32 const normalcount = (u32)meshData.normals.size();

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

int MREAData::LoadTextures()
{
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

	for(i=0; i<m_materialInfo.TextureCount; ++i)
	{
		CurTexture=Dir;
		CurTexture+="\\"+TextureNames[i];

		memset(buff,0,2048);
		if(TextureNames[i].c_str()[0] == '1'){
			decompFile(CurTexture.c_str(),buff);
		}else{
			sprintf(buff,"%s",CurTexture.c_str());
		}
		LoadTXTR(std::string(buff), &m_materialInfo.TexturesIDs[i]);//Should be loaded?
		//DeleteFile(buff);
	}

	return 0;
}
int MREAData::ReadTextures(void)
{
	int i =0;
	char sprinter[1024]={0};
	FILE* test;
	string stest;
	//unsigned long TEXT;
	int TextureCount=m_materialInfo.TextureCount;
	//	delete[] TextureNames;
	TextureNames.resize(TextureCount);
	lstTextures.Clear();
	lstTextures.SetListIndex(0);
	for(i=0;i<TextureCount;i++)
	{
		sprintf(sprinter,
			"0_%x.TXTR",
			m_materialInfo.TextureNames[i]
		);
		stest=strCurDir;
		stest+="\\";
		stest+=sprinter;
		test=fopen(stest.c_str(),"r+b");
		if(test==NULL)
		{
			//1_x didn't work so lets try 0_x
			sprintf(sprinter,
				"01_%x.TXTR",
				m_materialInfo.TextureNames[i]
			);
			stest=strCurDir;
			stest+="\\";
			stest+=sprinter;
			test=fopen(stest.c_str(),"r+b");
			int shit = GetLastError();
			if(test==NULL)
			{
				sprintf(sprinter,
					"1_%x.TXTR",
					m_materialInfo.TextureNames[i]
				);
				stest=strCurDir;
				stest+="\\";
				stest+=sprinter;
				test=fopen(stest.c_str(),"r+b");
				int shit = GetLastError();
				if(test==NULL)
				{
					MessageBox(0,"Some reason a texture that was referenced doesn't exist, please inform interdpth or revl8n on the model you are trying to load.","ERROR",MB_OK);
					sprintf(sprinter,"INVALID:%s",sprinter);
					goto nextloop;
				}
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


int MREAData::MeshData::draw(clsMaterialHeader & materialHeader)
{
	//hey, any chance you could speed this function up? Hit the CPU kind of hard, I just added some pointer changes
	int Seg=0;
	int datasize=0;
	int HowToDraw=GL_POINTS;
	int subtract=0;
	u32 i=0;
	int j=0;
	int DontDraw=-1;
	unsigned char* pnt=NULL;
	int lastuv=0;
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
// 	glLoadIdentity();									// Reset The Current Modelview Matrix
// 	glTranslatef(xCamera,yCamera,zCamera);						// Move Left 1.5 Units And Into The Screen 6.0
// 	// Drawing Using Triangles
	txtrinfo* info;
// 	//clsMaterialHeader* MaterialHeader;
// 	// mGroup* matGroup;
// 	glRotatef(xRot,1.0f,0.0f,0.0f);//Rotate x axis? o_O
// 	glRotatef(yRot,0.0f,1.0f,0.0f);
// 	glRotatef(zRot,0.0f,0.0f,1.0f);
	/*GLfloat Ambient[]= { 0.05f, 0.05f, 0.05f, 1.0f }; 				// Ambient Light Values ( NEW )
	GLfloat Diffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };				 // Diffuse Light Values ( NEW )
	GLfloat Position[]= { 0.0f, 0.0f, 20.0f, 255.0f };				 // Light Position ( NEW )
	glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);				// Setup The Ambient Light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);				// Setup The Diffuse Light
	glLightfv(GL_LIGHT0, GL_POSITION,Position);	*/		// Position The Light
	// rquad-=1.15f;

	//GL_POINTS);
	/*#define GL_POINTS                         0x0000
	#define GL_LINES                          0x0001
	#define GL_LINE_LOOP                      0x0002
	#define GL_LINE_STRIP                     0x0003
	#define GL_TRIANGLES                      0x0004
	#define GL_TRIANGLE_STRIP                 0x0005
	#define GL_TRIANGLE_FAN                   0x0006
	#define GL_QUADS                          0x0007
	#define GL_QUAD_STRIP                     0x0008
	#define GL_POLYGON                        0x0009
	*/
	//0x5d10	
	//glBindTexture(GL_TEXTURE_2D,CMDL.TextureIDs[1]);

	//if( CMDL.candraw!= 0)
	{
		//int index = lstMaterials.GetListIndex();
		//if(index<0) return 0;
		//MaterialHeader = &CMDL.MaterialHeader[index];

		u32 const vertexcount = (u32)vertices.size();
		u32 const normalcount = (u32)normals.size();
		u32 const uvcount = (u32)uvs.size();

		u32 l = 0;
		for (l = 0; l < 8; ++l)
		{
			glActiveTextureARB(GL_TEXTURE0_ARB + l);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
		glActiveTextureARB(GL_TEXTURE0_ARB);

		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);

		glBegin(GL_POINTS);
		for (i = 0; i < vertexcount; ++i)
		{
			glColor4f(
				0.0f,
				0.0f,
				0.0f,
				1.0f
				);

			glVertex3f(
				(vertices[i].X),
				(vertices[i].Y),
				(vertices[i].Z)
				);
		}
		glEnd();

		glEnable(GL_LIGHTING);
		glEnable(GL_BLEND);

		u32 const meshCount = (u32)subMeshes.size();
		// NOTE: use this to control which mesh materials are rendered, currently only allows a range
		u32 materialStart = 0;
		u32 materialEnd = materialHeader.MaterialCount;
		u32 Materials = 0;
		for (Materials = materialStart; (Materials < materialEnd) && (Materials < materialHeader.MaterialCount); ++Materials)
		{
			u32 uMesh = 0, uSubMesh = 0;
			u32 meshStart = 0;
			u32 meshEnd = meshCount;
			for (uMesh = meshStart; (uMesh < meshEnd) && (uMesh < meshCount); ++uMesh)
			{
				struct_Mesh & mesh = subMeshes[uMesh];

				if (mesh.GroupId != Materials)
				{
					continue;
				}

				info = &materialHeader.MaterialSegment[mesh.GroupId].info;

				for (uSubMesh = 0; (uSubMesh < mesh.subMeshes.size()); ++uSubMesh)
				{
					struct_SubMesh & submesh = mesh.subMeshes[uSubMesh];

					if (submesh.primitiveFlags == 0 ||
						0 >= submesh.indexCount ||
						submesh.indices.empty())
					{
						continue;
					}

					glPolygonMode(GL_FRONT, GL_FILL );
					glPolygonMode(GL_BACK, GL_LINE );

					u32 k=0;
					u32 l = 0;
					// NOTE: use this to control which texture images to use, currently only allows a range
					u32 kStart = 1, kEnd = kStart + 1; //info->count;
					if (uvcount != 0)
					{
						for (k = kStart; k < kEnd && k < info->count; ++k)
						{
							if (k > 0 && (info->ids[k] == info->ids[k-1])) 
							{
								//break;
							}
							//I guess I can do my crappy little test here! XD

							glActiveTextureARB(GL_TEXTURE0_ARB + l);
							glEnable(GL_TEXTURE_2D);
							//for helment trans o.o
							glDisable( GL_BLEND );
							/*
							#define GL_ZERO                           0
							#define GL_ONE                            1
							#define GL_SRC_COLOR                      0x0300
							#define GL_ONE_MINUS_SRC_COLOR            0x0301
							#define GL_SRC_ALPHA                      0x0302
							#define GL_ONE_MINUS_SRC_ALPHA            0x0303
							#define GL_DST_ALPHA                      0x0304
							#define GL_ONE_MINUS_DST_ALPHA            0x0305
							*/
							// glColor4f(4,2,4,0);
							glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

							glBindTexture(GL_TEXTURE_2D, materialHeader.TexturesIDs[info->ids[k]]);
							//glDisable(GL_BLEND);
							//	glBindTexture(GL_TEXTURE_2D, CMDL.MaterialHeader.TexturesIDs[4]);
							if (k > 0)
							{
								//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
							}
							++l;
						}
					}
					for (; l < 8; ++l)
					{
						glActiveTextureARB(GL_TEXTURE0_ARB + l);
						glBindTexture(GL_TEXTURE_2D, 0);
						glDisable(GL_TEXTURE_2D);
					}
					glActiveTextureARB(GL_TEXTURE0_ARB);

					switch (submesh.primitiveFlags & 0xF8)
					{
					case 0xB8:
						glBegin(GL_POINTS);
						break;
					case 0xA8:
						glBegin(GL_LINES);
						break;
					case 0xB0:
						glBegin(GL_LINE_STRIP);
						break;
					case 0x90:
						glBegin(GL_TRIANGLES);
						break;
					case 0x98:
						glBegin(GL_TRIANGLE_STRIP);
						break;
					case 0xA0:
						glBegin(GL_TRIANGLE_FAN);
						break;
					case 0x80:
						glBegin(GL_QUADS);
						break;
					default:
						glBegin(GL_POINTS);
						break;
					}

					struct_Faces * pFaces = &submesh.indices.front();

					u32 vertexIndex = 0;
					u32 normalIndex = 0;

					for (i=0; (i < submesh.indexCount); ++i)
					{
						vertexIndex = pFaces[i].vertexIndex;
						if (vertexIndex >= vertexcount)
						{
							break;
						}
						normalIndex = pFaces[i].normalIndex;
						if (normalIndex >= normalcount)
						{
							normalIndex = vertexIndex;
						}

						l = 0;
						if (uvcount != 0)
						{
							//u32 k=0;
							//kStart = 0;
							//kEnd = info->count;
							for(k = kStart; k < kEnd && k < info->count; ++k)
							{
								if (k > 0 && (info->ids[k] == info->ids[k-1]))
								{
									//continue;
								}

								GLuint uvIndex = pFaces[i].uvIndex[1];
								// NOTE: this currently seems like a hack, not sure if this is how this should be done
								// will probably be fixed once the attribute information from the headers is known
								// 							if (info->count > mesh.format)
								// 							{
								// 								if (mesh.format > 0)
								// 								{
								// 									uvIndex = pFaces[i].uvIndex[mesh.format-1];
								// 								}
								// 								else
								// 								{
								// 									uvIndex = normalIndex;
								// 								}
								// 							}
								// 							else
								// 							{
								// 								uvIndex = pFaces[i].uvIndex[k];
								// 							}

								float a=0;
								float b=0;
								if (uvIndex < uvcount)
								{
									a = uvs[uvIndex].U;
									b = uvs[uvIndex].V;
								}

								//glColor4f(0.0f, 0.0f, 0.0f, 255.0f);       //define the alpha value
								//glBlendFunc(GL_ONE, GL_ONE );  //define blending factors

								glMultiTexCoord2fARB(
									GL_TEXTURE0_ARB + l,
									a,
									b
									);
							}

							++l;
						}

						glColor4f(
							1.0f,
							1.0f,
							1.0f,
							1.0f
							);

						if (normalIndex < normalcount && normalcount !=0)
						{
							glNormal3f(
								(normals[normalIndex].X),
								(normals[normalIndex].Y),
								(normals[normalIndex].Z)
								);
						}
						glVertex3f(
							(vertices[vertexIndex].X),
							(vertices[vertexIndex].Y),
							(vertices[vertexIndex].Z)
							);
					}

					glEnd();
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
				}
			}
		}
	}

	return TRUE;
}


int MREAData::draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
//	glTranslatef(xCamera,yCamera,zCamera);						// Move Left 1.5 Units And Into The Screen 6.0

	//gluLookAt(xCamera, yCamera, zCamera, center.X, center.Y, center.Z, 0, 1, 0);
	//gluLookAt(xCamera, yCamera, zCamera,xRot, yRot, zRot, 0, 1, 0);
 	//glRotatef(xRot,1.0f,0.0f,0.0f);//Rotate x axis? o_O
 	//glRotatef(yRot,0.0f,1.0f,0.0f);
 	//glRotatef(zRot,0.0f,0.0f,1.0f);
	glTranslatef(xCamera,yCamera,zCamera);						// Move Left 1.5 Units And Into The Screen 6.0


	glRotatef(xRot,1.0f,0.0f,0.0f);//Rotate x axis? o_O
	glRotatef(yRot,0.0f,1.0f,0.0f);
	glRotatef(zRot,0.0f,0.0f,1.0f);
	for (u32 iMesh = 0; iMesh < m_fileHeader.meshCount; ++iMesh)
	//if (!m_meshes.empty())
	{
		if (!m_meshes.empty()&&glIsList(m_meshes[iMesh].glList))
		{
			glCallList(m_meshes[iMesh].glList);
		}
		else
		{
			m_meshes[iMesh].glList = glGenLists(1);
			glNewList(m_meshes[iMesh].glList, GL_COMPILE_AND_EXECUTE);
			m_meshes[iMesh].draw(m_materialInfo);
			glEndList();
		}
	}

	SwapBuffers(hDC);

	return 0;
}
