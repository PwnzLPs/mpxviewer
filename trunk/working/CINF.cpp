#include "main.h"
int LoadCINF(){

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

  //glEnable(GL_2DTEXTURE);
	if( CMDL.candraw!= 0){
	              
	glColor3f(0,0,0);
	/*	glBegin(GL_QUADS);						// Draw A Quad
		glVertex3f(-1.0f, 1.0f, 0.0f);				// Top Left
		glVertex3f( 1.0f, 1.0f, 0.0f);				// Top Right
		glVertex3f( 1.0f,-1.0f, 0.0f);				// Bottom Right
		glVertex3f(-1.0f,-1.0f, 0.0f);				// Bottom Left
	glEnd();	*/	
					 glBegin(GL_LINES); 
					 for(i=0;i<Joints.jointCount+3;i++){
					
						for(j=0;j<Joints.JointIndice[i].ref;j++){
							glVertex3f(
								Joints.JointIndice[i].pos.X,
								Joints.JointIndice[i].pos.Y,
								Joints.JointIndice[i].pos.Z);

							glVertex3f(
							(Joints.JointIndice[Joints.JointIndice[i].refs[j]].pos.X),
							(Joints.JointIndice[Joints.JointIndice[i].refs[j]].pos.Y),
							(Joints.JointIndice[Joints.JointIndice[i].refs[j]].pos.Z)
							);
						}
				
				    }


				
				   glEnd();
			
	}
	
    glColor3f(1.f,1.f,1.f);
	SwapBuffers(hDC);
	return TRUE;										// Keep Going
}
void ReadCINF(char* FileName){
u32 test=0,test2=0;
int i=0;
int j=0;
struct_joints* Joint;
FILE* fp=fopen(FileName,"r+b");
	if(fp){
		fseek(fp,0,SEEK_SET);
		fread(&Joints.jointCount,1,4,fp);
		toDWORD((u32&)Joints.jointCount);//Might not really be joint count..
		Joints.JointIndice.resize(Joints.jointCount+3);
		//Clear the indexes :o
		
		for(i=0;i<Joints.jointCount+3;i++){
			Joints.JointIndice[i].parent=Joints.JointIndice[i].index=Joints.JointIndice[i].pos.X=Joints.JointIndice[i].pos.Y=Joints.JointIndice[i].pos.Z=Joints.JointIndice[i].ref=0;
		}


        Joints.JointIndice.resize(Joints.jointCount+3);
		for(i=0;i<Joints.jointCount;i++){
	    
			fread(&test,1,4,fp);
			toDWORD((u32&)test);
		//Joint=&Joints.JointIndice[test];
			Joints.JointIndice[test].index=test;

			fread(&Joints.JointIndice[test].parent,1,4,fp);
			toDWORD((u32&)Joints.JointIndice[test].parent);
			fread(&Joints.JointIndice[test].pos.X,1,4,fp);
			toDWORD((u32&)Joints.JointIndice[test].pos.X);
			
			fread(&Joints.JointIndice[test].pos.Y,1,4,fp);
			toDWORD((u32&)Joints.JointIndice[test].pos.Y);
			
			fread(&Joints.JointIndice[test].pos.Z,1,4,fp);
			toDWORD((u32&)Joints.JointIndice[test].pos.Z);
			
			fread(&Joints.JointIndice[test].ref,1,4,fp);
			toDWORD((u32&)Joints.JointIndice[test].ref);
			
			Joints.JointIndice[test].refs.resize(Joints.JointIndice[test].ref);
			for(j=0;j<Joints.JointIndice[test].ref;j++){
				fread(&Joints.JointIndice[test].refs[j],1,4,fp);
				toDWORD((u32&)Joints.JointIndice[test].refs[j]);
			}
	

		}




	fclose(fp);
	CMDL.candraw=1;
	}
}
