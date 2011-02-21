#ifndef MREA_H
#define MREA_H

#include "main.h"

#pragma pack(push, 1)

class MREAData
{
public:
	u32 m_sectionIndex;

	struct Header
	{
		u32 fileID;
		u32 fileVersion;
		float transformMatrix[12];
		u32 meshCount;
		u32 sectionCount;
		u32 unknown0x40[8];
	} m_fileHeader;;
	std::vector<u32> m_sectionSizes;
	clsMaterialHeader m_materialInfo;
	struct MeshData
	{
		//std::vector<u8> headerSection;
		struct MeshHeader 
		{
			u32 unknown0x00;
			float transformMatrix[12];
			float boundingBox[6];
		} header;
		//std::vector<u8> verticesSection;
		std::vector<tex> vertices;
		//std::vector<u8> normalsSection;
		std::vector<tex> normals;
		//
		std::vector<u8> zeroSection;
		//std::vector<u8> texCoordsSection;
		std::vector<suvMap> uvs;
		//
		std::vector<u8> unknownSection;
		//std::vector<u8> subMeshInfoSection;
		struct SubMeshInfo 
		{
			u32 subMeshCount;
			std::vector<u32> subMeshOffsets;
		} subMeshInfo;

		std::vector<struct_Mesh> subMeshes;

		GLuint glList;

		int draw(clsMaterialHeader & materialHeader);
	};
	std::vector<MeshData> m_meshes;
	std::vector<string> TextureNames;

	tex minBounds;
	tex maxBounds;
	tex center;

public:
	MREAData();
	~MREAData();

	int Read(char* fstr);
	bool ReadHeader(FILE* fp);
	int ReadMesh(FILE* fp, int index, int segmentNum);
	int ReadSubMesh(FILE* fp, MeshData & mesh);
	int ReadTextures(void);
	int LoadTextures(void);

	int draw(void);
};

#pragma pack(pop)

EXT MREAData MREA;

#endif // MREA_H
