/*
*		This Code Was Created By Jeff Molofee 2000
*		A HUGE Thanks To Fredric Echols For Cleaning Up
*		And Optimizing The Base Code, Making It More Flexible!
*		If You've Found This Code Useful, Please Let Me Know.
*		Visit My Site At nehe.gamedev.net
*/
#include "main.h"
#include "mrea.h"
//#include <windows.h>		// Header File For Windows

//#include <glaux.h>		// Header File For The Glaux Libraryfmain

GLfloat LightAmbient[]= { 0.05f, 0.05f, 0.05f, 1.0f }; 				// Ambient Light Values ( NEW )
GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };				 // Diffuse Light Values ( NEW )
GLfloat LightPosition[]= { 0.0f, 0.0f, 10.0f, 1.0f };				 // Light Position ( NEW )
/*Matrix4fT   Transform   = {  1.0f,  0.0f,  0.0f,  0.0f,				// NEW: Final Transform
0.0f,  1.0f,  0.0f,  0.0f,
0.0f,  0.0f,  1.0f,  0.0f,
0.0f,  0.0f,  0.0f,  1.0f };

Matrix3fT   LastRot     = {  1.0f,  0.0f,  0.0f,					// NEW: Last Rotation
0.0f,  1.0f,  0.0f,
0.0f,  0.0f,  1.0f };

Matrix3fT   ThisRot     = {  1.0f,  0.0f,  0.0f,					// NEW: This Rotation
0.0f,  1.0f,  0.0f,
0.0f,  0.0f,  1.0f };

*/
GLfloat     rquad;

HGLRC		hRC=NULL;		// Permanent Rendering Context

typedef struct {									// Structure For Keyboard Stuff
	BOOL keyDown [256];								// Holds TRUE / FALSE For Each Key
} Keys;												// Keys
Keys*		g_keys;
// User Defined Variables
GLUquadricObj *quadratic;											// Used For Our Quadric

const float PI2 = 2.0*3.1415926535f;								// PI Squared
HINSTANCE	hInstance;		// Holds The Instance Of The Application
// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
int rotate=1;
GLuint	texture[1];	
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
// AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
// {
// 	FILE *File=NULL;									// File Handle
// 
// 	if (!Filename)										// Make Sure A Filename Was Given
// 	{
// 		return NULL;									// If Not Return NULL
// 	}
// 
// 	File=fopen(Filename,"r");							// Check To See If The File Exists
// 
// 	if (File)											// Does The File Exist?
// 	{
// 		fclose(File);									// Close The Handle
// 		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
// 	}
// 
// 	return NULL;										// If Load Failed Return NULL
// }
// 
// int LoadGLTextures()									// Load Bitmaps And Convert To Textures
// {
// 	int Status=FALSE;									// Status Indicator
// 
// 	AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture
// 
// 	memset(TextureImage,0,sizeof(void *)*1);           	// Set The Pointer To NULL
// 
// 	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
// 	if (TextureImage[0]=LoadBMP("Data/NeHe.bmp"))
// 	{
// 		Status=TRUE;									// Set The Status To TRUE
// 
// 		glGenTextures(1, &texture[0]);					// Create The Texture
// 
// 		// Typical Texture Generation Using Data From The Bitmap
// 		glBindTexture(GL_TEXTURE_2D, texture[0]);
//	glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
// 		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
// 		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
// 	}
// 
// 	if (TextureImage[0])									// If Texture Exists
// 	{
// 		if (TextureImage[0]->data)							// If Texture Image Exists
// 		{
// 			free(TextureImage[0]->data);					// Free The Texture Image Memory
// 		}
// 
// 		free(TextureImage[0]);								// Free The Image Structure
// 	}
// 
// 	return Status;										// Return The Status
// }

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,1000000.0f);

	// Select The Modelview Matrix
	// Reset The Modelview Matrix

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
	glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
	glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
	glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");

	glEnable(GL_TEXTURE_2D);						// Enable Texture Mapping ( NEW )

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);						// Enable Smooth Shading
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);					// Black Background
	glClearDepth(1.0f);							// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);							// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Really Nice Perspective Calculations

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);				// Setup The Ambient Light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);				// Setup The Diffuse Light
	glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);			// Position The Light
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	init();
	return TRUE;	
}
long RetLong(long& address){
	long newnum = address;
	toDWORD((u32&)newnum);
	return newnum;

}
float RetFloat(float& address){
	float newnum = address;
	toDWORD((u32&)newnum);
	return newnum;

}
void Update ()
{							// Perform Motion Updates Here
	/*
	if (isRClicked)													// If Right Mouse Clicked, Reset All Rotations
	{
	Matrix3fSetIdentity(&LastRot);								// Reset Rotation
	Matrix3fSetIdentity(&ThisRot);								// Reset Rotation
	Matrix4fSetRotationFromMatrix3f(Transform, &ThisRot);		// Reset Rotation
	}

	if (!isDragging)												// Not Dragging
	{
	if (isClicked)												// First Click
	{
	isDragging = true;										// Prepare For Dragging
	LastRot = ThisRot;										// Set Last Static Rotation To Last Dynamic One
	ArcBall->click(&MousePt);								// Update Start Vector And Prepare For Dragging
	}
	}
	else
	{
	if (isClicked)												// Still Clicked, So Still Dragging
	{
	Quat4fT     ThisQuat;

	ArcBall->drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion
	Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
	Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
	Matrix4fSetRotationFromMatrix3f(Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
	}
	else														// No Longer Dragging
	isDragging = false;
	}*/

}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	// glPushMatrix();													// NEW: Prepare Dynamic Transform
	//glMultMatrixf(Transform.M);	

	switch(FileType){
		case 0:
			LoadCMDL();
			break;
		case 1:
			DrawTXTR();
			break;
		case 2:
			LoadCINF();
			break;
		case 3:
			MREA.draw();
			break;
	}

	//	glPopMatrix();													// NEW: Unapply Dynamic Transform

	//	glFlush ();	
	return 0;
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	/*	if (fullscreen)										// Are We In Fullscreen Mode?
	{
	ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
	ShowCursor(TRUE);								// Show Mouse Pointer
	}
	*/

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hwndGL,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hwndGL && !DestroyWindow(hwndGL))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hwndGL=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
*	title			- Title To Appear At The Top Of The Window				*
*	width			- Width Of The GL Window Or Fullscreen Mode				*
*	height			- Height Of The GL Window Or Fullscreen Mode			*
*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	//	DWORD		dwExStyle;				// Window Extended Style
	//	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	//fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= hGlobal;				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW;// | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hGlobal;							// Set The Instance
	wc.hIcon			= NULL;			// Load The Default Icon
	wc.hCursor			= NULL;			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}

	/*if (fullscreen)												// Attempt Fullscreen Mode?
	{
	DEVMODE dmScreenSettings;								// Device Mode
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
	dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
	dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
	dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{
	// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
	if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
	{
	fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
	}
	else
	{
	// Pop Up A Message Box Letting User Know The Program Is Closing.
	MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
	return FALSE;									// Return FALSE
	}
	}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
	dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
	dwStyle=WS_POPUP;										// Windows Style
	ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{*/
	//dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	//dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	//}

	//AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hwndGL=CreateWindowEx(	NULL,							// Extended Style For The Window
		"OpenGL",							// Class Name
		"Model",								// Window Title
		WS_VISIBLE|WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME   ,					// Required Window Style
		384, 0,								// Window Position
		640,	// Calculate Window Width
		480,	// Calculate Window Height
		hwndMain,								// No Parent Window
		NULL,								// No Menu
		hGlobal,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 16Bit Z-Buffer (Depth Buffer)  
		8,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC=GetDC(hwndGL)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hwndGL,SW_SHOW);						// Show The Window
	SetForegroundWindow(hwndGL);						// Slightly Higher Priority
	SetFocus(hwndGL);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

float ReturnNum(HWND hwnd){
	char buff[11]={0};
	float val=0;
	GetWindowText(hwnd,buff,10);
	sscanf(buff,"%f",&val);

	return val;
}
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
						 UINT	uMsg,			// Message For This Window
						 WPARAM	wParam,			// Additional Message Information
						 LPARAM	lParam)			// Additional Message Information
{
	float xr=0,yr=0,zr=0;
	float xc=0,yc=0,zc=0;

	xr=ReturnNum(GetDlgItem(hwndMain,ID_XRS));
	yr=ReturnNum(GetDlgItem(hwndMain,ID_YRS));
	zr=ReturnNum(GetDlgItem(hwndMain,ID_ZRS));

	xc=ReturnNum(GetDlgItem(hwndMain,ID_XCS));
	yc=ReturnNum(GetDlgItem(hwndMain,ID_YCS));
	zc=ReturnNum(GetDlgItem(hwndMain,ID_ZCS));
	//	HDC hdc;
	PAINTSTRUCT ps;

	switch (uMsg)									// Check For Windows Messages
	{
	case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}
	case WM_PAINT:
		DrawGLScene();
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

	case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_LEFT:   
			if(GetAsyncKeyState( VK_SHIFT )){
				xRot-=0.2f*xr;
			}else{
				xCamera-=0.2F*xc;
			}
			DrawGLScene();
			InvalidateRect(hwndGL,0,1);
			break;
		case VK_UP:    
			if(GetAsyncKeyState( VK_SHIFT )){
				yRot+=0.2f*yr;
			}else{
				yCamera+=0.2F*yc;
			}
			DrawGLScene();
			InvalidateRect(hwndGL,0,1);
			break;            
		case VK_RIGHT: 
			if(GetAsyncKeyState( VK_SHIFT )){
				xRot+=0.2f*xr;
			}else{
				xCamera+=0.2F*xc;
			}
			DrawGLScene();
			InvalidateRect(hwndGL,0,1);
			break;          
		case VK_DOWN:  
			if(GetAsyncKeyState( VK_SHIFT )){
				yRot-=0.2f*yr;
			}else{
				yCamera-=0.2F*yc;
			}
			DrawGLScene();
			InvalidateRect(hwndGL,0,1);
			break;
		case VK_OEM_PLUS:
			if(GetAsyncKeyState( VK_SHIFT )){
				zRot+=0.2f*zr;
			}else{
				zCamera+=0.2F*zc;
			}
			DrawGLScene();
			InvalidateRect(hwndGL,0,1);
			break;
		case VK_OEM_MINUS:
			if(GetAsyncKeyState( VK_SHIFT )){
				zRot-=0.2f*zr;
			}else{
				zCamera-=0.2F*zc;
			}
			DrawGLScene();
			InvalidateRect(hwndGL,0,1);
			break;
		}
	case WM_MOUSEMOVE:
		if(isClicked){
			MousePt.s.X = (GLfloat)LOWORD(lParam);
			MousePt.s.Y = (GLfloat)HIWORD(lParam);
			//isClicked   = (LOWORD(wParam) & MK_LBUTTON) ? true : false;

			// isRClicked  = (LOWORD(wParam) & MK_RBUTTON) ? true : false;
			DrawGLScene();
			MousePt.t.X = MousePt.s.X;
			MousePt.t.X = MousePt.s.Y;
		}
		return 0;
	case WM_LBUTTONUP:
		isClicked   = false;

		//Update();
		break;
	case WM_RBUTTONUP:
		isRClicked  = false;
		// Update();
		break;
	case WM_LBUTTONDOWN:
		if(!isClicked){
			MousePt.t.X = MousePt.s.X = (GLfloat)LOWORD(lParam);
			MousePt.t.X = MousePt.s.Y = (GLfloat)HIWORD(lParam);
			isClicked   = true;
		}
		///Update();
		break;
	case WM_RBUTTONDOWN:
		isRClicked  = true;
		// Update();
		break;


	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
/*
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
HINSTANCE	hPrevInstance,		// Previous Instance
LPSTR		lpCmdLine,			// Command Line Parameters
int			nCmdShow)			// Window Show State
{
MSG		msg;									// Windows Message Structure
BOOL	done=FALSE;								// Bool Variable To Exit Loop

// Ask The User Which Screen Mode They Prefer
//if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
//{
//		fullscreen=FALSE;							// Windowed Mode
//}

// Create Our OpenGL Window
if (!CreateGLWindow("Metroid Stuff",640,480,16,0))
{
return 0;									// Quit If Window Was Not Created
}

while(!done)									// Loop That Runs While done=FALSE
{
if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
{
if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
{
done=TRUE;							// If So done=TRUE
}
else									// If Not, Deal With Window Messages
{
TranslateMessage(&msg);				// Translate The Message
DispatchMessage(&msg);				// Dispatch The Message
}
}
else										// If There Are No Messages
{
// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
{
done=TRUE;							// ESC or DrawGLScene Signalled A Quit
}
else									// Not Time To Quit, Update Screen
{
SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
}

/*if (keys[VK_F1])						// Is F1 Being Pressed?
{
keys[VK_F1]=FALSE;					// If So Make Key FALSE
KillGLWindow();						// Kill Our Current Window
//				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
// Recreate Our OpenGL Window
if (!CreateGLWindow("NeHe's First Polygon Tutorial",640,480,16,fullscreen))
{
return 0;						// Quit If Window Was Not Created
}
}*/
/*		}
}

// Shutdown
KillGLWindow();									// Kill The Window
return (msg.wParam);							// Exit The Program
}
*/
