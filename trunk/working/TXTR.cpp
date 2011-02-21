#include "main.h"
int g_keyColor[3] ={ 4, 2, 4 };
int DrawTXTR(){


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
//	glTranslatef(0,0,0);						// Move Left 1.5 Units And Into The Screen 6.0
	// Drawing Using Triangles
	glTranslatef(xCamera,yCamera,zCamera);						// Move Left 1.5 Units And Into The Screen 6.0
	// Drawing Using Triangles
    //txtrinfo* info;
	 //clsMaterialHeader* MaterialHeader;
	// mGroup* matGroup;
	glRotatef(xRot,1.0f,0.0f,0.0f);//Rotate x axis? o_O
	glRotatef(yRot,0.0f,1.0f,0.0f);
	glRotatef(zRot,0.0f,0.0f,1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, g_textureID );
    glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f); glVertex3f(-1.0f,-1.0f, 0.0f );
    glTexCoord2f(1.0f,0.0f); glVertex3f(1.0f,-1.0f, 0.0f );
    glTexCoord2f(1.0f,1.0f); glVertex3f(1.0f, 1.0f, 0.0f );
    glTexCoord2f(0.0f,1.0f); glVertex3f(-1.0f, 1.0f, 0.0f );
    glEnd();
    SwapBuffers(hDC);
	return 0;
}
int SetTheirAlpha(unsigned char* pnt){
	int g_keyColor[18][3]={
		{0,0,0},
		{4,2,4},
		{5,3,5},
		{126,130,126},
		{70,71,70},
		{186,189,186},
		{42,42,42},
		{105,106,105},
		{163,164,163},
		{220,220,220},
		{89,89,89},
		{26,24,26},
		{146,147,146},
		{204,206,204},
		{55,56,55},
		{118,118,118},
		{177,178,177},
		{232,238,232}
	};
int i=0;
for(i=0;i<18;i++){
			if( pnt[i]   == g_keyColor[i][0] &&
                pnt[i+1] == g_keyColor[i][1] &&
                pnt[i+2] == g_keyColor[i][2] )
            {
                
				return 0;   // If so, set alpha to fully transparent.
             }

}
return 255;
}

void LoadTXTR(string fileName,GLuint*  txtrno){
	int i=0;
   char* buffer=NULL;
   char* RGBB=NULL;
	vector<unsigned char> btiArray;
	if(dumpTxtr(fileName.c_str(), &btiArray)==1){
		MessageBox(0,"ERROR1","ERROR",MB_OK);
		return;
	}
	if(dumpBti(&btiArray,std::string("Temp.dds"))==1){
		MessageBox(0,"ERROR2","ERROR",MB_OK);
		return ;
	}

	
	//loadDds("Temp.dds", txtrno);
	
//	  SDL_Event event;
  ILuint texid; /* ILuint is a 32bit unsigned integer.
    Variable texid will be used to store image name. */
  ILboolean success; /* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
    it can have different value (because it's just typedef of unsigned char), but this sould be
    avoided.
    Variable success will be used to determine if some function returned success or failure. */
  //GLuint image;
  //int finished;


 
  ilInit(); /* Initialization of DevIL */
  ilGenImages(1, &texid); /* Generation of one image name */
  ilBindImage(texid); /* Binding of image name */
  success = ilLoadImage("Temp.dds"); /* Loading of image "image.jpg" */
  

  if (success) /* If no error occured: */
  {
	    
      
	//  ilClearColour(0, 0, 0, 0);
	  //ilSetAlpha(1);
	  //ilKeyColour(0,0,0,1);
   // success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every colour component into
    //  unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
    success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	unsigned char* pnt = ilGetData();
    glGenTextures(1, txtrno); /* Texture name generation */
    glBindTexture(GL_TEXTURE_2D, *txtrno); /* Binding of texture name */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear
      interpolation for magnification filter */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear
      interpolation for minifying filter */
	
	 /*   for( i = 0; i < ilGetInteger(IL_IMAGE_WIDTH)* ilGetInteger(IL_IMAGE_HEIGHT)* 4; i += 4 )
        {
           // Does the current pixel match the selected color key?
            if( pnt[i]   == g_keyColor[0] &&
                pnt[i+1] == g_keyColor[1] &&
                pnt[i+2] == g_keyColor[2] )
            {
                
				pnt[i]=pnt[i+1]=pnt[i+2]=pnt[i+3] = 0;   // If so, set alpha to fully transparent.
            }
            else
            {
                pnt[i+3] =255; // If not, set alpha to fully opaque.
            }

          //pnt[i+3]= SetTheirAlpha(&pnt[i] );
        }*/
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
      ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
      pnt); 
	/* Texture specification */


  
  }
  else
  {
    /* Error occured */
    //SDL_Quit();
    return ;
  }
  ilDeleteImages(1, &texid); /* Because we have already copied image data into texture data
    we can release memory used by image. */
  CMDL.candraw=1;
	return ;
}