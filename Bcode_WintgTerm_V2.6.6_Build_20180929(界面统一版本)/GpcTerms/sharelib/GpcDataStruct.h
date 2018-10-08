
#if !defined(__GpcDataStruct_H__)
#define __GpcDataStruct_H__

template<class T> class GpcDataTmpl;

#define GPC_STATION			("[STATION]")	
struct GPCSTATION 
{
	//int     nId;//lyc
	int		nStation;
	int		nSection;
	//int		nDispPage;
	SIZE	sizeDisp;
	USHORT	nWidth,nHeight;
	float	fXScale,fYScale;
	USHORT	nDispWidth,nDispHeight;
	USHORT  nXPosition,nYPosition;
	int		nDispSection;
	char	szStationName[32];
	int		nStationSideNum;
	char	fStaDistance[64];
    bool    bDrawSides;
	bool    bDrawTrainTime;//draw train arrive and depart time;
	int     nStationClass;

	int  	nProperty;   //车站性质:-1, 无设备, 0,TDCS, 1, CTC
	char   szRtServerName[32];
	
	GPCSTATION()
	{
		memset(this, 0, sizeof(GPCSTATION));
	}
	int getID(){ return nStation; }
	int loadContent(char *ptr);
};
typedef GpcDataTmpl<GPCSTATION> gpcSTATION;


#define GPC_DIRECTIONSTRING	("[DIRECTIONSTRING]")	//wsg Aug 13, 2002
typedef struct tagGPCDIRECTIONSTRING	//wsg Aug 13, 2002
{
	int	iSectionID;
	int iStationID;
	CString sDirString;

	int getID() { return 0; }
	int loadContent(char *ptr);
}GPCDIRECTIONSTRING;
typedef GpcDataTmpl<GPCDIRECTIONSTRING> gpcDIRECTIONSTRING;

#define GPC_SIGNALRADIUS	("[SIGNALRADIUS]")
#define GPC_LINETHICKNESS	("[LINETHICKNESS]")


// added by wsg for SdLayout.dll
#define GPC_PAGE               ("[PAGE]")
typedef struct tagGPCPAGE {
  int  nPageNO;
  int  nStationNum;
  int  nStationList[48];
  int  nNeighbourNum;
  int  nNeighbourList[24];

  int getID(){ return nPageNO; }
	int loadContent(char *ptr);
} GPCPAGE;
typedef GpcDataTmpl<GPCPAGE> gpcPAGE;


#include <vector>
#define GPC_PURETEXTSTATION		("[PURETEXTSTATION]")
struct  CPureTextStation
{
public:
	int loadContent(char *ptr)
	{
		char *token;
		char seps[]=" ,\t\\/=\n";

	//	[PURETEXTSTATION] = 1/1
		token = strtok(ptr, seps);
		if(token==NULL) return 0;
		for(int i=atoi(token); i>0; i--)
		{
			token = strtok(NULL, seps);
			if(token==NULL) return 0;
			stationList.push_back(atoi(token));
		}
		return 1;
	}
	BOOL operator()(int  id)
	{
		for(size_t i=0; i<stationList.size(); i++)
		{
			if(id == stationList[i])
				return  TRUE;
		}
		return  FALSE;
	}
private:
	std::vector<int>   stationList;
};



// 默认AIX高字节优先
// 默认高字节优先系统为AIX
#define GPC_SERVER_OS		("[SERVER_OS]")
struct  CIsHighByteFirst
{
	CIsHighByteFirst() : bIsHBF(FALSE) {}
	int loadContent(char *ptr)
	{
		char *token;
		char seps[]=" ,\t\\/=\n";

		//[SERVER_OS] = AIX or OS2
		token = strtok(ptr, seps);
		if(token==NULL) return 0;
		if(_stricmp(token, "AIX") == 0)
			bIsHBF = TRUE;
		else if(_stricmp(token, "OS2") == 0)
			bIsHBF = FALSE;
		else
			bIsHBF = FALSE;

		return 1;
	}
	BOOL  operator()()
	{	return  bIsHBF;	}
private:
	BOOL	bIsHBF;
};



#define GPC_TMIS_SERVER_NAME		("[TMIS_SERVER_NAME]")
#define GPC_DAPING_SERVER		("[DAPING_SERVER]")
#define GPC_WINVNC_OPTION		("[WINVNC_OPTION]")
struct  CDapingServer
{
	CDapingServer() {}
	int loadContent(char *ptr)
	{
		//char *token;
		//char seps[]="=\r\n";

		//[DAPING_SERVER] = 172.26.19.111
		//token = strtok(ptr, seps);
		//if(token==NULL) return 0;
		//ASSERT(strlen(token) <= 15);
		//strcpy(addr, token);
		addr = ptr;
		addr.Replace("=", "");
		addr.Replace("\n", "");
		addr.TrimLeft();
		addr.TrimRight();
		return 1;
	}
	const char*  operator()()
	{	return  (LPCTSTR)addr;	}
private:
	CString	addr;
};

#endif	//__GpcDataStruct_H__
