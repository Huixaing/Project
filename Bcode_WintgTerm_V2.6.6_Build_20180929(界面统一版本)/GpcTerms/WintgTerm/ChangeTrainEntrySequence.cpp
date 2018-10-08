// ChangeTrainEntrySequence.cpp : implementation file
//

#include "stdafx.h"
#include "oscalls.h"
#include "sysshare.h"
#include "..\wintg\resource.h"
#include "ChangeTrainEntrySequence.h"
#include "messages.h"
#include "function.h"

#define MAX_GUANWANG_ENTRY_LINK		5

extern int   send_message(ushort usProcessId, ushort usQueueHandle,
	    ushort usMsgSize,char *abBufferMsg);
int ArrayTrainSequence(struct _tagMiddleStationRunSequence *info);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MAX_ENTRY_NUMBER_PER_STATION 15

ushort usAllStationEntryLinkInfoNum = 0;
struct _tagStationEntryLink (*sAllStationEntryLinkInfo)[ ];

void OsDeInitializeAllStationEntryLinkInform( )
{
	if(usAllStationEntryLinkInfoNum)
		delete [ ] sAllStationEntryLinkInfo;
	sAllStationEntryLinkInfo = NULL;
	usAllStationEntryLinkInfoNum = 0;
}

void OsInitializeAllStationEntryLinkInform(int nAbsRuntimeInfoNum,struct _tagAbsRuntimeInform **abs_runtime_info)
{
	if(nAbsRuntimeInfoNum == 0)
		return;
	if(sAllStationEntryLinkInfo != NULL && usAllStationEntryLinkInfoNum != 0)
	{
		delete [ ] sAllStationEntryLinkInfo;
		sAllStationEntryLinkInfo = NULL;
		usAllStationEntryLinkInfoNum = 0;
	}

	sAllStationEntryLinkInfo = (struct _tagStationEntryLink (*)[ ])new char[sizeof(struct _tagStationEntryLink) * nAbsRuntimeInfoNum];
	if(sAllStationEntryLinkInfo == NULL)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG]: alloc failed in file %s at line %d",
			__FILE__,__LINE__);
		return;
	}
	int j,nCount = 0;
	for(int i=0;i < nAbsRuntimeInfoNum;i++)
	{
		for(j=0;j < nCount;j++)
		{
			if((*sAllStationEntryLinkInfo)[j].usSourceStationNo == abs_runtime_info[i]->usFirstStationNo
//				 && (*sAllStationEntryLinkInfo)[j].usDestineStationNo == abs_runtime_info[i]->usSecondStationNo
				 && (*sAllStationEntryLinkInfo)[j].usSourceStationEntryNo == abs_runtime_info[i]->usExit
//				 && (*sAllStationEntryLinkInfo)[j].usDestineStationEntryNo == abs_runtime_info[i]->usEntry
//				 && (*sAllStationEntryLinkInfo)[j].usDirectionFlag == abs_runtime_info[i]->usDirection
				 )
				break;
		}
		if(j >= nCount)
		{
			(*sAllStationEntryLinkInfo)[nCount].usSourceStationNo = abs_runtime_info[i]->usFirstStationNo;
			(*sAllStationEntryLinkInfo)[nCount].usDestineStationNo = abs_runtime_info[i]->usSecondStationNo;
			(*sAllStationEntryLinkInfo)[nCount].usSourceStationEntryNo = abs_runtime_info[i]->usExit;
			(*sAllStationEntryLinkInfo)[nCount].usDestineStationEntryNo = abs_runtime_info[i]->usEntry;
			(*sAllStationEntryLinkInfo)[nCount].usDirectionFlag = abs_runtime_info[i]->usDirection;
			nCount += 1;

			continue;
		}
		for(j=0;j < nCount;j++)
		{
			if((*sAllStationEntryLinkInfo)[j].usSourceStationNo == abs_runtime_info[i]->usSecondStationNo 
//				&&(*sAllStationEntryLinkInfo)[j].usDestineStationNo == abs_runtime_info[i]->usFirstStationNo
				&& (*sAllStationEntryLinkInfo)[j].usSourceStationEntryNo == abs_runtime_info[i]->usEntry
//				&& (*sAllStationEntryLinkInfo)[j].usDestineStationEntryNo == abs_runtime_info[i]->usExit
//				&& (*sAllStationEntryLinkInfo)[j].usDirectionFlag == abs_runtime_info[i]->usDirection
				)
				break;
		}
		if(j < nCount)
			continue;
		(*sAllStationEntryLinkInfo)[nCount].usSourceStationNo = abs_runtime_info[i]->usSecondStationNo;
		(*sAllStationEntryLinkInfo)[nCount].usDestineStationNo = abs_runtime_info[i]->usFirstStationNo;
		(*sAllStationEntryLinkInfo)[nCount].usSourceStationEntryNo = abs_runtime_info[i]->usEntry;
		(*sAllStationEntryLinkInfo)[nCount].usDestineStationEntryNo = abs_runtime_info[i]->usExit;
		(*sAllStationEntryLinkInfo)[nCount].usDirectionFlag = abs_runtime_info[i]->usDirection;
		nCount += 1;
	}
	usAllStationEntryLinkInfoNum = nCount;
	return;
}

ushort OsGetStationEntryDirectionFlag(ushort usStationID,
		ushort usEntryNo)
{
	for(int i=0;i < usAllStationEntryLinkInfoNum;i++)
	{
		if(usStationID == (*sAllStationEntryLinkInfo)[i].usSourceStationNo &&
			usEntryNo == (*sAllStationEntryLinkInfo)[i].usSourceStationEntryNo)
		{//查找出口
			return (*sAllStationEntryLinkInfo)[i].usDirectionFlag;
		}
		if(usStationID == (*sAllStationEntryLinkInfo)[i].usDestineStationNo &&
			usEntryNo == (*sAllStationEntryLinkInfo)[i].usDestineStationEntryNo)
		{//查找入口
			return (*sAllStationEntryLinkInfo)[i].usDirectionFlag;
		}
	}
	if(usEntryNo == 1 || usEntryNo == 3)
		return DOWN_DIRECT;
	if(usEntryNo == 4 || usEntryNo == 2)
		return UP_DIRECT;
	return -1;
}

ushort OsGetStationEntry(ushort usSourceStationNo,ushort usDestineStationNo,
						 ushort usDirFlag)
{
	for(int i=0;i < usAllStationEntryLinkInfoNum;i++)
	{
		if(usSourceStationNo == (*sAllStationEntryLinkInfo)[i].usSourceStationNo &&
			usDestineStationNo == (*sAllStationEntryLinkInfo)[i].usDestineStationNo &&
			usDirFlag == (*sAllStationEntryLinkInfo)[i].usDirectionFlag)
		{
			return (*sAllStationEntryLinkInfo)[i].usSourceStationEntryNo;
		}
	}
	for(i=0;i < usAllStationEntryLinkInfoNum;i++)
	{
		if(usSourceStationNo == (*sAllStationEntryLinkInfo)[i].usSourceStationNo &&
			usDestineStationNo == (*sAllStationEntryLinkInfo)[i].usDestineStationNo)
		{
			if(usDirFlag == UP_DIRECT && 
				( (*sAllStationEntryLinkInfo)[i].usSourceStationEntryNo % 2 == 0 ||
				(*sAllStationEntryLinkInfo)[i].usDestineStationEntryNo % 2 == 0) )
			return (*sAllStationEntryLinkInfo)[i].usSourceStationEntryNo;

			if(usDirFlag == DOWN_DIRECT && 
				( (*sAllStationEntryLinkInfo)[i].usSourceStationEntryNo % 2 == 1 ||
				(*sAllStationEntryLinkInfo)[i].usDestineStationEntryNo % 2 == 1) )
			return (*sAllStationEntryLinkInfo)[i].usSourceStationEntryNo;
		}
	}
	for(i=0;i < usAllStationEntryLinkInfoNum;i++)
	{
		if(usSourceStationNo == (*sAllStationEntryLinkInfo)[i].usSourceStationNo &&
			usDestineStationNo == (*sAllStationEntryLinkInfo)[i].usDestineStationNo)
		{
			return (*sAllStationEntryLinkInfo)[i].usSourceStationEntryNo;
		}
	}
	return -1;
}

ushort OsGetDestineStationNo(ushort usStationNo,ushort usEntryNo)
{
	for(int i=0;i < usAllStationEntryLinkInfoNum;i++)
	{
		if(usStationNo == (*sAllStationEntryLinkInfo)[i].usSourceStationNo &&
			usEntryNo == (*sAllStationEntryLinkInfo)[i].usSourceStationEntryNo)
		{
			return (*sAllStationEntryLinkInfo)[i].usDestineStationNo;
		}
	}
	return -1;
};

ushort OsGetStationEntrylinkInfo(ushort usStationNo,ushort *usEntryNum,struct _tagStationEntryLink *sEntryLinkInfo)
{
	ushort usCount = 0;

	for(int i=0;i < usAllStationEntryLinkInfoNum;i++)
	{
		if(usStationNo == (*sAllStationEntryLinkInfo)[i].usSourceStationNo)
		{
			if(sEntryLinkInfo != NULL && *usEntryNum > usCount)
			{
				sEntryLinkInfo[usCount] = (*sAllStationEntryLinkInfo)[i]; 
			}
			usCount += 1;
		}
	}
	if(usEntryNum != NULL)
		*usEntryNum = usCount;
	return usCount;
};

ushort OsGetStationEntrylinkInfo(ushort usStationNo,ushort *usEntryNum,ushort *usEntryLinkIndex)
{
	ushort usCount = 0;

	for(int i=0;i < usAllStationEntryLinkInfoNum;i++)
	{
		if(usStationNo == (*sAllStationEntryLinkInfo)[i].usSourceStationNo)
		{
			if(usEntryLinkIndex != NULL && *usEntryNum > usCount)
				usEntryLinkIndex[usCount] = i; 
			usCount += 1;
		}
	}
	if(usEntryNum != NULL)
		*usEntryNum = usCount;
	return usCount;
};

/****************************************
//following data created by load tle function
//车站入口信息由LOADTLE程序根据站场描述数据自动生成
struct _tagStationEntryLink sAllStationEntryLinkInfo[ ] =
{
		{1,1,-1,-1},
		{1,2,-1,-1},
		{1,3,1,2},
		{1,4,2,2},
		{2,1,3,1},
		{2,2,4,1},
		{2,3,1,3},
		{2,4,2,3},
		{3,1,3,2},
		{3,2,4,2},
		{3,3,1,4},
		{3,4,2,4},
		{4,1,3,3},
		{4,2,4,3},
		{4,3,1,5},
		{4,4,2,5},
		{5,1,3,4},
		{5,2,4,4},
		{5,3,1,6},
		{5,4,2,6},
		{6,1,3,5},
		{6,2,4,5},
		{6,3,-1,-1},
		{6,4,2,60},
		{7,1,-1,-1},
		{7,3,6,60},
		{7,2,-1,-1},
		{7,4,1,61},
		{7,5,-1,-1},
		{7,6,-1,-1},
		{8,1,-1,-1},
		{8,2,-1,-1},
		{8,3,1,9},
		{8,4,2,9},
		{9,1,3,8},
		{9,3,2,58},
		{9,2,4,8},
		{9,4,1,58},
		{9,5,2,10},
		{9,6,4,10},
		{10,1,3,58},
		{10,2,5,9},
		{10,4,6,9},
		{10,3,2,53},
		{10,5,1,11},
		{10,6,2,11},
		{11,1,5,10},
		{11,2,6,10},
		{11,3,1,12},
		{11,4,2,12},
		{12,1,3,11},
		{12,2,4,11},
		{12,3,2,63},
		{12,4,1,46},
		{12,5,3,13},
		{12,6,4,46},
		{12,7,4,13},
		{13,1,3,46},
		{13,2,2,46},
		{13,3,5,12},
		{13,4,7,12},
		{13,5,1,14},
		{13,6,2,14},
		{13,7,3,14},
		{13,8,4,14},
		{14,1,5,13},
		{14,2,6,13},
		{14,3,7,13},
		{14,4,8,13},
		{14,5,1,15},
		{14,6,2,15},
		{15,1,5,14},
		{15,2,6,14},
		{15,3,2,16},
		{15,4,5,59},
		{15,5,3,16},
		{15,6,3,59},
		{16,1,4,59},
		{16,2,3,15},
		{16,3,5,15},
		{16,4,6,59},
		{16,5,1,17},
		{16,6,2,18},
		{17,1,5,16},
		{17,2,1,18},
		{17,3,1,19},
		{18,1,2,17},
		{18,2,6,16},
		{18,3,2,19},
		{18,4,2,20},
		{19,1,3,17},
		{19,2,3,18},
		{19,3,1,20},
		{20,1,3,19},
		{20,2,4,18},
		{20,3,1,21},
		{20,4,2,21},
		{21,1,3,20},
		{21,2,4,20},
		{21,3,1,22},
		{21,4,2,22},
		{22,1,3,21},
		{22,2,4,21},
		{22,3,1,23},
		{22,4,2,24},
		{23,1,3,22},
		{23,2,1,24},
		{23,3,1,25},
		{24,1,2,23},
		{24,2,4,22},
		{24,3,2,25},
		{24,4,2,26},
		{25,1,3,23},
		{25,2,3,24},
		{25,3,1,26},
		{26,1,3,25},
		{26,2,4,24},
		{26,3,1,27},
		{26,4,2,27},
		{27,1,3,26},
		{27,2,4,26},
		{27,3,1,28},
		{27,4,2,28},
		{28,1,3,27},
		{28,2,4,27},
		{28,3,-1,-1},
		{28,4,-1,-1},
		{28,5,1,29},
		{28,6,2,29},
		{29,1,5,28},
		{29,2,6,28},
		{29,3,1,30},
		{29,4,2,30},
		{30,1,3,29},
		{30,2,4,29},
		{30,3,1,31},
		{30,4,2,31},
		{31,1,3,30},
		{31,2,4,30},
		{31,3,1,32},
		{31,4,2,32},
		{32,1,3,31},
		{32,2,4,31},
		{32,3,-1,-1},
		{32,4,1,33},
		{32,5,2,33},
		{33,1,4,32},
		{33,2,5,32},
		{33,3,1,34},
		{33,4,2,34},
		{34,1,3,33},
		{34,2,4,33},
		{34,3,1,35},
		{34,4,2,35},
		{35,1,3,34},
		{35,2,4,34},
		{35,3,1,36},
		{35,4,2,36},
		{36,1,3,35},
		{36,2,4,35},
		{36,3,-1,-1},
		{36,4,1,37},
		{36,5,2,37},
		{36,6,3,37},
		{36,7,4,37},
		{37,2,5,36},
		{37,3,6,36},
		{37,4,7,36},
		{37,1,4,36},
		{37,5,1,39},
		{37,6,2,39},
		{37,7,3,39},
		{37,8,-1,-1},
		{39,1,5,37},
		{39,2,6,37},
		{39,3,7,37},
		{39,4,-1,-1},
		{39,5,1,40},
		{39,6,2,40},
		{40,1,5,39},
		{40,2,6,39},
		{40,3,-1,-1},
		{40,4,1,41},
		{40,5,2,41},
		{40,6,-1,-1},
		{41,1,4,40},
		{41,2,5,40},
		{41,3,-1,-1},
		{41,4,1,42},
		{41,5,2,42},
		{42,1,4,41},
		{42,2,5,41},
		{42,3,1,43},
		{42,4,2,43},
		{43,1,3,42},
		{43,2,4,42},
		{43,3,1,44},
		{43,4,2,44},
		{44,1,3,43},
		{44,2,4,43},
		{44,3,1,45},
		{44,4,2,45},
		{45,1,3,44},
		{45,2,4,44},
		{45,3,-1,-1},
		{45,4,-1,-1},
		{46,1,4,12},
		{46,2,2,13},
		{46,3,1,13},
		{46,4,6,12},
		{46,5,1,47},
		{46,6,2,47},
		{47,1,5,46},
		{47,2,6,46},
		{47,3,1,48},
		{47,4,2,48},
		{48,1,3,47},
		{48,2,4,47},
		{48,3,1,49},
		{48,4,2,49},
		{49,1,3,48},
		{49,2,4,48},
		{49,3,1,50},
		{49,4,2,50},
		{50,1,3,49},
		{50,2,4,49},
		{50,3,1,51},
		{50,4,2,51},
		{51,1,3,50},
		{51,2,4,50},
		{51,3,1,52},
		{51,4,2,52},
		{52,1,3,51},
		{52,2,4,51},
		{52,3,-1,-1},
		{52,4,-1,-1},
		{53,1,11,58},
		{53,2,3,10},
		{53,3,2,55},
		{53,4,4,57},
		{53,5,3,57},
		{53,6,-1,-1},
		{53,7,-1,-1},
		{53,8,1,55},
		{54,1,6,57},
		{54,2,4,56},
		{54,3,-1,-1},
		{54,4,-1,-1},
		{54,5,6,58},
		{54,6,7,58},
		{54,7,8,58},
		{54,8,2,59},
		{55,1,8,53},
		{55,2,3,53},
		{55,6,1,59},
		{55,3,3,56},
		{55,4,2,56},
		{55,5,5,56},
		{56,1,7,57},
		{56,2,4,55},
		{56,3,3,55},
		{56,4,2,54},
		{56,5,5,55},
		{57,1,9,58},
		{57,2,10,58},
		{57,5,4,58},
		{57,4,4,53},
		{57,3,5,53},
		{57,6,1,54},
		{57,6,5,58},
		{57,7,1,56},
		{58,1,4,9},
		{58,2,3,9},
		{58,11,1,53},
		{58,3,1,10},
		{58,4,5,57},
		{58,5,6,57},
		{58,6,5,54},
		{58,7,6,54},
		{58,8,7,54},
		{58,9,1,57},
		{58,10,2,57},
		{59,1,6,55},
		{59,2,8,54},
		{59,3,6,15},
		{59,4,1,16},
		{59,5,4,15},
		{59,6,4,16},
		{60,1,-1,-1},
		{60,2,4,6},
		{60,3,-1,-1},
		{60,4,-1,-1},
		{60,5,-1,-1},
		{60,6,3,7},
		{61,1,4,7},
		{61,2,1,62},
		{62,1,2,61},
		{62,2,1,63},
		{63,1,2,62},
		{63,2,3,12}
};

ushort OsGetStationEntry(ushort usSourceStationNo,ushort usDestineStationNo,
						 ushort usDirFlag)
{
	for(int i=0;i < sizeof(sAllStationEntryLinkInfo) / sizeof(sAllStationEntryLinkInfo[0]);i++)
	{
		if(usSourceStationNo == sAllStationEntryLinkInfo[i].usSourceStationNo &&
			usDestineStationNo == sAllStationEntryLinkInfo[i].usDestineStationNo)
		{
			if(usDirFlag == UP_DIRECT && 
				(sAllStationEntryLinkInfo[i].usSourceStationEntryNo % 2 == 0 ||
				sAllStationEntryLinkInfo[i].usDestineStationEntryNo % 2 == 0) )
			return sAllStationEntryLinkInfo[i].usSourceStationEntryNo;

			if(usDirFlag == DOWN_DIRECT && 
				(sAllStationEntryLinkInfo[i].usSourceStationEntryNo % 2 == 1 ||
				sAllStationEntryLinkInfo[i].usDestineStationEntryNo % 2 == 1) )
			return sAllStationEntryLinkInfo[i].usSourceStationEntryNo;
		}
	}
	for(i=0;i < sizeof(sAllStationEntryLinkInfo) / sizeof(sAllStationEntryLinkInfo[0]);i++)
	{
		if(usSourceStationNo == sAllStationEntryLinkInfo[i].usSourceStationNo &&
			usDestineStationNo == sAllStationEntryLinkInfo[i].usDestineStationNo)
		{
			return sAllStationEntryLinkInfo[i].usSourceStationEntryNo;
		}
	}
	return -1;
}

ushort OsGetDestineStationNo(ushort usStationNo,ushort usEntryNo)
{
	for(int i=0;i < sizeof(sAllStationEntryLinkInfo) / sizeof(sAllStationEntryLinkInfo[0]);i++)
	{
		if(usStationNo == sAllStationEntryLinkInfo[i].usSourceStationNo &&
			usEntryNo == sAllStationEntryLinkInfo[i].usSourceStationEntryNo)
		{
			return sAllStationEntryLinkInfo[i].usDestineStationNo;
		}
	}
	return -1;
};

ushort OsGetStationEntrylinkInfo(ushort usStationNo,ushort *usEntryNum,struct _tagStationEntryLink *sEntryLinkInfo)
{
	ushort usCount = 0;

	for(int i=0;i < sizeof(sAllStationEntryLinkInfo) / sizeof(sAllStationEntryLinkInfo[0]);i++)
	{
		if(usStationNo == sAllStationEntryLinkInfo[i].usSourceStationNo)
		{
			if(sEntryLinkInfo != NULL && *usEntryNum > usCount)
			{
				sEntryLinkInfo[usCount] = sAllStationEntryLinkInfo[i]; 
			}
			usCount += 1;
		}
	}
	if(usEntryNum != NULL)
		*usEntryNum = usCount;
	return usCount;
};

ushort OsGetStationEntrylinkInfo(ushort usStationNo,ushort *usEntryNum,ushort *usEntryLinkIndex)
{
	ushort usCount = 0;

	for(int i=0;i < sizeof(sAllStationEntryLinkInfo) / sizeof(sAllStationEntryLinkInfo[0]);i++)
	{
		if(usStationNo == sAllStationEntryLinkInfo[i].usSourceStationNo)
		{
			if(usEntryLinkIndex != NULL && *usEntryNum > usCount)
				usEntryLinkIndex[usCount] = i; 
			usCount += 1;
		}
	}
	if(usEntryNum != NULL)
		*usEntryNum = usCount;
	return usCount;
};
****************************************/
/////////////////////////////////////////////////////////////////////////////
// CChangeTrainEntrySequence dialog


CChangeTrainEntrySequence::CChangeTrainEntrySequence(CWintgDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CChangeTrainEntrySequence::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeTrainEntrySequence)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
}


void CChangeTrainEntrySequence::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeTrainEntrySequence)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeTrainEntrySequence, CDialog)
	//{{AFX_MSG_MAP(CChangeTrainEntrySequence)
	ON_BN_CLICKED(IDC_SES_ACK, OnSesAck)
	ON_BN_CLICKED(IDC_SES_CANCEL, OnSesCancel)
	ON_BN_CLICKED(IDC_SES_DELETE, OnSesDelete)
	ON_BN_CLICKED(IDC_SES_DOWN, OnSesDown)
	ON_BN_CLICKED(IDC_SES_INSERT, OnSesInsert)
	ON_CBN_SELCHANGE(IDC_SES_STATIONLIST, OnSelchangeSesStationlist)
	ON_BN_CLICKED(IDC_SES_UP, OnSesUp)
	ON_MESSAGE(UM_RECEIVE_STATION_ENTRY_RUN_SEQUENCE,OnReceiveStationEntryRunSequence)
	ON_WM_DESTROY()
	ON_LBN_SETFOCUS(IDC_SES_LIST1, OnSetfocusSesList1)
	ON_LBN_SETFOCUS(IDC_SES_LIST2, OnSetfocusSesList2)
	ON_LBN_SETFOCUS(IDC_SES_LIST3, OnSetfocusSesList3)
	ON_LBN_SETFOCUS(IDC_SES_LIST4, OnSetfocusSesList4)
	ON_LBN_SETFOCUS(IDC_SES_LIST5, OnSetfocusSesList5)
	ON_LBN_SETFOCUS(IDC_SES_LIST6, OnSetfocusSesList6)
	ON_LBN_SETFOCUS(IDC_SES_LIST7, OnSetfocusSesList7)
	ON_LBN_SETFOCUS(IDC_SES_LIST8, OnSetfocusSesList8)
	ON_LBN_SETFOCUS(IDC_SES_LIST9, OnSetfocusSesList9)
	ON_LBN_SETFOCUS(IDC_SES_LIST10, OnSetfocusSesList10)
	ON_LBN_SETFOCUS(IDC_SES_LIST11, OnSetfocusSesList11)
	ON_LBN_SETFOCUS(IDC_SES_LIST12, OnSetfocusSesList12)
	ON_LBN_SETFOCUS(IDC_SES_LIST13, OnSetfocusSesList13)
	ON_LBN_SETFOCUS(IDC_SES_LIST14, OnSetfocusSesList14)
	ON_LBN_SETFOCUS(IDC_SES_LIST15, OnSetfocusSesList15)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeTrainEntrySequence message handlers

void CChangeTrainEntrySequence::OnSesAck() 
{
	// TODO: Add your control notification handler code here
	int i,j,k;
	struct _tagTrainStationEntryRunSequence *curr;
	struct _tagEntrySequence *rec;
	ushort usMessageLength;
	char *ptr,messageBuffer[2048],*temp;
	ushort usEntryCount,*pusEntryCount;
	ptr = messageBuffer;

	*(byte *)ptr = PMTG_MESSAGE;
	ptr += 3;
//	*(ushort *)ptr = NettoHostUShort(S_SET_MIDDLE_STATION_TRAIN_RUN_SEQUENCE);
	*(ushort *)ptr = NettoHostUShort(SET_STATION_ENTRY_TRAIN_LIST);
	ptr += 2;
	int nChangeFlag,nCount;
	CListBox *pListBox;
	CString text;

	temp = ptr;
	for(i=0;i < m_nTrainStationEntryRunSequenceNum;i++)
	{
		curr = info[i];
		ptr = temp;
		*(ushort *)ptr = NettoHostUShort(curr->usStationNo);
		ptr += 2;
		pusEntryCount = (ushort *)ptr;
		ptr += 2;
		usEntryCount = 0;
		for(j=0;j < curr->usEntryNum;j++)
		{
			rec = &curr->rec[j];

			nChangeFlag = 0;
			pListBox = (CListBox *)GetDlgItem(IDC_SES_LIST1 + j);
			nCount = pListBox->GetCount( );
			if(nCount == rec->usTrainNum)
			{
				for(k=0;k < nCount;k++)
				{
					pListBox->GetText(k,text);

					if(text.Compare(rec->sTrainList[k]))
					{
						strcpy(rec->sTrainList[k],(char *)(LPCTSTR)text);
						nChangeFlag = 1;
					}
				}
			} else {
				nChangeFlag = 1;
				for(k=0;k < nCount;k++)
				{
					pListBox->GetText(k,text);
					strcpy(rec->sTrainList[k],(char *)(LPCTSTR)text);
				}
				rec->usTrainNum = nCount;
			}
			if(nChangeFlag)
			{
				usEntryCount += 1;
				*(byte *)ptr = rec->usEntryID;
				ptr += 1;
				*(ushort *)ptr = NettoHostUShort(rec->usTrainNum);
				ptr += 2;
				for(k=0;k < rec->usTrainNum;k++)
				{
					strcpy(ptr,rec->sTrainList[k]);
					ptr += 10;
				}
			}
		}
		if(usEntryCount)
		{
			*(ushort *)pusEntryCount = NettoHostUShort(usEntryCount);
			usMessageLength = (ushort)(ptr - messageBuffer);
			*(ushort *)(messageBuffer + 1) = NettoHostUShort(usMessageLength - 1);
		//将车站的入口信息发送给服务器
			if(SendTGCommMessage(
				usMessageLength,messageBuffer))
				return;
		}
	}

	CDialog::OnOK( );
}

void CChangeTrainEntrySequence::OnSesCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel( );	
}

void CChangeTrainEntrySequence::OnSesDelete() 
{
	// TODO: Add your control notification handler code here
	
	do {
		if(m_sFocusEntryList == NULL)
			break;
		if(m_sFocusEntryList->IsWindowVisible( ) == false)
			break;
		int nSel = m_sFocusEntryList->GetCurSel( );
		if(nSel == LB_ERR)
			break;
		CString text;

		m_sFocusEntryList->DeleteString(nSel);
		if(nSel == 0)
			m_sFocusEntryList->SetCurSel(0);
		else
			m_sFocusEntryList->SetCurSel(nSel - 1);

	}while(0);
}

void CChangeTrainEntrySequence::OnSesDown() 
{
	// TODO: Add your control notification handler code here
	do {
		if(m_sFocusEntryList == NULL)
			break;
		if(m_sFocusEntryList->IsWindowVisible( ) == false)
			break;
		int nSel = m_sFocusEntryList->GetCurSel( );
		if(nSel == LB_ERR)
			break;
		if(nSel == m_sFocusEntryList->GetCount( ) - 1)
			break;
		CString text;
		m_sFocusEntryList->GetText(nSel,text);
		m_sFocusEntryList->DeleteString(nSel);
		m_sFocusEntryList->InsertString(nSel + 1,text);
		m_sFocusEntryList->SetCurSel(nSel + 1);
	}while(0);
}

void CChangeTrainEntrySequence::OnSesInsert() 
{
	// TODO: Add your control notification handler code here
	
	do {
		if(m_sFocusEntryList == NULL)
			break;
		if(m_sFocusEntryList->IsWindowVisible( ) == false)
			break;
		int nSel = m_sFocusEntryList->GetCurSel( );
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_SES_TRAINID);
		if(pEdit == NULL)
			break;
		CString text;
		pEdit->GetWindowText(text);
		if(text.GetLength( ) == 0)
			break;
		if(IsAllSpace((char *)(LPCTSTR)text) )
			break;
		if(nSel == LB_ERR) {
			m_sFocusEntryList->AddString(TrimSpace((char *)(LPCTSTR)text));
			m_sFocusEntryList->SetCurSel(m_sFocusEntryList->GetCount( ) - 1);
		}else {
			m_sFocusEntryList->InsertString(nSel,TrimSpace((char *)(LPCTSTR)text));
			m_sFocusEntryList->SetCurSel(nSel);
		}
	}while(0);
}

void CChangeTrainEntrySequence::OnSelchangeSesStationlist() 
{
	// TODO: Add your control notification handler code here
	struct _tagTrainStationEntryRunSequence *curr;
	struct _tagEntrySequence *rec;
	int i,j;
	CListBox *pListBox;
	CString text;

	do {
		//store current edit info 
		if(m_nFocusSelection != -1)
		{
			int nTotalCount;

			curr = info[m_nFocusSelection];
			for(i=0;i < curr->usEntryNum;i++)
			{
				pListBox = (CListBox *)GetDlgItem(IDC_SES_LIST1 + i);
				nTotalCount = pListBox->GetCount( );
				for(j=0;j < nTotalCount;j++)
				{
					pListBox->GetText(j,text);
					if(text.Compare(rec->sTrainList[j]) )
					{
						rec->usChangeFlag = 1;
						strcpy(rec->sTrainList[j],(char *)(LPCTSTR)text);
					}
				}
				rec->usTrainNum = nTotalCount;
			}
		}

		CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_SES_STATIONLIST);
		if(pComboBox == NULL)
			break;
		int nSel = pComboBox->GetCurSel( );
		if(nSel == CB_ERR)
			break;
		ushort usStationNo = (ushort)pComboBox->GetItemData(nSel);

		for(i=0;i < m_nTrainStationEntryRunSequenceNum;i++)
		{
			curr = info[i];
			if(curr->usStationNo == usStationNo)
				break;
		}
		if(i < m_nTrainStationEntryRunSequenceNum)
		{//已有列车在该站入口运行次序信息
			CWnd *pWnd;

			m_nFocusSelection =  i;
			for(i=0;i < curr->usEntryNum;i++)
			{
				pWnd = GetDlgItem(IDC_SES_LIST1 + i);
				pWnd->ShowWindow(SW_SHOW);
				pListBox = (CListBox *)pWnd;
				pListBox->ResetContent( );

				pWnd = GetDlgItem(IDC_SES_LABEL1 + i);
				pWnd->ShowWindow(SW_SHOW);
				rec = &curr->rec[i];
				text.Format("%s至%s(入口%d)",
					gpcdef.GetStationName(rec->usFirstStationNo),
					gpcdef.GetStationName(rec->usSecondStationNo),
					rec->usEntryID);
				pWnd->SetWindowText(text);
				pListBox->ResetContent( );
				for(j=0;j < rec->usTrainNum;j++)
				{
					pListBox->AddString(rec->sTrainList[j]);
				}
			}
			m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST1);	
			for(;i < MAX_ENTRY_NUMBER_PER_STATION;i++)
			{
				pWnd = GetDlgItem(IDC_SES_LIST1 + i);
				pWnd->ShowWindow(SW_HIDE);
				pWnd = GetDlgItem(IDC_SES_LABEL1 + i);
				pWnd->ShowWindow(SW_HIDE);
			}
			ChangeWindowSize(curr->usEntryNum);
		} else {//向主机发送申请信息	
			char *ptr,messageBuffer[128];
			ushort usMessageLength;
			//向服务器发送申请信息
			ptr = messageBuffer;
			*ptr = PMTG_MESSAGE;
			ptr += 1;
			ptr += 2;
//			*(ushort *)ptr = NettoHostUShort(S_REQUEST_STATION_ENTRY_SEQUENCE_INFORM);
			*(ushort *)ptr = NettoHostUShort(GET_STATION_ENTRY_TRAIN_LIST);
			ptr += 2;
			*(ushort *)ptr = NettoHostUShort(usStationNo);
			ptr += 2;
			usMessageLength = (ushort)(ptr - messageBuffer);
			*(ushort *)(messageBuffer + 1) = NettoHostUShort(usMessageLength - 1);
			if(SendTGCommMessage(
				usMessageLength,messageBuffer))
				return;


			//查找该车站的入口信息
			ushort usEntryNum;
			struct _tagStationEntryLink sEntryInfo[64];
			struct _tagEntrySequence *rec;


			usEntryNum = sizeof(sEntryInfo) / sizeof(sEntryInfo[0]);
			if(OsGetStationEntrylinkInfo(usStationNo,&usEntryNum,sEntryInfo) == -1)
			{
				sysprint(0,PMTG.ThreadId,"[PMTG]: get station entry info failed",
					__FILE__,__LINE__);
				return;
			}

			info[m_nTrainStationEntryRunSequenceNum] = (struct _tagTrainStationEntryRunSequence *)new char[sizeof(struct _tagTrainStationEntryRunSequence) + sizeof(struct _tagEntrySequence) * (usEntryNum - 1)];
			if(info[m_nTrainStationEntryRunSequenceNum] == NULL)
			{
				sysprint(0,PMTG.ThreadId,"[PMTG]: alloc failed in file %s at line %d",
					__FILE__,__LINE__);
				return;
			}
			info[m_nTrainStationEntryRunSequenceNum]->usStationNo = usStationNo;
			info[m_nTrainStationEntryRunSequenceNum]->usEntryNum = usEntryNum;
			
			CWnd *pWnd;

			for(i=0;i < usEntryNum;i++)
			{
				rec = &info[m_nTrainStationEntryRunSequenceNum]->rec[i];
				rec->usEntryID = sEntryInfo[i].usSourceStationEntryNo;
				rec->usChangeFlag = 0;
				//根据站号和入口号得到另一车站站码
				rec->usSecondStationNo = sEntryInfo[i].usDestineStationNo;
				rec->usTrainNum = 0;
				rec->usFirstStationNo = usStationNo;

				pWnd = GetDlgItem(IDC_SES_LIST1 + i);
				pWnd->ShowWindow(SW_SHOW);
				pListBox = (CListBox *)pWnd;
				pListBox->ResetContent( );

				pWnd = GetDlgItem(IDC_SES_LABEL1 + i);
				pWnd->ShowWindow(SW_SHOW);
				text.Format("%s至%s(入口%d)",
					gpcdef.GetStationName(rec->usFirstStationNo),
					gpcdef.GetStationName(rec->usSecondStationNo),
					rec->usEntryID);
				pWnd->SetWindowText(text);
			}
			for( ;i < MAX_ENTRY_NUMBER_PER_STATION;i++)
			{
				pWnd = GetDlgItem(IDC_SES_LIST1 + i);
				pWnd->ShowWindow(SW_HIDE);
				pWnd = GetDlgItem(IDC_SES_LABEL1 + i);
				pWnd->ShowWindow(SW_HIDE);
			}
			m_nTrainStationEntryRunSequenceNum += 1;
			m_nFocusSelection =  -1;
			m_sFocusEntryList = NULL;	

			ChangeWindowSize(usEntryNum);
		}
	}while(0);
}

void CChangeTrainEntrySequence::OnSesUp() 
{
	// TODO: Add your control notification handler code here
	
	do {
		if(m_sFocusEntryList == NULL)
			break;
		if(m_sFocusEntryList->IsWindowVisible( ) == false)
			break;
		int nSel = m_sFocusEntryList->GetCurSel( );
		if(nSel == LB_ERR)
			break;
		if(nSel == 0)
			break;
		CString text;
		m_sFocusEntryList->GetText(nSel,text);
		m_sFocusEntryList->DeleteString(nSel);
		m_sFocusEntryList->InsertString(nSel - 1,text);
		m_sFocusEntryList->SetCurSel(nSel - 1);
	}while(0);
}

BOOL CChangeTrainEntrySequence::PreTranslateMessage(MSG* lpMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ((lpMsg->message == WM_KEYDOWN) &&
		((lpMsg->wParam == VK_RETURN) || (lpMsg->wParam == VK_ESCAPE)))
	{
		CWnd *pWndFocus = NULL;
		TCHAR szClass[10];
		if ((lpMsg->wParam == VK_RETURN) &&
			((pWndFocus = GetFocus()) != NULL) &&
			IsChild(pWndFocus)  &&
			GetClassName(pWndFocus->m_hWnd, szClass, 10) &&
			(_tcsicmp(szClass, _T("EDIT")) == 0)
		){
			return TRUE;
		} else if (lpMsg->wParam == VK_ESCAPE)
			return TRUE;
	}	
	return CDialog::PreTranslateMessage(lpMsg);
}

BOOL CChangeTrainEntrySequence::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nFocusSelection = -1;
	m_sFocusEntryList = NULL;
	m_nTrainStationEntryRunSequenceNum = 0;
	// TODO: Add extra initialization here
	int nStationIndexFlag[32];
	memset(nStationIndexFlag,0x00,sizeof(nStationIndexFlag));
	int nCount = 0;
	int *subStation,nStationNum;
	CComboBox *pComboBox = (CComboBox *)GetDlgItem(IDC_SES_STATIONLIST);
	for(int i=0;i < m_pDoc->m_nAllStationNum;i++)
	{
		if(subStation = gpcdef.GetSubstation(m_pDoc->m_nAllStationList[i],nStationNum))
		{
			do {
				nStationNum -= 1;
				subStation[nStationNum];

				if(nStationIndexFlag[(subStation[nStationNum] - 1)/32] & (1 << (subStation[nStationNum] - 1)%32))
					continue;
				nStationIndexFlag[(subStation[nStationNum] - 1)/32] |= (1 << (subStation[nStationNum] - 1)%32);
				
				pComboBox->AddString(gpcdef.GetStationName(subStation[nStationNum]));
				pComboBox->SetItemData(nCount,(DWORD)subStation[nStationNum]);
				nCount += 1;
			}while(nStationNum > 0);
		} else {
			if(nStationIndexFlag[(m_pDoc->m_nAllStationList[i] - 1)/32] & (1 << (m_pDoc->m_nAllStationList[i] - 1)%32))
				continue;
			nStationIndexFlag[(m_pDoc->m_nAllStationList[i] - 1)/32] |= (1 << (m_pDoc->m_nAllStationList[i] - 1)%32);
			
			pComboBox->AddString(gpcdef.GetStationName(m_pDoc->m_nAllStationList[i]));
			pComboBox->SetItemData(nCount,(DWORD)m_pDoc->m_nAllStationList[i]);
			nCount += 1;
		}
	}
	pComboBox->SetCurSel(0);
	//向主机发送申请信息	
	while(OsGetQueueHandle(PMTG.ThreadId,&m_
		"QUEUES.GPC.COMM"))
	{
		OsSleep(1000L,PMTG.ThreadId);
		sysprint(0,PMTG.ThreadId,"[PMTG]:get comm queue handle failed");
	}
	char *ptr,messageBuffer[128];
	ushort usMessageLength;
	//向服务器发送申请信息
	ptr = messageBuffer;
	*ptr = PMTG_MESSAGE;
	ptr += 1;
	ptr += 2;
//	*(ushort *)ptr = NettoHostUShort(S_REQUEST_STATION_ENTRY_SEQUENCE_INFORM);
	*(ushort *)ptr = NettoHostUShort(GET_STATION_ENTRY_TRAIN_LIST);
	ptr += 2;
	ushort usStationNo = (ushort)pComboBox->GetItemData(0);
	*(ushort *)ptr = NettoHostUShort(usStationNo);
	ptr += 2;
	usMessageLength = (ushort)(ptr - messageBuffer);
	*(ushort *)(messageBuffer + 1) = NettoHostUShort(usMessageLength - 1);
	if(SendTGCommMessage(
		usMessageLength,messageBuffer))
		return false;


	//查找该车站的入口信息
	ushort usEntryNum;
	struct _tagStationEntryLink sEntryInfo[64];
	struct _tagEntrySequence *rec;


	usEntryNum = sizeof(sEntryInfo) / sizeof(sEntryInfo[0]);
	if(OsGetStationEntrylinkInfo(usStationNo,&usEntryNum,sEntryInfo) == -1)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG]: get station entry info failed",
			__FILE__,__LINE__);
		return false;
	}

	info[m_nTrainStationEntryRunSequenceNum] = (struct _tagTrainStationEntryRunSequence *)new char[sizeof(struct _tagTrainStationEntryRunSequence) + sizeof(struct _tagEntrySequence) * (usEntryNum - 1 + MAX_GUANWANG_ENTRY_LINK)];
	if(info[m_nTrainStationEntryRunSequenceNum] == NULL)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG]: alloc failed in file %s at line %d",
			__FILE__,__LINE__);
		return false;
	}
	info[m_nTrainStationEntryRunSequenceNum]->usStationNo = usStationNo;
	info[m_nTrainStationEntryRunSequenceNum]->usEntryNum = usEntryNum;
	
	CWnd *pWnd;
	CString text;
	for(i=0;i < usEntryNum;i++)
	{
		rec = &info[m_nTrainStationEntryRunSequenceNum]->rec[i];
		rec->usEntryID = sEntryInfo[i].usSourceStationEntryNo;
		rec->usChangeFlag = 0;
		//根据站号和入口号得到另一车站站码
		rec->usSecondStationNo = sEntryInfo[i].usDestineStationNo;
		rec->usTrainNum = 0;
		rec->usFirstStationNo = usStationNo;

		pWnd = GetDlgItem(IDC_SES_LIST1 + i);
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_SES_LABEL1 + i);
		pWnd->ShowWindow(SW_SHOW);
		text.Format("%s至%s(入口%d)",
			gpcdef.GetStationName(rec->usFirstStationNo),
			gpcdef.GetStationName(rec->usSecondStationNo),
			rec->usEntryID);
		pWnd->SetWindowText(text);
	}
	for( ;i < MAX_ENTRY_NUMBER_PER_STATION;i++)
	{
		pWnd = GetDlgItem(IDC_SES_LIST1 + i);
		pWnd->ShowWindow(SW_HIDE);
		pWnd = GetDlgItem(IDC_SES_LABEL1 + i);
		pWnd->ShowWindow(SW_HIDE);
	}
/****************************************
	//初始化当前车站各个入口的列车运行次序信息
	train_list = (struct _tagMiddleStationRunSequence (*)[ ])new char[sizeof(struct _tagMiddleStationRunSequence) * info[m_nTrainStationEntryRunSequenceNum]->usEntryNum];
	if(train_list == NULL)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG]: alloc failed in file %s at line %d",
			__FILE__,__LINE__);
		return false;
	}
	for(i=0;i < info[m_nTrainStationEntryRunSequenceNum]->usEntryNum;i++)
	{
		rec = &info[m_nTrainStationEntryRunSequenceNum]->rec[i];
		(*train_list)[i].usStationNo = info[m_nTrainStationEntryRunSequenceNum]->usStationNo;
		(*train_list)[i].usExitEntryNo = rec->usEntryID;
		(*train_list)[i].usTrainRecNum = 0;
	}
	Pstruct TRAINRECORD *trnrec;
	struct _tagTrainEntryRec *curr;
	int flag;
	for(int j=0;j < m_pDoc->m_nSchdTrainNum;j++)
	{
		if(m_pDoc->m_sSchdTrain[j] == NULL)
			continue;
		trnrec = m_pDoc->m_sSchdTrain[j]->GetStationRecord(info[m_nTrainStationEntryRunSequenceNum]->usStationNo);
		if(trnrec == NULL)
			continue;
		flag = 0;
		for(i=0;i < info[m_nTrainStationEntryRunSequenceNum]->usEntryNum;i++)
		{
			curr = &(*train_list)[i].rec[(*train_list)[i].usTrainRecNum];
			if(trnrec->entry == (*train_list)[i].usExitEntryNo &&
				trnrec->arrival_time != NOTIME && trnrec->arrival_date != NOTIME)
			{
				strcpy(curr->lpszTrainID,(char *)(LPCTSTR)m_pDoc->m_sSchdTrain[j]->m_strTrainId);
				curr->usRunDate = trnrec->arrival_date;
				curr->usRunTime = trnrec->arrival_time;
				(*train_list)[i].usTrainRecNum += 1;
				flag |= 1;
			}
			if(trnrec->exit == (*train_list)[i].usExitEntryNo &&
				trnrec->depart_time != NOTIME && trnrec->depart_date != NOTIME)
			{
				strcpy(curr->lpszTrainID,(char *)(LPCTSTR)m_pDoc->m_sSchdTrain[j]->m_strTrainId);
				curr->usRunDate = trnrec->depart_date;
				curr->usRunTime = trnrec->depart_time;
				(*train_list)[i].usTrainRecNum += 1;
				flag |= 2;
			}
			if(flag == 3)
				break;
		}
	}
	//显示当前车站各个入口的运行列车信息
	CListBox *pListBox;
	for(i=0;i < info[m_nTrainStationEntryRunSequenceNum]->usEntryNum;i++)
	{
		ArrayTrainSequence(&(*train_list)[i]);
		pListBox = (CListBox *)GetDlgItem(IDC_SES_LIST1 + i);
		for(j=0;j < (*train_list)[i].usTrainRecNum;j++)
		{
			curr = &(*train_list)[i].rec[j];
			pListBox->AddString(curr->lpszTrainID);
		}
	}
****************************************/
	ChangeWindowSize(usEntryNum);
	m_nTrainStationEntryRunSequenceNum += 1;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeTrainEntrySequence::OnReceiveStationEntryRunSequence(WPARAM wParam,LPARAM lParam)
{
	struct _tagEntrySequence *rec;
	struct _tagTrainStationEntryRunSequence *curr;
	char *ptr = (char *)wParam;

	ushort usStationNo = NettoHostUShort(*(ushort *)ptr);
	ptr += 2;
	ushort usEntryCount = NettoHostUShort(*(ushort *)ptr);
	ptr += 2;

/********************************
	info[m_nTrainStationEntryRunSequenceNum] = (struct _tagTrainStationEntryRunSequence *)new char[sizeof(struct _tagTrainStationEntryRunSequence) + sizeof(struct _tagEntrySequence) * (usEntryCount - 1)];
	if(info[m_nTrainStationEntryRunSequenceNum] == NULL)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG]: alloc failed in file %s at line %d",
			__FILE__,__LINE__);
		return ;
	}
	info[m_nTrainStationEntryRunSequenceNum]->usStationNo = usStationNo;
	info[m_nTrainStationEntryRunSequenceNum]->usEntryNum = usEntryCount;
********************************/
	for(int i=0;i < m_nTrainStationEntryRunSequenceNum;i++)
	{
		curr = info[i];
		if(curr->usStationNo == usStationNo)
			break;
	}
	if(i >= m_nTrainStationEntryRunSequenceNum)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG]: client havn't request station %d entry run seq info",
			usStationNo);
		return;//没有列车在该站入口运行次序信息
	}
	if(usEntryCount > curr->usEntryNum + MAX_GUANWANG_ENTRY_LINK)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG]: station %d entry info %d too much",
			usStationNo,usEntryCount);
		return;//入口信息太多
	}
	
	int j;
	ushort usEntryID;
	for(i=0;i < usEntryCount;i++)
	{
		usEntryID = *(byte *)ptr;
		ptr += 1;
		for(j=0;j < curr->usEntryNum;j++)
		{
			rec = &curr->rec[j];
			if(rec->usEntryID == usEntryID)
				break;
		}
		if(j >= curr->usEntryNum)
		{////没有找到该入口号信息,suppose 管外 entry link
//			sysprint(0,PMTG.ThreadId,"[PMTG]: havn't found station %d entry id %d",
//				usStationNo,usEntryID);
			rec = &curr->rec[curr->usEntryNum];
			rec->usChangeFlag = 0;
			rec->usEntryID = usEntryID;
			rec->usFirstStationNo = usStationNo;
			rec->usSecondStationNo = 0;
			rec->usTrainNum = NettoHostUShort(*(ushort *)ptr);
			ptr += 2;
			if(rec->usTrainNum > sizeof(rec->sTrainList) / sizeof(rec->sTrainList[0]) )
			{
				ptr += rec->usTrainNum * 10;
				rec->usTrainNum = 0;
				continue;
			}
			for(j=0;j < rec->usTrainNum;j++)
			{
				strcpy(rec->sTrainList[j],ptr);
				ptr += 10;
			}
			curr->usEntryNum += 1;
		} else {
			rec->usChangeFlag = 0;
			//根据站号和入口号得到另一车站站码
			rec->usTrainNum = NettoHostUShort(*(ushort *)ptr);
			ptr += 2;
			if(rec->usTrainNum > sizeof(rec->sTrainList) / sizeof(rec->sTrainList[0]) )
			{
				ptr += rec->usTrainNum * 10;
				rec->usTrainNum = 0;
				continue;
			}
			for(j=0;j < rec->usTrainNum;j++)
			{
				strcpy(rec->sTrainList[j],ptr);
				ptr += 10;
			}
		}
	}
	
	m_nFocusSelection =  i;

	CString text;
	CListBox *pListBox;
	CWnd *pWnd;
	for(i=0;i < curr->usEntryNum;i++)
	{
		pWnd = GetDlgItem(IDC_SES_LIST1 + i);
		pWnd->ShowWindow(SW_SHOW);
		pListBox = (CListBox *)pWnd;
		pListBox->ResetContent( );

		pWnd = GetDlgItem(IDC_SES_LABEL1 + i);
		pWnd->ShowWindow(SW_SHOW);
		rec = &curr->rec[i];
		text.Format("%s至%s(入口%d)",
			gpcdef.GetStationName(rec->usFirstStationNo),
			gpcdef.GetStationName(rec->usSecondStationNo),
			rec->usEntryID);
		pWnd->SetWindowText(text);
//		pListBox->ResetContent( );
		for(j=0;j < rec->usTrainNum;j++)
		{
			pListBox->AddString(rec->sTrainList[j]);
		}
	}
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST1);	
	for(;i < MAX_ENTRY_NUMBER_PER_STATION;i++)
	{
		pWnd = GetDlgItem(IDC_SES_LIST1 + i);
		pWnd->ShowWindow(SW_HIDE);
		pWnd = GetDlgItem(IDC_SES_LABEL1 + i);
		pWnd->ShowWindow(SW_HIDE);
	}

	ChangeWindowSize(curr->usEntryNum);
}

void CChangeTrainEntrySequence::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	for(int i = 0;i < m_nTrainStationEntryRunSequenceNum;i++)
	{
		delete info[i];
	}
}

void CChangeTrainEntrySequence::OnSetfocusSesList1() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST1);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList2() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST2);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList3() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST3);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList4() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST4);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList5() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST5);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList6() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST6);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList7() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST7);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList8() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST8);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList9() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST9);	
}

void CChangeTrainEntrySequence::ChangeWindowSize(ushort usEntryNum)
{
	CWnd *pWnd;
	CRect rclRange;
	int xPos;
	int xWidth,yHeight;

	if(usEntryNum <= 6)
	{
		pWnd = GetDlgItem(IDC_SES_LABEL6);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		xPos = rclRange.right + 15;
		yHeight = rclRange.bottom + 50;
		pWnd = GetDlgItem(IDC_SES_ACK);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_CANCEL);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_DOWN);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_UP);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_DELETE);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_INSERT);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_TRAINID);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		xWidth = xPos + rclRange.Width( ) + 50;
	}else if(usEntryNum > 6 && usEntryNum <= 9) {
		pWnd = GetDlgItem(IDC_SES_LABEL9);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		xPos = rclRange.right + 15;
		yHeight = rclRange.bottom + 50;
		pWnd = GetDlgItem(IDC_SES_ACK);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_CANCEL);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_DOWN);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_UP);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_DELETE);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_INSERT);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_TRAINID);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		xWidth = xPos + rclRange.Width( ) + 50;
	}else if(usEntryNum > 9 && usEntryNum <= 12) {
		pWnd = GetDlgItem(IDC_SES_LABEL12);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		xPos = rclRange.right + 15;
		yHeight = rclRange.bottom + 50;
		pWnd = GetDlgItem(IDC_SES_ACK);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_CANCEL);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_DOWN);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_UP);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_DELETE);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_INSERT);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_TRAINID);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		xWidth = xPos + rclRange.Width( ) + 50;
	}else if(usEntryNum > 12 && usEntryNum <= 15) {
		pWnd = GetDlgItem(IDC_SES_LABEL15);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		xPos = rclRange.right + 15;
		yHeight = rclRange.bottom + 50;
		pWnd = GetDlgItem(IDC_SES_ACK);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_CANCEL);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_DOWN);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_UP);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_DELETE);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_INSERT);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		pWnd = GetDlgItem(IDC_SES_TRAINID);
		pWnd->GetWindowRect(&rclRange);
		ScreenToClient(&rclRange);
		pWnd->SetWindowPos(NULL,xPos,rclRange.top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		xWidth = xPos + rclRange.Width( ) + 50;
	}
	SetWindowPos(NULL,0,0,xWidth,yHeight,
		SWP_NOZORDER|SWP_NOMOVE);
	CenterWindow( );
	Invalidate( );
}

void CChangeTrainEntrySequence::OnSetfocusSesList10() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST10);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList11() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST11);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList12() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST12);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList13() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST13);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList14() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST14);	
}

void CChangeTrainEntrySequence::OnSetfocusSesList15() 
{
	// TODO: Add your control notification handler code here
	m_sFocusEntryList = (CListBox *)GetDlgItem(IDC_SES_LIST15);	
}


int ArrayTrainSequence(struct _tagMiddleStationRunSequence *info)
{
	if(info == NULL || info->usTrainRecNum <= 0)
		return -1;
	struct _tagTrainEntryRec tmp;
	for(int i=0;i < info->usTrainRecNum - 1;i++)
	{
		for(int j= i + 1;j < info->usTrainRecNum;j++)
		{
			if(info->rec[i].usRunDate > info->rec[j].usRunDate ||
				(info->rec[i].usRunDate == info->rec[j].usRunDate &&
				info->rec[i].usRunTime > info->rec[j].usRunTime))
			{
				tmp = info->rec[i];
				info->rec[i] = info->rec[j];
				info->rec[j] = tmp;
			}
		}
	}
	return 0;
}
