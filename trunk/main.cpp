//#include <vld.h>
#include "main.h"
 	
BOOL CALLBACK   DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HANDLE hFind;
	WIN32_FIND_DATA ffd;
	BOOL bFound = TRUE;
	string filestr;
	u32 TreeCount=0;
	char Dir[2048]={0};
	string CurDir;
	char buff[2048]={0};
	u32 i=0;
	int Decomp=0;
	char ext[5]={0};
	char num[]="1";
	switch (message)
	{

	case WM_INITDIALOG:
		hwndMain=hwnd;
		SetWindowText(GetDlgItem(hwnd,ID_XRS),num);
		SetWindowText(GetDlgItem(hwnd,ID_YRS),num);
		SetWindowText(GetDlgItem(hwnd,ID_ZRS),num);

		SetWindowText(GetDlgItem(hwnd,ID_XCS),num);
		SetWindowText(GetDlgItem(hwnd,ID_YCS),num);
		SetWindowText(GetDlgItem(hwnd,ID_ZCS),num);
		CreateGLWindow("Metroid Stuff",640,480,32,0);

		DrawGLScene();
		ThisTree.Init(GetDlgItem(hwnd,ID_TREE_LIST));
		//ThisTree.AddParent(NULL,TVI_ROOT,"Files");
		InvalidateRect(hwndGL,0,1);/*Set up controlers*/
		lstFiles.Init(GetDlgItem(hwndMain,ID_lstFiles2));
		lstTextures.Init(GetDlgItem(hwndMain,ID_lstTextures));
		lstMaterials.Init(GetDlgItem(hwndMain,ID_LIST_MATERIALSET));
		//SetWindowText(GetDlgItem(hwnd,ID_txtFolders),"F:\\Users\\matt\\Desktop\\MetroidPrime3Dir\\Metroid8.pak_dir");
		SetWindowText(GetDlgItem(hwnd,ID_txtFolders),"C:\\Users\\Matt\\Desktop\\Desktop\\Desktop\\MetroidPrime1Dir\\Metroid1.pak_dir");
		//SetWindowText(GetDlgItem(hwnd,ID_txtFolders),"C:\\Documents and Settings\\Administrator\\Desktop\\MetroidPrime2Dir\\Metroid1.pak_dir");
		//SetWindowText(GetDlgItem(hwnd,ID_txtFolders),"F:\\Users\\matt\\Desktop\\MetroidPrime1Dir\\Metroid1.pak_dir");// for inter
		//SetWindowText(GetDlgItem(hwnd,ID_txtFolders),"C:\\Projects\\emulation\\mp\\extract");
		//SetWindowText(GetDlgItem(hwnd,ID_txtFolders),"C:\\Projects\\emulation\\mp\\extract\\Metroid1.pak_dir");//for revl8n
		//SetWindowText(GetDlgItem(hwnd,ID_txtFolders),"C:\\Projects\\emulation\\mp\\extract\\Metroid2.pak_dir");
		//SetWindowText(GetDlgItem(hwnd,ID_txtFolders),"C:\\Projects\\emulation\\mp\\extract\\Metroid6.pak_dir");
		//SetWindowText(GetDlgItem(hwnd,ID_txtFolders),"C:\\Projects\\emulation\\mp\\extract\\Metroid8.pak_dir");
		break;
	case BN_CLICKED:

		break;
	case WM_PAINT:

		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
	case ID_LIST_MATERIALSET:
		if(HIWORD(wParam)==CBN_SELCHANGE){
			CMDL.ReadTextures();
		}
		break;


	case ID_XAX_MINUS:
		xRot-=1.0f;
		sprintf(buff,"X Axis rotation:\n%f",xRot);
		SetWindowText(GetDlgItem(hwndMain,ID_XAX_ROT),buff);
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_YAX_MINUS:
		yRot-=1.0f;
		sprintf(buff,"Y Axis rotation:\n%f",yRot);
		SetWindowText(GetDlgItem(hwndMain,ID_YAX_ROT),buff);
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_ZAX_MINUS:
		zRot-=1.0f;
		sprintf(buff,"Z Axis rotation:\n%f",zRot);
		SetWindowText(GetDlgItem(hwndMain,ID_ZAX_ROT),buff);
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_XAX_PLUS:
		xRot+=1.0f;
		sprintf(buff,"X Axis rotation:\n%f",xRot);
		SetWindowText(GetDlgItem(hwndMain,ID_XAX_ROT),buff);
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_YAX_PLUS:
		yRot+=1.0f;
		sprintf(buff,"Y Axis rotation:\n%f",yRot);
		SetWindowText(GetDlgItem(hwndMain,ID_YAX_ROT),buff);
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_ZAX_PLUS:
		zRot+=1.0f;
		sprintf(buff,"Z Axis rotation:\n%f",zRot);
		SetWindowText(GetDlgItem(hwndMain,ID_ZAX_ROT),buff);
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_CAMERA_UP:
		yCamera+=0.2F;
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_CAMERA_DOWN:
		yCamera-=0.2F;
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_CAMERA_LEFT:
		xCamera-=0.2F;
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_CAMERA_RIGHT:
		xCamera+=0.2F;
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_CAMERA_IN:
		zCamera-=0.2F;
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;
	case ID_CAMERA_OUT:
		zCamera+=0.2F;
		DrawGLScene();
		InvalidateRect(hwndGL,0,1);
		break;


	case WM_DESTROY:
		EndDialog(hwndMain,0);

		break;

	case ID_cmdLoad:

		TreeCount=TreeView_GetCount(GetDlgItem(hwnd,ID_TREE_LIST));
		for( i=0;i<TreeCount;i++){
			TreeView_DeleteAllItems(GetDlgItem(hwnd,ID_TREE_LIST));
		}
		ThisTree.AddParent(NULL,TVI_ROOT,"Files");
		GetWindowText(GetDlgItem(hwndMain,ID_txtFolders),Dir,2048);
		CurDir=Dir;
		strCurDir=Dir;
		CurDir+="\\*.*";
		hFind = FindFirstFile(CurDir.c_str(), &ffd);
		while ((TRUE == bFound) && (hFind != INVALID_HANDLE_VALUE)){//This is the first pass where we create nodes
			if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
				memcpy(ext,&ffd.cFileName[strlen(ffd.cFileName)-4],4);
				//we have the extension
				if(ThisTree.pNodes.CheckParentNode(std::string(ext))==NULL){
					ThisTree.AddNode(ext);
					ThisTree.pNodes.AddParentNode(ext,ThisTree.Parent);


				}
			}
			bFound = FindNextFile(hFind, &ffd);

		}

		FindClose(hFind);

		for(i=1;i<ThisTree.pNodes.nParents.size();i++){//ThisTree.pNodes.nParents.size();
			CurDir=Dir;
			CurDir+="\\*.";
			CurDir+=ThisTree.pNodes.nParents[i].pName;

			hFind = FindFirstFile(CurDir.c_str(), &ffd);
			bFound=TRUE;
			while ((TRUE == bFound) && (hFind != INVALID_HANDLE_VALUE)){//This is the first pass where we create nodes
				if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
					ThisTree.Parent = ThisTree.pNodes.nParents[i].Parent;
					ThisTree.AddChild(ffd.cFileName);
				}
				bFound = FindNextFile(hFind, &ffd);

			}

			FindClose(hFind);
		}
		UpdateWindow(GetDlgItem(hwnd,ID_TREE_LIST));
		TreeView_SelectDropTarget(ThisTree.hTree,ThisTree.Root);
		break;	
		}


	case WM_VSCROLL: // exact same idea, but V scroll instead of H scroll

		break;
	case WM_HSCROLL:

		break;
	case WM_LBUTTONDOWN:


		break;



	case WM_NOTIFY:
		{
	case ID_TREE_LIST:

		if(((LPNMHDR)lParam)->code ==  NM_DBLCLK ) // if code == NM_CLICK - Single click on an item
		{
			char Text[255]="";
			memset(&ThisTree.tvi,0,sizeof(ThisTree.tvi));

			ThisTree.Selected=(HTREEITEM)SendDlgItemMessage(hwnd,ID_TREE_LIST,TVM_GETNEXTITEM,TVGN_CARET,(LPARAM)ThisTree.Selected);

			if(ThisTree.Selected==NULL)
			{
				MessageBox(hwnd,"No Items in TreeView","Error",MB_OK|MB_ICONINFORMATION);
				break;
			}
			TreeView_EnsureVisible(hwnd,ThisTree.Selected);
			SendDlgItemMessage(hwnd,ID_TREE_LIST,TVM_SELECTITEM,TVGN_CARET,(LPARAM)ThisTree.Selected);
			ThisTree.flagSelected=true;
			ThisTree.tvi.mask=TVIF_TEXT;
			ThisTree.tvi.pszText=Text;
			ThisTree.tvi.cchTextMax=256;
			ThisTree.tvi.hItem=ThisTree.Selected;

			if(SendDlgItemMessage(hwnd,ID_TREE_LIST,TVM_GETITEM,TVGN_CARET,(LPARAM)&ThisTree.tvi))
			{
				if(Text[strlen(Text)-5]=='.'){


					if(IsSupported(&Text[strlen(Text)-4])!=0xFF){//Will return what to use. 
						string tf="Current File:";
						tf=+Text;
						SetWindowText(GetDlgItem(hwndMain,ID_CURFILE),tf.c_str());
						FileType=IsSupported(&Text[strlen(Text)-4]);

						LoadFile(Text);
					}else{
						MessageBox(hwnd,"FILE IS NOT SUPPORTED","NULL",MB_OK);
					}
				}
			}
		}






		}
		break;
	}
	return 0;
}


int WINAPI      WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	hGlobal = hInstance;
	ArcBall=new ArcBallT(640.0f, 480.0f);	
	DialogBox(hInstance, MAKEINTRESOURCE(ID_frmMain), 0, DialogProc);
	delete ArcBall;
	return 0;
}
