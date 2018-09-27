// SoftKeyBoardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SoftKeyBoardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoftKeyBoardDlg dialog

CSoftKeyBoardDlg::CSoftKeyBoardDlg(CWnd* pParent /*=NULL*/, BYTE nKBStyle)
	: CDialog(CSoftKeyBoardDlg::IDD, pParent), m_pParent(pParent), m_nKBStyle(nKBStyle)
{
	//{{AFX_DATA_INIT(CSoftKeyBoardDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSoftKeyBoardDlg::~CSoftKeyBoardDlg()
{
	SHORT	rs = GetKeyState( VK_CAPITAL );
	if((rs&0x0001) == 1)//退出时将大写改为小写
	{	
		keybd_event(VK_CAPITAL, 0, 0, 0);
		keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0);
	}
}

char CSoftKeyBoardDlg::m_KeyArray[36] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
									'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
									'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
									'u', 'v', 'w', 'x', 'y', 'z' };

CString CSoftKeyBoardDlg::m_KeyText[52] = {"a", "A", "b", "B", "c", "C", "d", "D", "e", "E", 
										   "f", "F", "g", "G", "h", "H", "i", "I", "j", "J",
										   "k", "K", "l", "L", "m", "M", "n", "N", "o", "O", 
										   "p", "P", "q", "Q", "r", "R", "s", "S", "t", "T", 
										   "u", "U", "v", "V", "w", "W", "x", "X", "y", "Y", 
										   "z", "Z"};
void CSoftKeyBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoftKeyBoardDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSoftKeyBoardDlg, CDialog)
	//{{AFX_MSG_MAP(CSoftKeyBoardDlg)
	ON_COMMAND_RANGE(IDC_BUTTON_0, IDC_BUTTON_CAPS, OnMyKeyDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoftKeyBoardDlg message handlers

void CSoftKeyBoardDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	//	delete this;
}

BOOL CSoftKeyBoardDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( (m_nKBStyle&0xf0) == KB_LOWER)//如果初始化是小写
	{	
		for(int keybt = IDC_BUTTON_A; keybt <= IDC_BUTTON_Z; keybt++)
		{
			SetDlgItemText(keybt, m_KeyText[(keybt-IDC_BUTTON_A)*2]);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSoftKeyBoardDlg::OnMyKeyDown(UINT message)
{
	BYTE  vk;
	BOOL  bCaps = FALSE;
	BOOL  bCapsClicked = FALSE;
	SHORT rs = GetKeyState( VK_CAPITAL );
	if( (rs&0x0001) == 1)//如果是大写
		bCaps = TRUE;

	switch(message) 
	{
	case IDC_BUTTON_BACKSPACE:
		{
			vk = VK_BACK;
		}
		break;
	case IDC_BUTTON_CAPS:
		{
			vk = VK_CAPITAL;
			bCaps = !bCaps;
			bCapsClicked = TRUE;
		}
		break;
	default:
		{
			short vkscan = VkKeyScan(m_KeyArray[message-IDC_BUTTON_0]);
			vk = vkscan & 0xff;
		}
		break;
	}

	ASSERT(m_pParent);
	m_pParent->SetActiveWindow();

	keybd_event(vk, 0, 0, 0);
	keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);

	if(bCapsClicked)//如果按了Caps这个键
	{	
		if(bCaps)
		{	
			for(int keybt = IDC_BUTTON_A; keybt <= IDC_BUTTON_Z; keybt++)
			{
				SetDlgItemText(keybt, m_KeyText[(keybt-IDC_BUTTON_A)*2+1]);
			}
		}
		else
		{
			for(int keybt = IDC_BUTTON_A; keybt <= IDC_BUTTON_Z; keybt++)
			{
				SetDlgItemText(keybt, m_KeyText[(keybt-IDC_BUTTON_A)*2]);
			}
		}
	}
}
