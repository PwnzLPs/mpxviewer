#include "main.h"

#define GX_BL_ZERO			0
#define GX_BL_ONE			1
#define GX_BL_SRCCLR		2
#define GX_BL_INVSRCCLR		3
#define GX_BL_SRCALPHA		4
#define GX_BL_INVSRCALPHA	5
#define GX_BL_DSTALPHA		6
#define GX_BL_INVDSTALPHA	7
#define GX_BL_DSTCLR		GX_BL_SRCCLR
#define GX_BL_INVDSTCLR		GX_BL_INVSRCCLR

void HowtoBlend(u8 val, int* blenders)
{
	switch(val)
	{
	case GX_BL_ZERO:

		
	case GX_BL_ONE:
           blenders[0]=GL_ONE;blenders[1]=GL_ONE;
		break;
	case GX_BL_SRCCLR:

		
	case GX_BL_INVSRCCLR:

		blenders[0]=GL_SRC_COLOR;blenders[1]=GL_ONE_MINUS_SRC_COLOR;
		break;
	case GX_BL_SRCALPHA:

		//break;
	case GX_BL_INVSRCALPHA:
		blenders[0]=GL_SRC_ALPHA;blenders[1]=GL_ONE_MINUS_SRC_ALPHA;
		break;
	case GX_BL_DSTALPHA:

	case GX_BL_INVDSTALPHA:
		blenders[0]=GL_DST_ALPHA;blenders[1]=GL_ONE_MINUS_DST_ALPHA;
		break;
	}
}

int LoadCMDL()
{
	//hey, any chance you could speed this function up? Hit the CPU kind of hard, I just added some pointer changes
	int Seg=0;
	int datasize=0;
	int HowToDraw=GL_POINTS;
	int subtract=0;
	int i=0;
	int j=0;
	int DontDraw=-1;
	unsigned char* pnt=NULL;
	int lastuv=0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(xCamera,yCamera,zCamera);						// Move Left 1.5 Units And Into The Screen 6.0
	// Drawing Using Triangles
	txtrinfo* info;
	clsMaterialHeader* MaterialHeader;
	// mGroup* matGroup;
	glRotatef(xRot,1.0f,0.0f,0.0f);//Rotate x axis? o_O
	glRotatef(yRot,0.0f,1.0f,0.0f);
	glRotatef(zRot,0.0f,0.0f,1.0f);
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

	if( CMDL.candraw!= 0)
	{
		int index=lstMaterials.GetListIndex();
		if(index<0) return 0;
		MaterialHeader = &CMDL.MaterialHeader[index];

		u32 l = 0;
		for(l=0;l<8;l++)
		{
			glActiveTextureARB(GL_TEXTURE0_ARB + l);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
		glActiveTextureARB(GL_TEXTURE0_ARB);

		/*glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);

		glBegin(GL_POINTS);
		for(i=0;i<CMDL.vertexcount;i++){
			glColor4f(
				0.0f,
				0.0f,
				0.0f,
				1.0f
				);

			glVertex3f(
				(CMDL.vertexes[i].X),
				(CMDL.vertexes[i].Y),
				(CMDL.vertexes[i].Z)
				);
		}
		glEnd();*/

		glEnable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);	// Turn Depth Testing Off


		u32 const meshCount = (u32)CMDL.Meshes.size();
		// NOTE: use this to control which mesh materials are rendered, currently only allows a range
		u32 materialStart = 0;
		u32 materialEnd = MaterialHeader->MaterialCount;
		u32 Materials = 0;
		for (Materials = materialStart; (Materials < materialEnd) && (Materials < MaterialHeader->MaterialCount); ++Materials)
		{
			u32 uMesh = 0, uSubMesh = 0;
			u32 meshStart = 0;
			u32 meshEnd = meshCount;
			for (uMesh = meshStart; (uMesh < meshEnd) && (uMesh < meshCount); ++uMesh)
			{
				struct_Mesh & mesh = CMDL.Meshes[uMesh];

				if (mesh.GroupId != Materials)
				{
					continue;
				}

				info = &MaterialHeader->MaterialSegment[mesh.GroupId].info;

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
					u32 kStart = 0, kEnd = info->count;//kStart + 1; //
					if (CMDL.uvcount != 0)
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
							glEnable( GL_BLEND );
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
							//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

							if(Materials!=0 &&
								MaterialHeader->MaterialSegment[Materials].Attributes.size() > k &&
								MaterialHeader->MaterialSegment[Materials-1].Attributes.size() > k  )
							{//hack
								/*int ttype =MaterialHeader->MaterialSegment[Materials].Attributes[k].Unknown6;
								int blenders[2];
								if(!(ttype>7))
								{
								HowtoBlend(ttype,blenders);
								glBlendFunc(blenders[0],blenders[1]);
								}*/

								if(MaterialHeader->MaterialSegment[Materials].Attributes[k].BlendType == MaterialHeader->MaterialSegment[Materials-1].Attributes[k].BlendType)
								{
									int ttype =MaterialHeader->MaterialSegment[Materials].Attributes[k].BlendType;
									int blenders[2]={0};
									if (!(ttype>7))
									{
										HowtoBlend(ttype,blenders);
										glBlendFunc(blenders[0],blenders[1]);
										//	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
									}
									else
									{
										//break;
									}
								}
							}

							glBindTexture(GL_TEXTURE_2D, MaterialHeader->TexturesIDs[info->ids[k]]);
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
						if (vertexIndex >= CMDL.vertexcount)
						{
							break;
						}
						normalIndex = pFaces[i].normalIndex;
						if (normalIndex >= CMDL.normalcount)
						{
							normalIndex = vertexIndex;
						}

						l = 0;
						if (CMDL.uvcount != 0)
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

								GLuint uvIndex = 0; //pFaces[i].uvIndex[1];
								// NOTE: this currently seems like a hack, not sure if this is how this should be done
								// will probably be fixed once the attribute information from the headers is known
								if (k < mesh.format)
								{
									uvIndex = pFaces[i].uvIndex[k];
								}
								else //if (info->count > mesh.format)
								{
									if (mesh.format > 0)
									{
										uvIndex = pFaces[i].uvIndex[mesh.format-1];
									}
									else
									{
										uvIndex = normalIndex;
									}
								}

								float a=0;
								float b=0;
								if (uvIndex < CMDL.uvcount)
								{
									a = CMDL.UVs[uvIndex].U;
									b = CMDL.UVs[uvIndex].V;
								}

								//glColor4f(0.0f, 0.0f, 0.0f, 255.0f);       //define the alpha value
								//glBlendFunc(GL_ONE, GL_ONE );  //define blending factors
							int BlendNo=MaterialHeader->MaterialSegment[Materials].Attributes[l].BlendNumber;
							if((BlendNo)==3) {
								glColor4f(
									(float)MaterialHeader->MaterialSegment[Materials].Attributes[l].hue,
									(float)MaterialHeader->MaterialSegment[Materials].Attributes[l].sat,
									(float)MaterialHeader->MaterialSegment[Materials].Attributes[l].lum,
									0.5f);
							 //glColor4f(1.0f, 1.0f, 1.0f, 0.5f);aaa
							}else{
								
								glColor4f(
									(float)MaterialHeader->MaterialSegment[Materials].Attributes[l].hue,
									(float)MaterialHeader->MaterialSegment[Materials].Attributes[l].sat,
									(float)MaterialHeader->MaterialSegment[Materials].Attributes[l].lum,
									1.0f);
							 //glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	
							}
							glLightfv(GL_LIGHT0, GL_AMBIENT,&MaterialHeader->MaterialSegment[Materials].LayerData[l]);//LightAmbient);	
							glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,0x7f);//(float)(MaterialHeader->MaterialSegment[Materials].Attributes[l].unknown0x00&0x7F));
								glMultiTexCoord2fARB(
									GL_TEXTURE0_ARB + l,
									a,
									b
									);
							}

							++l;
						}


						if (normalIndex < CMDL.normalcount && CMDL.normalcount !=0)
						{
							glNormal3f(
								(CMDL.mNormals[normalIndex].X),
								(CMDL.mNormals[normalIndex].Y),
								(CMDL.mNormals[normalIndex].Z)
								);
						}
						glVertex3f(
							(CMDL.vertexes[vertexIndex].X),
							(CMDL.vertexes[vertexIndex].Y),
							(CMDL.vertexes[vertexIndex].Z)
							);
					}

					glEnd();
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
				}
			}
		}
	}
// 
// 		unsigned int meshCount = CMDL.Meshes.size();
// 		int iMesh = 0;
// 		// NOTE: use this to control which mesh materials are rendered, currently only allows a range
// 		int materialStart = 0;
// 		int materialEnd = MaterialHeader->MaterialCount;
// 		int Materials=0;
// 		for(Materials=materialStart; Materials<materialEnd && (u32)Materials<MaterialHeader->MaterialCount;Materials++){
// 			/*	pnt=&MaterialHeader->MaterialSegments[Materials].front();
// 			memcpy(&info.count,&pnt[4],4);//Read Texture information for this segment
// 			toDWORD((u32&)info.count);
// 			info.ids.resize(info.count);
// 			for(i=0;i<info.count;i++){
// 			memcpy(&info.ids[i],&pnt[8+i*4],4);
// 			toDWORD((u32&)info.ids[i]);
// 			}*/
// 			info=&MaterialHeader->MaterialSegment[Materials].info;
// 			for (iMesh = 0; iMesh < meshCount; ++iMesh) //!CMDL.Meshes.empty())
// 			{
// 
// 				struct_Mesh & mesh = CMDL.Meshes[iMesh]; //.front();
// 
// 				if(mesh.GroupId!=Materials)
// 				{
// 					continue;
// 				}
// 				if (mesh.primitiveFlags == 0 ||
// 					0 >= mesh.indexCount ||
// 					mesh.indices.empty())
// 				{
// 					continue;
// 				}
// 
// 				glPolygonMode(GL_FRONT, GL_FILL );
// 				glPolygonMode(GL_BACK, GL_LINE );
// 
// 				int l = 0;
// 				int kStart=0,kEnd=0;
// 				if(	CMDL.uvcount!=0)
// 				{
// 					// NOTE: use this to control which texture images to use, currently only allows a range
// 					kStart = 0;
// 					kEnd = kStart+1; //info->count;
// 					unsigned int k=0;
// 					for(k=kStart;k<kEnd && k<info->count;k++)
// 					{
// 						if (k > 0 && (info->ids[k] == info->ids[k-1])) 
// 						{
// 							//break;
// 						}
// 						//I guess I can do my crappy little test here! XD
// 
// 						glActiveTextureARB(GL_TEXTURE0_ARB + k);
// 						glEnable(GL_TEXTURE_2D);
// 						//for helment trans o.o
// 						glEnable( GL_BLEND );
// 						/*
// 						#define GL_ZERO                           0
// 						#define GL_ONE                            1
// 						#define GL_SRC_COLOR                      0x0300
// 						#define GL_ONE_MINUS_SRC_COLOR            0x0301
// 						#define GL_SRC_ALPHA                      0x0302
// 						#define GL_ONE_MINUS_SRC_ALPHA            0x0303
// 						#define GL_DST_ALPHA                      0x0304
// 						#define GL_ONE_MINUS_DST_ALPHA            0x0305
// 						*/
// 						// glColor4f(4,2,4,0);
// 						glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
// 
// 						glBindTexture(GL_TEXTURE_2D, MaterialHeader->TexturesIDs[info->ids[k]]);
// 						//glDisable(GL_BLEND);
// 						//	glBindTexture(GL_TEXTURE_2D, CMDL.MaterialHeader.TexturesIDs[4]);
// 						if (k > 0)
// 						{
// 							//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// 						}
// 						++l;
// 					}
// 				}
// 				for(;l<8;l++)
// 				{
// 					glActiveTextureARB(GL_TEXTURE0_ARB + l);
// 					glBindTexture(GL_TEXTURE_2D, 0);
// 					glDisable(GL_TEXTURE_2D);
// 				}
// 				glActiveTextureARB(GL_TEXTURE0_ARB);
// 
// 				switch (mesh.primitiveFlags & 0xF8)
// 				{
// 				case 0xB8:
// 					glBegin(GL_POINTS);
// 					break;
// 				case 0xA8:
// 					glBegin(GL_LINES);
// 					break;
// 				case 0xB0:
// 					glBegin(GL_LINE_STRIP);
// 					break;
// 				case 0x90:
// 					glBegin(GL_TRIANGLES);
// 					break;
// 				case 0x98:
// 					glBegin(GL_TRIANGLE_STRIP);
// 					break;
// 				case 0xA0:
// 					glBegin(GL_TRIANGLE_FAN);
// 					break;
// 				case 0x80:
// 					glBegin(GL_QUADS);
// 					break;
// 				default:
// 					glBegin(GL_POINTS);
// 					break;
// 				}
// 
// 				struct_Faces * pFaces = &mesh.indices.front();
// 
// 				int vertexIndex = 0;
// 				int normalIndex = 0;
// 
// 				for(i=0; (i < mesh.indexCount); i++)
// 				{
// 					vertexIndex = pFaces[i].vertexIndex;
// 					if (vertexIndex >= CMDL.vertexcount)
// 					{
// 						break;
// 					}
// 					normalIndex = pFaces[i].normalIndex;
// 					if (normalIndex >= CMDL.normalcount)
// 					{
// 						normalIndex = vertexIndex;
// 					}
// 
// 					int l = 0;
// 					if(	CMDL.uvcount!=0)
// 					{
// 						unsigned int k=0;
// 						kStart = 0;
// 						kEnd = info->count;
// 						for(k=kStart;k<kEnd && k<info->count;k++)
// 						{
// 							if (k > 0 && (info->ids[k] == info->ids[k-1]))
// 							{
// 								//continue;
// 							}
// 
// 							GLuint uvIndex = 0;
// 							// NOTE: this currently seems like a hack, not sure if this is how this should be done
// 							// will probably be fixed once the attribute information from the headers is known
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
// 
// 							float a=0;
// 							float b=0;
// 							if (uvIndex < CMDL.uvcount)
// 							{
// 								a = CMDL.UVs[uvIndex].U;
// 								b = CMDL.UVs[uvIndex].V;
// 							}
// 
// 							//glColor4f(0.0f, 0.0f, 0.0f, 255.0f);       //define the alpha value
// 							//glBlendFunc(GL_ONE, GL_ONE );  //define blending factors
// 
// 							glMultiTexCoord2fARB(
// 								GL_TEXTURE0_ARB + l,
// 								a,
// 								b
// 								);
// 						}
// 
// 						++l;
// 					}
// 
// 					glColor4f(
// 						1.0f,
// 						1.0f,
// 						1.0f,
// 						1.0f
// 						);
// 
// 					if(((unsigned int)normalIndex<CMDL.mNormals.size() && CMDL.normalcount !=0))
// 					{
// 						glNormal3f(
// 							(CMDL.mNormals[normalIndex].X),
// 							(CMDL.mNormals[normalIndex].Y),
// 							(CMDL.mNormals[normalIndex].Z)
// 							);
// 					}
// 					glVertex3f(
// 						(CMDL.vertexes[vertexIndex].X),
// 						(CMDL.vertexes[vertexIndex].Y),
// 						(CMDL.vertexes[vertexIndex].Z)
// 						);
// 				}
// 
// 				glEnd();
// 				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
// 			}
// 		}
// 	}

	SwapBuffers(hDC);
	return TRUE;										// Keep Going
}
