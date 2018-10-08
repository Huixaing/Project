#include "stdafx.h"
#include <io.h>
#include <stdio.h>
#include <direct.h>
#include "winspool.h"
#include "tg.h"
#include "sysshare.h"
////////////////////////////////////////
//初始化对话框的位置，避免多个显示器的时候弹出的对话框在两个显示器中间
void InitDlgPosition(CWnd* dlg)
{
	if(gpcdef.GetScreenNum() == 6)
	{
		int screenx = GetSystemMetrics(SM_CXSCREEN);
		int screeny = GetSystemMetrics(SM_CYMAXIMIZED);

		CRect dlgRect;

		dlg->GetWindowRect(&dlgRect);

		int xpos = 0;
		int ypos = 0;
		//xpos = (screenx/2 - dlgRect.Width())/2;
		//xpos = screenx/2 + (screenx/2 - dlgRect.Width())/2;
		xpos = (screenx/6 - dlgRect.Width())/2;
		ypos = (screeny - dlgRect.Height())/2;

		dlg->SetWindowPos(&CWnd::wndTop,xpos,ypos,0,0,SWP_NOSIZE);

	}
	if(gpcdef.GetScreenNum() == 2)
	{
		int screenx = GetSystemMetrics(SM_CXSCREEN);
		int screeny = GetSystemMetrics(SM_CYMAXIMIZED);

		CRect dlgRect;

		dlg->GetWindowRect(&dlgRect);

		int xpos = 0;
		int ypos = 0;

		xpos = (screenx/2 - dlgRect.Width())/2;
		ypos = (screeny - dlgRect.Height())/2;

		dlg->SetWindowPos(&CWnd::wndTop,xpos,ypos,0,0,SWP_NOSIZE);

	}
	else
	{
		int screenx = GetSystemMetrics(SM_CXSCREEN);
		int screeny = GetSystemMetrics(SM_CYMAXIMIZED);

		CRect dlgRect;

		dlg->GetWindowRect(&dlgRect);

		int xpos = 0;
		int ypos = 0;

		xpos = (screenx - dlgRect.Width())/2;
		ypos = (screeny - dlgRect.Height())/2;

		dlg->SetWindowPos(&CWnd::wndTop,xpos,ypos,0,0,SWP_NOSIZE);
	}
}
BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode)
{
	// if NULL is passed, then assume we are setting app object's
    // devmode and devnames
    if (phDevMode == NULL || phDevNames == NULL)
        return FALSE;
	
    // Open printer
    HANDLE hPrinter;
    if (OpenPrinter(pszPrinterName, &hPrinter, NULL) == FALSE)
        return FALSE;
	// obtain PRINTER_INFO_2 structure and close printer
    DWORD dwBytesReturned, dwBytesNeeded;
    GetPrinter(hPrinter, 2, NULL, 0, &dwBytesNeeded);
    PRINTER_INFO_2* p2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR,
        dwBytesNeeded);
    if (GetPrinter(hPrinter, 2, (LPBYTE)p2, dwBytesNeeded,
		&dwBytesReturned) == 0) {
		GlobalFree(p2);
		ClosePrinter(hPrinter);
		return FALSE;
    }
    ClosePrinter(hPrinter);
	
	if (!OpenPrinter(pszPrinterName, &hPrinter, NULL))
		return FALSE ;
	int nSize = DocumentProperties(NULL, hPrinter, pszPrinterName, NULL, NULL, 0);
	ASSERT(nSize >= 0);
	HGLOBAL local_hDevMode = ::GlobalAlloc(GHND, nSize) ;	// allocate on heap
	LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(local_hDevMode);// lock it
	if (DocumentProperties(NULL, hPrinter, pszPrinterName, lpDevMode, NULL, DM_OUT_BUFFER) != IDOK)
	{
	  ASSERT(::GlobalFlags(local_hDevMode) != GMEM_INVALID_HANDLE);
	  UINT nCount = ::GlobalFlags(local_hDevMode) & GMEM_LOCKCOUNT;
	  while (nCount--)
	  ::GlobalUnlock(local_hDevMode);
	  ::GlobalFree(local_hDevMode);
	  local_hDevMode = NULL;
	  GlobalFree(p2);
	  ClosePrinter(hPrinter);
	  return FALSE ;
	}
	::GlobalUnlock(local_hDevMode) ;
	ClosePrinter(hPrinter);
	  
    // Compute size of DEVNAMES structure from PRINTER_INFO_2's data
    DWORD drvNameLen = lstrlen(p2->pDriverName)+1;  // driver name
    DWORD ptrNameLen = lstrlen(p2->pPrinterName)+1; // printer name
    DWORD porNameLen = lstrlen(p2->pPortName)+1;    // port name
	
    // Allocate a global handle big enough to hold DEVNAMES.
    HGLOBAL hDevNames = GlobalAlloc(GHND,
        sizeof(DEVNAMES) +
        (drvNameLen + ptrNameLen + porNameLen)*sizeof(TCHAR));
    ASSERT(hDevNames);
    DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(hDevNames);
    ASSERT(pDevNames);
	
    // Copy the DEVNAMES information from PRINTER_INFO_2
    // tcOffset = TCHAR Offset into structure
    int tcOffset = sizeof(DEVNAMES)/sizeof(TCHAR);
    ASSERT(sizeof(DEVNAMES) == tcOffset*sizeof(TCHAR));
	
    pDevNames->wDriverOffset = tcOffset;
    memcpy((LPTSTR)pDevNames + tcOffset, p2->pDriverName,
        drvNameLen*sizeof(TCHAR));
    tcOffset += drvNameLen;
	
    pDevNames->wDeviceOffset = tcOffset;
    memcpy((LPTSTR)pDevNames + tcOffset, p2->pPrinterName,
        ptrNameLen*sizeof(TCHAR));
    tcOffset += ptrNameLen;
	
    pDevNames->wOutputOffset = tcOffset;
    memcpy((LPTSTR)pDevNames + tcOffset, p2->pPortName,
        porNameLen*sizeof(TCHAR));
    pDevNames->wDefault = 0;
	
    GlobalUnlock(hDevNames);
    GlobalFree(p2);   // free PRINTER_INFO_2
	
    // set the new hDevMode and hDevNames
    *phDevMode = local_hDevMode;
    *phDevNames = hDevNames;
    return TRUE;
} 

//根据文件全路径创建需要的目录
/*********************************************
char* filename //格式如 c:\temp\test\1.txt
				//c:\temp\test\
*******************************************/
bool CreateRecursiveDir(const char* filename)
{
	if(strlen(filename)>MAX_PATH)
		return false;

	char filepath[MAX_PATH+1];
	char newfilename[MAX_PATH+1];

	memset(filepath,0,sizeof(filepath));
	memset(newfilename,0,sizeof(newfilename));
	strcpy(newfilename,filename);

	//replace the / 
	int filenamelen = strlen(newfilename);
	for(int i=0; i<filenamelen;i++)
	{
		if(newfilename[i] ==  '/')
			newfilename[i] = '\\';
	}
	
	char* string = newfilename;
	char seps[]   = "\\";	
	char *token;
	
	//去掉尾部的文件名字
	char* rdelim;
	rdelim = strrchr(string,'\\');
	
	if(rdelim != NULL)
	{
		*rdelim = '\0';
	}
	else
	{
		return true;
	}
	
	if(strlen(string)<2)
		return false;
	
	//for network address create, exp. \\172.20.48.51\sharedocs\
	//copy the first two character,
	if(strncmp(string,"\\\\",2) == 0)
	{
		strncpy(filepath,string,2);
		filepath[2] = '\0';
		string += 2;
	}
	else
	{
		strncpy(filepath,string,3);
		filepath[3] = '\0';
		string += 3;
	}


	/* Establish string and get the first token: */
	token = strtok( string, seps );
	while( token != NULL )
	{
		/* While there are tokens in "string" */
	//	printf( " %s\n", token );
		//判断该目录是否已经存在，如果不存在，建立
		strcat(filepath,token);
		strcat(filepath,"\\.");
		/* Check for existence */
		if( (_access(filepath, 0)) != -1 )//存在
		{
			filepath[strlen(filepath)-1]='\0';
			
		}
		else
		{
			filepath[strlen(filepath)-1]='\0';
			mkdir(filepath);
		}
		/* Get next token: */
		token = strtok( NULL, seps );
	}
	
	
	return true;
}

//清空某个目录
BOOL EmptyDirectory(CString destdir)
{
	BOOL flag;
	CFileFind fileFind;
	char sourcePath[MAX_PATH+1];
	if(destdir.GetLength() > MAX_PATH || destdir.GetLength() <=0 )
		return FALSE;	
	
	strcpy(sourcePath,(char*)(LPCSTR)destdir);
	
	if(sourcePath[strlen(sourcePath)-1] == '\\') //末尾有"\"
		strcat(sourcePath,"*.*");
	else
		strcat(sourcePath,"\\*.*");
	
	
	flag = fileFind.FindFile(sourcePath);
	
	CString filepath;
	while(flag)
	{
		flag = fileFind.FindNextFile();
		
		if(fileFind.IsDots())
			continue;
		
		filepath = fileFind.GetFilePath();
		if(fileFind.IsDirectory())
		{	
			if(!EmptyDirectory(filepath))
			{
				fileFind.Close();
				return FALSE;
			}
			rmdir(filepath);
		}
		else
		{			
			DeleteFile(filepath);
		}		
		
	}
	fileFind.Close();
	
	return TRUE;
}
//拷贝整个目录 
BOOL CopyDirectory (char* source,char* dest,BOOL nooverwrite)
{
	BOOL flag;
	CFileFind fileFind;
	char sourcePath[MAX_PATH+1],destPath[MAX_PATH+1];
	if(strlen(source)>MAX_PATH || strlen(dest)>MAX_PATH)
		return FALSE;
	
	if(strlen(source)<=0 || strlen(dest)<=0)
		return FALSE;
	
	strcpy(sourcePath,source);
	strcpy(destPath,dest);
	
	if(sourcePath[strlen(sourcePath)-1] == '\\') //末尾有"\"
		strcat(sourcePath,"*.*");
	else
		strcat(sourcePath,"\\*.*");

	if(destPath[strlen(destPath)-1] == '\\') //末尾有"\"
		;
	else
		strcat(destPath,"\\");

	
	
	if(!CreateRecursiveDir(destPath))
		return FALSE;
	
	flag = fileFind.FindFile(sourcePath);
	
	CString filepath;
	while(flag)
	{
		flag = fileFind.FindNextFile();
		
		if(fileFind.IsDots())
			continue;
		
		filepath = fileFind.GetFilePath();
		if(fileFind.IsDirectory())
		{	
			strcpy(destPath,dest);	
			if(destPath[strlen(destPath)-1] == '\\') //末尾有"\"
				;
			else
				strcat(destPath,"\\");
			strcat(destPath,(char*)(LPCTSTR)fileFind.GetFileTitle());
			strcat(destPath,"\\");
			
			if(!CopyDirectory((char*)(LPCTSTR)filepath,destPath,nooverwrite))
			{
				fileFind.Close();
				return FALSE;
			}
		}
		else
		{
			strcpy(destPath,dest);
			if(destPath[strlen(destPath)-1] == '\\') //末尾有"\"
				;
			else
				strcat(destPath,"\\");
			strcat(destPath,(char*)(LPCTSTR)fileFind.GetFileName());
			
			CopyFile((char*)(LPCTSTR)filepath,destPath,nooverwrite);			
			
		}		
		
	}
	fileFind.Close();
	
	return TRUE;

}

int DrawTextInRect(CDC *pDC,const char *str,RECT *prect,int coltap,int rowtap,BYTE type)
{
	char substr[4];
	int x,y;
	CSize size;
        size.cx=16;size.cy=16;
	x=prect->left;
	y=prect->top;
	pDC->SetTextAlign(0); 
	while(*str){
		if(*str==10){  // ignore linefeed
     		str++; 
			continue;
        }else if(*str==13){ // return
			if(type == 0){
				x=prect->left;
				y+=size.cy+coltap*size.cy/10;
			}
			str++;
            continue;
        }
		if((BYTE)*str>0x80){
			substr[0]=*str++;
			substr[1]=*str++;
			substr[2]=0;
		}else{
			substr[0]=*str++;
			substr[1]=0;
		}
		size=pDC->GetTextExtent(substr,strlen(substr));
		if((x+size.cx)>prect->right){
			x=prect->left;
			y+=size.cy+coltap*size.cy/10;
		}
		if(y+size.cy>prect->bottom)  // CUIHU,
			break;
		pDC->TextOut(x,y,substr,strlen(substr));
		x+=size.cx+size.cx*coltap/(10*strlen(substr));
	}
	return 0;
}

