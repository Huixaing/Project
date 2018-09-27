#include "stdafx.h"
#include <io.h>
#include "TrainSubaoDlg.h"
#include "cadstruct.h"
#include "tgdocument.h"
#include ".\trainsubaodlg.h"

#define MAX_COLUMN 3
TCHAR * _gzBiaoTi[MAX_COLUMN] ={"机车类型", "机车号码","牵引附挂",};

CSuBaoDlg::CSuBaoDlg(CWnd* pParent, ULONG train_index, CTgDocument* pDoc, const char* trainid, long space) : 
CDialog(CSuBaoDlg::IDD, pParent)
{
	m_pDoc = pDoc;
	schd_type = WORK_SCHEDULE;
	m_nTrainIndex=train_index;
	m_bReadOnly=m_pDoc->IsQueryMode();
	CTG_ScheduleInfo info;
	m_pDoc->GetScheduleInfo(info);
	m_nSectionID=info.m_nSectionID;
	m_nTrainId = trainid;
	m_sel=-1;
	m_space=space;

	GpcLogTrainMessage(0, 0, "列车 %s 超限等级为:%d", m_nTrainId, m_space);

}

void CSuBaoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSuBaoDlg)
	DDX_Control(pDX, IDC_LIST_LOCALID, m_List);

	DDX_Control(pDX, IDC_DRIVER_NAME, m_driver_name);
	DDX_Control(pDX, IDC_TRAIN_LEADER_NAME, m_driver_leader_name);
	DDX_Control(pDX, IDC_CAL_LEN, m_train_length);
	DDX_Control(pDX, IDC_TOTAL_WEIGHT, m_train_weight);
	DDX_Control(pDX, IDC_K_COUNT, m_train_car_num);
	DDX_Control(pDX, IDC_TRAIN_STATION_NAME, m_train_report);

	DDX_Control(pDX, IDC_COMBO1, m_enginetype);
	DDX_Control(pDX, IDC_COMBO2, m_tpye);
	DDX_Control(pDX, IDC_SPACE_LEVEL, m_spacelevel);

	DDX_Control(pDX, IDC_EDIT1, m_train_engineid);
	DDX_Text(pDX, IDC_EDIT1, m_strtrain_engineid);
	DDV_MaxChars(pDX, m_strtrain_engineid, 5);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSuBaoDlg, CDialog)
	//{{AFX_MSG_MAP(CSuBaoDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuBaoDlg message handlers

LRESULT CSuBaoDlg::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;

	if(pMsgStruct->msg_id==MSGID_MsgSubaoFormat)
	{
		RecvSubao(( CLBTH::MsgSubaoFormat *)pMsgStruct->pmsg);
	}

	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

BOOL CSuBaoDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *pNMHDR = (NMHDR *)lParam;
	if(pNMHDR->idFrom == IDC_LIST_LOCALID && pNMHDR->code == NM_CLICK)
	{
		POSITION pos = m_List.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			m_sel = m_List.GetNextSelectedItem(pos);			
			fillLeftContent(m_sel);
			
			return TRUE;
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CSuBaoDlg::fillLeftContent(int curSel)
{
	if(curSel >= m_List.GetItemCount())
		return;

	// 机车名称
	CString pref = "0";
	CString engineid = m_List.GetItemText(curSel,1);
	CString type = m_List.GetItemText(curSel,2);
	
	if(type == "牵引")
		m_tpye.SetCurSel(0);
	else
		m_tpye.SetCurSel(1);

	if(engineid.GetLength()>=8)
	{
		pref = engineid.Left(3);
		engineid = engineid.Right(5);
	}
	else if(engineid.GetLength()>5)
	{
		engineid = engineid.Right(5);
	}

	m_train_engineid.SetWindowText(engineid);

	if(!pref.IsEmpty())
	{
		for(int i=0; i<m_prefNum; i++)
		{
			int locol=m_enginetype.GetItemData(i);
			if(locol == atoi(pref))
			{
				m_enginetype.SetCurSel(i);
				break;
			}
		}
	}
}

BOOL CSuBaoDlg::OnInitDialog() 
{   
	CDialog::OnInitDialog();

	m_tpye.AddString("牵引");
	m_tpye.AddString("附挂");
	m_tpye.SetCurSel(0);
	m_prefNum=0;
	for(int i=0;i<256;i++)
	{
		int locoid;
		CString loconame=GetTGBasicParam()->GetLocoName(m_nSectionID, i, locoid);
		if(!loconame.IsEmpty())
		{
			int sel=m_enginetype.AddString(loconame);
			m_prefNum++;
			if(sel>=0)
				m_enginetype.SetItemData(sel,locoid);
		}
	}
	m_enginetype.SetCurSel(0);
	

	//改变listctrl的风格
	CRect rect;
	m_List.SetExtendedStyle(LVS_REPORT|LVS_EX_FULLROWSELECT| LVS_EX_ONECLICKACTIVATE);
	m_List.GetClientRect(&rect);

	LV_COLUMN lvc;
	//添加标题头
	for(int count=0;count<MAX_COLUMN;count++)
	{
		lvc.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT;
		lvc.fmt = LVCFMT_CENTER;
		lvc.cx = rect.Width() / MAX_COLUMN;
		lvc.pszText = _gzBiaoTi[count];
		m_List.InsertColumn(count, &lvc);
	}

	_TCHAR *NAME[5] = {
		_T("正常"),//0
		_T("超级"),//1
		_T("一级"),//2
		_T("二级"),//3
		_T("三级"),//4
	};

	if(GetTGFunctionConfig()->IsSubaoShowSpace())
	{
		CWnd* pWnd=GetDlgItem(IDC_STATIC_SPACE);
		if(pWnd)
			pWnd->ShowWindow(SW_SHOW);
		m_spacelevel.ShowWindow(SW_SHOW);

		for( i=0;i < 5;i++)
		{
			int row=m_spacelevel.AddString(NAME[i]);
			m_spacelevel.SetItemData(row,i);
		}
		if(0<=m_space && m_space<=4)
			m_spacelevel.SetCurSel(m_space);
		else
			m_spacelevel.SetCurSel(-1);
		
	}
	else
	{
		CWnd* pWnd=GetDlgItem(IDC_STATIC_SPACE);
		if(pWnd)
			pWnd->ShowWindow(SW_HIDE);
		m_spacelevel.ShowWindow(SW_HIDE);
	}

	QuerySubao();
	InitIdentify();
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgSubaoFormat,0);
	
	return TRUE;
}

void CSuBaoDlg::InitIdentify()
{
	if(m_bReadOnly)
	{
		CWnd *pWnd;
	
		pWnd = GetDlgItem(IDC_ADD);
		if(pWnd)
		    pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHANGE);
		if(pWnd)
		    pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_DELETE);
		if(pWnd)
			pWnd->EnableWindow(FALSE);
	}
}

void CSuBaoDlg::OnOK() 
{
	CDialog::OnOK();
}

bool CSuBaoDlg::GetSubaoInfo()
{
	BYTE tract_engine_count=0;
	BYTE attach_engine_count=0;

	int num = m_List.GetItemCount();
	int index=0;
	for(int i=0; i<num&&i<2; i++)
	{
		CString engineid = m_List.GetItemText(i, 1);
		CString strtype = m_List.GetItemText(i, 2);
		if(strtype == "牵引")
		{
			SetTractionEngineExt(m_suBao.subao_engine[index].engine_type);
			memset(m_suBao.subao_engine[index].engine_id, 0, sizeof(ENGINE_NO));
			strncpy(m_suBao.subao_engine[index].engine_id, engineid, sizeof(ENGINE_NO)-1);
			
			index++;
			tract_engine_count++;
		}
	}
	
	for(int i=0; i<num&&i<4; i++)
	{
		CString engineid = m_List.GetItemText(i, 1);
		CString strtype = m_List.GetItemText(i, 2);
		if(strtype == "附挂")
		{
			SetAttachEngineExt(m_suBao.subao_engine[index].engine_type);
			memset(m_suBao.subao_engine[index].engine_id, 0, sizeof(ENGINE_NO));
			strncpy(m_suBao.subao_engine[index].engine_id, engineid, sizeof(ENGINE_NO)-1);
			
			index++;
			attach_engine_count++;
		}
	}
	
	attach_engine_count = attach_engine_count&0x0F;
	tract_engine_count = tract_engine_count&0x0F;
	attach_engine_count <<= 4;

	m_suBao.engine_count = attach_engine_count + tract_engine_count;
	
	CString text;
	m_driver_name.GetWindowText(text);
	memset(m_suBao.driver_name, 0, sizeof(USERNAME));
	strncpy(m_suBao.driver_name, text, sizeof(USERNAME)-1);

	m_driver_leader_name.GetWindowText(text);
	memset(m_suBao.train_leader_name, 0, sizeof(USERNAME));
	strncpy(m_suBao.train_leader_name, text, sizeof(USERNAME)-1);

	m_train_length.GetWindowText(text);
	m_suBao.cal_len = atof(text)*10;

	m_train_car_num.GetWindowText(text);
	m_suBao.pass_cnt = atoi(text);

	m_train_weight.GetWindowText(text);
	m_suBao.total_weight = atof(text)*10;

	if(GetTGFunctionConfig()->IsSubaoShowSpace())
	{
		int sel=m_spacelevel.GetCurSel();
		if(sel==-1)
		{
			MessageBox("请选择列车超限级别", "警告", MB_OK|MB_ICONHAND);
			return false;
		}
		else
			m_suBao.space_rank = m_spacelevel.GetItemData(sel);
	}
	return true;
}

void CSuBaoDlg::OnAdd() 
{
	if(m_bReadOnly)
		return;

	m_suBao.space_rank = 0;

	if(!GetSubaoInfo())
		return;

	CTG_ScheduleInfo info;
	m_pDoc->GetScheduleInfo(info);

	m_suBao.shift_id=info.m_nShiftID;
	m_suBao.train_index = m_nTrainIndex;
	
	strncpy(m_suBao.train_id, m_nTrainId, sizeof(TRAIN_NO)-1);
	m_suBao.train_id[sizeof(TRAIN_NO)-1]=0;

	m_suBao.old_shift_id=0;
	m_suBao.old_train_index = 0;
	memset(m_suBao.old_train_id, 0, sizeof(TRAIN_NO));

	m_suBao.length_rank = 0;
	m_suBao.super_rank = 0;
	m_suBao.military_rank = 0;
	m_suBao.speed_limit = 255;

	m_suBao.e_engine = 0;
	m_suBao.print_flag = 0;
	
	m_suBao.load_cnt = 0;
	m_suBao.pk_count = 0;
	
	m_suBao.pz_count = 0;
	m_suBao.ck_count = 0;
	m_suBao.cz_count = 0;
	m_suBao.pik_count = 0;
	m_suBao.piz_count = 0;

	m_suBao.gk_count = 0;
	m_suBao.gz_count = 0;
	m_suBao.o_count = 0;
	m_suBao.s_count = 0;
	m_suBao.total_count = m_suBao.pass_cnt;

	m_suBao.van_count = 0;
	m_suBao.subao_type = 1;
	m_suBao.noutil_cnt = 0;
	m_suBao.speed = 0;
	
	SendSubaoMsg(CLBTH::MsgSubaoFormat::SUBAO_GPC_INSERT);	   
	Sleep(200);
	ClearEdit();
	QuerySubao();
}

void CSuBaoDlg::OnDelete() 
{  
	if(m_bReadOnly)
		return;

	if(!GetSubaoInfo())
		return;

	SendSubaoMsg(CLBTH::MsgSubaoFormat::SUBAO_GPC_DELETE);

	ClearEdit();
	QuerySubao();
}

void CSuBaoDlg::OnChange()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_bReadOnly)
		return;

	if(!GetSubaoInfo())
		return;

	SendSubaoMsg(CLBTH::MsgSubaoFormat::SUBAO_GPC_CHANGE_MISC);
	
	ClearEdit();
	QuerySubao();
}

BOOL CSuBaoDlg::ShowSubao()
{   
	TCHAR buffer[50];

	// 司机
	m_driver_name.SetWindowText(m_suBao.driver_name);

	// 车长姓名
	m_driver_leader_name.SetWindowText(m_suBao.train_leader_name);

	// 辆数
	memset(buffer, 0, 50);
	sprintf(buffer,"%d", m_suBao.pass_cnt);
	m_train_car_num.SetWindowText(buffer);

	// 总重
	CString tmp;
	long total_weight = m_suBao.total_weight;
	tmp.Format("%d.%d", total_weight/10, total_weight%10);
	m_train_weight.SetWindowText(tmp);

	// 计长
	long cal_len = m_suBao.cal_len;
	tmp.Format("%d.%d", cal_len/10, cal_len%10);
	m_train_length.SetWindowText(tmp);

	memset(buffer, 0, 50);

	// 上报站
	CString stName = gpcdef.GetStationName(m_suBao.report_sta);
	if(stName != "管外")
		m_train_report.SetWindowText(stName);
	else
		m_train_report.SetWindowText("");

	LV_ITEM lvi;

	BYTE tract_engine_count =   0x0F &(m_suBao.engine_count) ;  //牵引机车个数
	BYTE attach_engine_count = (0xF0 &(m_suBao.engine_count))>>4 ; //附挂机车个数
	//牵引机车数
	for(int i=0; i<tract_engine_count+attach_engine_count;i++ )
	{
		CString text;
	
		// 机车号
		CString engineId = m_suBao.subao_engine[i].engine_id;
		CString pref="";
		if(engineId.GetLength()>3)
			pref = engineId.Left(3);

		pref=GetTGBasicParam()->GetLocoPreName(0, pref);
		// 机车类型
		lvi.mask  =  LVIF_TEXT;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.pszText = const_cast<char *>((LPCSTR)pref);
		m_List.InsertItem(&lvi);

		// 机车号
		m_List.SetItemText(i, 1, engineId);
	
		// 模式
		if(i<tract_engine_count)
			m_List.SetItemText(i, 2, "牵引");
		else
			m_List.SetItemText(i, 2, "附挂");
	}

	return TRUE;
}

BOOL CSuBaoDlg::QuerySubao()
{   
	CLBTH::MsgSubaoQuery mMsgSubaoQuery;
	CTG_ScheduleInfo info;
	m_pDoc->GetScheduleInfo(info);
	mMsgSubaoQuery.shift_id = info.m_nShiftID;
	mMsgSubaoQuery.con_id =  info.m_nSectionID;
	mMsgSubaoQuery.schd_type = info.m_nType;
	mMsgSubaoQuery.train_index = m_nTrainIndex;
	mMsgSubaoQuery.query_filter = CLBTH::MsgSubaoQuery::SUBAO_QUERY_BY_TRAININDEX;

	long eid=GetSpecialEntityID(SERVER_TYPE_TG, mMsgSubaoQuery.con_id);
	SendTGBMsg(mMsgSubaoQuery.con_id,&mMsgSubaoQuery,eid);	
	GpcLogTrainMessage(0, 0, "[查询列车速报]: train_index %d svr_entity %d con_id %d shift_id:%d schd_type %d",
		mMsgSubaoQuery.train_index, 
		eid, 
		mMsgSubaoQuery.con_id, 
		mMsgSubaoQuery.shift_id,
		mMsgSubaoQuery.schd_type);
	return TRUE;
}

void CSuBaoDlg::ClearEdit()
{
	m_driver_name.SetWindowText("");
	m_driver_leader_name.SetWindowText("");
	m_train_length.SetWindowText("");
	m_train_weight.SetWindowText("");
	m_train_car_num.SetWindowText("");
	m_train_report.SetWindowText("");
	
	m_List.DeleteAllItems();
}

void CSuBaoDlg::RecvSubao(CLBTH::MsgSubaoFormat *pBuf)
{  
	if(pBuf->oper != CLBTH::MsgSubaoFormat::SUBAO_SERVER_RELAY)
		return;

	m_suBao.input_time = pBuf->input_time;
	m_suBao.shift_id = pBuf->shift_id;
	m_suBao.old_shift_id = pBuf->old_shift_id;
	m_suBao.train_index = pBuf->train_index;
	m_suBao.old_train_index = pBuf->old_train_index;

	m_suBao.con_id = pBuf->con_id;
	m_suBao.schd_type = pBuf->schd_type;
	m_suBao.report_sta = pBuf->report_sta;

	strncpy(m_suBao.train_id, pBuf->train_id, sizeof(TRAIN_NO)-1);
	m_suBao.train_id[sizeof(TRAIN_NO)-1]=0;

	strncpy(m_suBao.old_train_id, pBuf->old_train_id, sizeof(TRAIN_NO)-1);
	m_suBao.old_train_id[sizeof(TRAIN_NO)-1]=0;

	strncpy(m_suBao.car_des, pBuf->car_des, sizeof(m_suBao.car_des)-1);
	m_suBao.car_des[sizeof(m_suBao.car_des)-1]=0;

	strncpy(m_suBao.orig, pBuf->orig, sizeof(m_suBao.orig)-1);
	m_suBao.orig[sizeof(m_suBao.orig)-1]=0;

	strncpy(m_suBao.dest, pBuf->dest, sizeof(m_suBao.dest)-1);
	m_suBao.dest[sizeof(m_suBao.dest)-1]=0;

	strncpy(m_suBao.driver_name, pBuf->driver_name, sizeof(USERNAME)-1);
	m_suBao.driver_name[sizeof(USERNAME)-1]=0;

	strncpy(m_suBao.train_leader_name, pBuf->train_leader_name, sizeof(USERNAME)-1);
	m_suBao.train_leader_name[sizeof(USERNAME)-1]=0;

	strncpy(m_suBao.cover, pBuf->cover, 9);
	m_suBao.cover[9]=0;

	strncpy(m_suBao.tail_no, pBuf->tail_no, 9);
	m_suBao.tail_no[9]=0;
	
	m_suBao.engine_count = pBuf->engine_count;
	memcpy(m_suBao.subao_engine, pBuf->subao_engine, sizeof(m_suBao.subao_engine));
		
	m_suBao.space_rank = pBuf->space_rank;
	m_suBao.length_rank = pBuf->length_rank;
	m_suBao.super_rank = pBuf->super_rank;
	m_suBao.military_rank = pBuf->military_rank;
	m_suBao.speed_limit = pBuf->speed_limit;

	m_suBao.e_engine = pBuf->e_engine;
	m_suBao.print_flag = pBuf->print_flag;
	m_suBao.pass_cnt = pBuf->pass_cnt;
	m_suBao.load_cnt = pBuf->load_cnt;
	m_suBao.pk_count = pBuf->pk_count;
	
	m_suBao.pz_count = pBuf->pz_count;
	m_suBao.ck_count = pBuf->ck_count;
	m_suBao.cz_count = pBuf->cz_count;
	m_suBao.pik_count = pBuf->pik_count;
	m_suBao.piz_count = pBuf->piz_count;

	m_suBao.gk_count = pBuf->gk_count;
	m_suBao.gz_count = pBuf->gz_count;
	m_suBao.o_count = pBuf->o_count;
	m_suBao.s_count = pBuf->s_count;
	m_suBao.total_count = pBuf->total_count;

	m_suBao.van_count = pBuf->van_count;
	m_suBao.subao_type = pBuf->subao_type;
	m_suBao.noutil_cnt = pBuf->noutil_cnt;
	m_suBao.speed = pBuf->speed;
	m_suBao.cal_len = pBuf->cal_len;

	m_suBao.total_weight = pBuf->total_weight;

	ShowSubao();
	return;
}

//速报的修改
BOOL CSuBaoDlg::SendSubaoMsg(ushort oper)
{
	CTG_ScheduleInfo info;
	m_pDoc->GetScheduleInfo(info);
	
	CTime t=CTime::GetCurrentTime();
	m_suBao.input_time = t.GetTime();
	m_suBao.con_id =  info.m_nSectionID;
	m_suBao.schd_type = info.m_nType;
	m_suBao.oper =(CLBTH::MsgSubaoFormat::SUBAO_OPER)oper;
	
	long eid=GetSpecialEntityID(SERVER_TYPE_TG, m_suBao.con_id);
	SendTGBMsg(m_suBao.con_id,&m_suBao,eid);	
	GpcLogTrainMessage(0, 0, "[修改列车速报]: train_index %d train_no %s  svr_entity %d con_id %d shift_id:%d schd_type %d oper %d(1:增加 2:修改 3:删除) space_rank %d",
		m_suBao.train_index, 
		m_suBao.train_id,
		eid, 
		m_suBao.con_id, 
		m_suBao.shift_id,
		m_suBao.schd_type,
		m_suBao.oper,
		m_suBao.space_rank);

	return TRUE;
}

void CSuBaoDlg::OnBnClickedButton1()
{
	CString engineid, pref;
	int pos = m_List.GetItemCount();
	if(pos >= 6)
	{
		CString tempStr;
		tempStr.Format("最多只能挂%d个机车",6);
		AfxMessageBox(tempStr);	
		return;
	}

	int nSel = m_enginetype.GetCurSel();
	if(nSel==-1)
	{
		AfxMessageBox("请选择机车类型");	
		return;
	}
	m_enginetype.GetWindowText(pref);
	m_List.InsertItem(pos, pref);

	int type = m_enginetype.GetItemData(nSel);
	pref.Format("%.3d", type);
	m_train_engineid.GetWindowText(engineid);
	engineid = pref+engineid;
	m_List.SetItemText(pos, 1, engineid);

	m_tpye.GetWindowText(pref);
	m_List.SetItemText(pos, 2, pref);
}

void CSuBaoDlg::OnBnClickedButton2()
{
	if(m_sel < 0)
		return;

	CString pref="",engineid;
	int nSel = m_enginetype.GetCurSel();
	if(nSel==-1)
	{
		AfxMessageBox("请选择机车类型");	
		return;
	}
	m_enginetype.GetWindowText(pref);
	m_List.SetItemText(m_sel,0, pref);

	int type = m_enginetype.GetItemData(nSel);
	pref.Format("%.3d", type);
	m_train_engineid.GetWindowText(engineid);
	engineid = pref+engineid;
	m_List.SetItemText(m_sel, 1, engineid);

	m_tpye.GetWindowText(pref);
	m_List.SetItemText(m_sel, 2, pref);
}

void CSuBaoDlg::OnBnClickedButton4()
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int index = m_List.GetNextSelectedItem(pos);
	m_List.DeleteItem(index);
}
