#include "main.h"
#include "mrea.h"

sCombo::~sCombo()
{
	Clear();
}
sCombo::sCombo()
{
	cClear();
}

int             sCombo::Init(HWND sComboHwnd)
{
	hCombo = sComboHwnd;
	Clear();
	return 0;
}
int             sCombo::Disable()
{
	EnableWindow(hCombo, 0);
	return 0;
}
int             sCombo::Enable()
{
	EnableWindow(hCombo, 1);
	return 0;
}
int             sCombo::cClear()
{
	hCombo = NULL;
	return 0;
}
int             sCombo::Clear()
{
	SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
	return 0;
}
long            sCombo::GetListIndex()
{
	return SendMessage(hCombo, CB_GETCURSEL, 0, 0);
}

int             sCombo::SetListIndex(int Value)
{
	SendMessage(hCombo, CB_SETCURSEL, Value, 0);
	return 0;
}
int             sCombo::sList(int index)
{
	SendMessage(hCombo, CB_GETITEMDATA, index, 0);
	return 0;
}
int             sCombo::Click()
{
	return 0;
}

int             sCombo::Additem(char *string)
{
	SendMessage(hCombo, CB_ADDSTRING, 0, (long) string);
	return 0;
}

HWND            sCombo::thishwnd()
{
	return hCombo;
}


sChecks::~sChecks()
{
	cClear();
}
sChecks::sChecks()
{
	cClear();
}

int             sChecks::cClear()
{
	check = NULL;
	return 0;
}

int             sChecks::SetCnt(HWND hwnd)
{
	check = hwnd;
	return 0;
}

int             sChecks::value()
{

	return SendMessage(check, BM_GETCHECK, 0, 0);
}

int             sChecks::value(u8 Val)
{
	if (Val < 3)
	{
		SendMessage(check, BM_SETCHECK, Val, 0);
	} else
	{
		MessageBox(0,(LPCSTR)"Bad checkbox value",(LPCSTR)"Default",MB_OK);
	}
	return 0;
}

int             sChecks::State(bool flag)
{
	EnableWindow(check, flag);
	return 0;
}

int             sChecks::State()
{

	return IsWindowEnabled(check);
}



int             stxts::SetHwnd(HWND hwnd)
{
	thisHwnd = hwnd;
	return 0;
}
int             stxts::GetText()
{

	int             val = 0;
	char            buf[100];

	GetWindowText(thisHwnd, (LPSTR)buf, 4);
	sscanf(buf, "%d", &val);
	return val;
}

int             stxts::SetText(char *string)
{
	SetWindowText(thisHwnd, (LPCSTR)string);
	return 0;
}






sList::~sList()
{
	Clear();
}
sList::sList()
{
	cClear();
}
int             sList::Click(){
	//SendMessage(GetParent(hCombo),WM_COMMAND,lstMatches, 0);


	return 0;
}
int             sList::Init(HWND sListHwnd)
{
	hCombo = sListHwnd;
	Clear();
	return 0;
}
int             sList::Disable()
{
	EnableWindow(hCombo, 0);
	return 0;
}
int             sList::Enable()
{
	EnableWindow(hCombo, 1);
	return 0;
}
int             sList::cClear()
{
	hCombo = NULL;
	return 0;
}
int             sList::Clear()
{
	SendMessage(hCombo, LB_RESETCONTENT, 0, 0);
	return 0;
}
long            sList::GetListIndex()
{
	return SendMessage(hCombo, LB_GETCURSEL, 0, 0);
}

int             sList::SetListIndex(int Value)
{
	SendMessage(hCombo, LB_SETCURSEL, Value, 0);
	return 0;
}
int             sList::sListStrings(int index)
{
	SendMessage(hCombo, LB_GETITEMDATA, index, 0);
	return 0;
}
int      sList::InsertString(char * string){
	int id=GetListIndex();

	SendMessage(hCombo, LB_DELETESTRING , id, (long) string);

	SendMessage(hCombo, LB_INSERTSTRING , id, (long) string);


	return 0;
}
int      sList::InsertString(HWND hDlg, int id, char * string){

	SendDlgItemMessage(hDlg, id, LB_SETITEMDATA, id, (LPARAM)string);


	return 0;
}
int             sList::Additem(char *string)
{
	SendMessage(hCombo, LB_ADDSTRING, 0, (long) string);
	return 0;
}
int             sList::Additem(const char *string)
{
	SendMessage(hCombo, LB_ADDSTRING, 0, (long) string);
	return 0;
}

HWND            sList::thishwnd()
{
	return hCombo;
}



int sTree::Init(HWND tWND){
	InitCommonControls();	    // make our tree control to work
	hTree=tWND;
	pNodes.nParents.resize(1);
	return 0;
}
int cParents::AddParentNode(std::string pszName, HTREEITEM hParent){//hacky
	std::vector<sParents> tParents;
	int i=0;
	tParents.resize(nParents.size()+1);
	for(i=0;i<nParents.size();i++){
		memcpy(&tParents[i].Parent,&nParents[i].Parent,sizeof(HTREEITEM));
		tParents[i].pName = nParents[i].pName;
	}
	tParents[nParents.size()].Parent = hParent;
	tParents[nParents.size()].pName = pszName;
	nParents.resize(tParents.size());
	for(i=0;i<tParents.size();i++){
		memcpy(&nParents[i].Parent,&tParents[i].Parent,sizeof(HTREEITEM));
		nParents[i].pName = tParents[i].pName;
	}


	return 0;
}
HTREEITEM* cParents::CheckParentNode(std::string pName){
	HTREEITEM* hti=NULL;
	int i=0;
	for(i=0;i<nParents.size();i++){
		if(pName.compare(nParents[i].pName) == 0){

			return &nParents[i].Parent;
		}
	}
	return hti;
}
HWND hMain;
int sTree::AddParent(HTREEITEM sParent, HTREEITEM InsertAfter,char* text){


	tvinsert.hParent=NULL;			// top most level no need handle
	tvinsert.hInsertAfter=TVI_ROOT; // work as root level
	tvinsert.item.pszText=text;
	tvinsert.item.mask=TVIF_TEXT;
	Root=Parent=(HTREEITEM)SendMessage(hTree,TVM_INSERTITEM,0,(LPARAM)&tvinsert);


	return 0;
}
int sTree::AddNode(char* Text){



	tvinsert.hParent=Parent;
	tvinsert.hParent=Root;			// top most level no need handle
	tvinsert.hInsertAfter=TVI_LAST; // work as root level
	tvinsert.item.pszText=Text;
	Parent=(HTREEITEM)SendMessage(hTree,TVM_INSERTITEM,0,(LPARAM)&tvinsert);

	return 0;
}
int sTree::AddChild(char* text){
	tvinsert.hParent=Parent;
	tvinsert.hInsertAfter=TVI_LAST; // work as root level
	tvinsert.item.pszText=text;
	Parent=(HTREEITEM)SendMessage(hTree,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
	TreeView_SelectDropTarget(hTree,Parent);
	return 0;
}

cParents::cParents(){


}
cParents::~cParents(){

}
sTree::sTree(){
	flagSelected=false;
}
sTree::~sTree(){
}
int IsSupported(char* Text){
	int i=0;
	int j=0;
	int k=0;
	char *Supported[4]={"CMDL","TXTR","CINF", "MREA"};
	//char *Supported[3]={"CMDL","CINF"};
	for(i=0;i<4;i++)
	{
		k=0;
		for(j=0;j<strlen(Supported[i]);j++)
		{
			if(Supported[i][j]==Text[j])
			{
				k++;
			}
		}
		if(k==4){
			return i;
		}
	}
	return 0xFF;
}

int LoadFile(char* File){
	HANDLE hFind;
	WIN32_FIND_DATA ffd;
	BOOL bFound = TRUE;
	string filestr;
	int TreeCount=0;
	char Dir[2048]={0};
	char buff[2048]={0};
	int i=0;
	int Decomp=0;
	char ext[5]={0};
	//Call the deconstructot XD JUST IN CASE You know



	//SendMessage(lstFiles.thishwnd(), LB_GETTEXT, (WPARAM)lstFiles.GetListIndex(), (LPARAM)buff);
	if(File[0]=='1'){//We need to decomp this
		Decomp=1;

	}


	OutputDebugString(File);
	OutputDebugString("\n");
	filestr=strCurDir+"\\"+File;
	if(filestr.size()==0){
		MessageBox(0,"OH SHIT MAN","ERROR",MB_OK);
	}
	if(Decomp==1){
		memset(buff,0,2048);
		decompFile(filestr.c_str(),buff);
		filestr=buff;
	}
	switch(FileType){
			case 0:
				xRot=-90;
				yRot=0;
				zRot=0;
				xCamera=0.0f;
				yCamera=0;//-0.8f;
				zCamera=-5.0;//-2.6f;
				MREA.~MREAData();
				CMDL.~cmdlContainment();
				CMDL.Read((char*)filestr.c_str());
				break;
			case 1://Not supported yet TXTR
				xRot=0;
				yRot=0;
				zRot=0;
				xCamera=0.0f;
				yCamera=0;//-0.8f;
				zCamera=-5.0;//-2.6f;
				glDeleteTextures(1,&g_textureID);
				LoadsTXTR((char*)filestr.c_str());
				break;
			case 2://Not supported yet
				xRot=-90;
				yRot=0;
				zRot=0;
				xCamera=0.0f;
				yCamera=0;//-0.8f;
				zCamera=-5.0;//-2.6f;
				ReadCINF((char*)filestr.c_str());
				break;
			case 3:
				xRot=-90;
				yRot=0;
				zRot=0;
				xCamera=0.0f;
				yCamera=0;//-0.8f;
				zCamera=-10.0;//-2.6f;
				CMDL.~cmdlContainment();
				MREA.~MREAData();
				MREA.Read((char*)filestr.c_str());
				break;
	}
	sprintf(buff,"X Axis rotation:\n%f",xRot);
	SetWindowText(GetDlgItem(hwndMain,ID_XAX_ROT),buff);
	sprintf(buff,"Y Axis rotation:\n%f",yRot);
	SetWindowText(GetDlgItem(hwndMain,ID_YAX_ROT),buff);
	sprintf(buff,"Z Axis rotation:\n%f",zRot);
	SetWindowText(GetDlgItem(hwndMain,ID_ZAX_ROT),buff);
	DrawGLScene();
	InvalidateRect(hwndGL,0,1);
	//Delete decomped file
	if(Decomp==1){
		DeleteFile(filestr.c_str());

		Decomp=0;
	}
	return 0;

}
struct Vertex
{
	float tu, tv;
	float x, y, z;
};

Vertex g_quadVertices[] =
{
	{ 0.0f,0.0f, -1.0f,-1.0f, 0.0f },
	{ 1.0f,0.0f,  1.0f,-1.0f, 0.0f },
	{ 1.0f,1.0f,  1.0f, 1.0f, 0.0f },
	{ 0.0f,1.0f, -1.0f, 1.0f, 0.0f }
};

int LoadsTXTR(char* FileName){//Load Single Texture

	LoadTXTR(std::string(FileName),&g_textureID );
	return 0;

}
