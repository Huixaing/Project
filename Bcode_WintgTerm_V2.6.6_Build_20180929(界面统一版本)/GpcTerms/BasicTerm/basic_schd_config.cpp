#include "StdAfx.h"
#include ".\basic_schd_config.h"
#include ".\tg_schedule_base.h"

SECTION_BASIC_NAME::SECTION_BASIC_NAME()
{
	nSelBackupSchdIndex=-1;
	Init(0);
}

void SECTION_BASIC_NAME::Init(int conid)
{
	memset(&basic_schd, 0, sizeof(basic_schd));
	memset(&basic_prev_schd, 0, sizeof(basic_prev_schd));
	memset(basic_backup_schd, 0, sizeof(basic_backup_schd));
	con_id = conid;
	num=0;
}

basic_schd_config::basic_schd_config(void)
{
	for(int i=0; i<64; i++)
	{
		m_section_basic_name[i].Init(0);
	}
}

basic_schd_config::~basic_schd_config(void)
{
}

SECTION_BASIC_NAME* basic_schd_config::NewBasicSchdNameStruct()
{
	for(int i=0; i<64; i++)
	{
		if(m_section_basic_name[i].con_id==0)
		{
			return &m_section_basic_name[i];
		}
	}

	return NULL;
}

SECTION_BASIC_NAME* basic_schd_config::GetBasicSchdNameStruct(int con_id)
{
	for(int i=0; i<64; i++)
	{
		if(m_section_basic_name[i].con_id==con_id)
		{
			return &m_section_basic_name[i];
		}
	}

	return NULL;
}

void basic_schd_config::SetCurSelBackupBasicTypeIndex(int con_id, int schd_type)
{
	SECTION_BASIC_NAME* pbasic_name_struct = GetBasicSchdNameStruct(con_id);
	if(pbasic_name_struct==NULL)
		return;

	pbasic_name_struct->nSelBackupSchdIndex=-1;
	for(int i=0; i<12; i++)
	{
		if(schd_type==pbasic_name_struct->basic_backup_schd[i].type)
		{
			pbasic_name_struct->nSelBackupSchdIndex=i;
			break;
		}
	}
}

int basic_schd_config::GetNewBackupBasicType(int con_id)
{
	SECTION_BASIC_NAME* pbasic_name_struct = GetBasicSchdNameStruct(con_id);
	if(pbasic_name_struct==NULL)
		return -1;

	int num=pbasic_name_struct->num;
	for(int i=0; i<12; i++)
	{
		int n=0;
		for(; n<num; n++)
		{
			if((21+i)==pbasic_name_struct->basic_backup_schd[n].type)
				break;
		}

		if(n>=num)
		{
			return 21+i;
		}
	}

	return -1;
}

int basic_schd_config::GetCurSelBackupBasicType(int con_id)
{
	SECTION_BASIC_NAME* pbasic_name_struct = GetBasicSchdNameStruct(con_id);
	if(pbasic_name_struct==NULL)
		return -1;

	int nSelBackupSchdIndex = pbasic_name_struct->nSelBackupSchdIndex;
	if(0<=nSelBackupSchdIndex && nSelBackupSchdIndex<12)
	{
		return pbasic_name_struct->basic_backup_schd[nSelBackupSchdIndex].type;
	}
	return -1;
}

CString basic_schd_config::GetBasicNameByType(int con_id, int schd_type)
{
	SECTION_BASIC_NAME* pbasic_name_struct = GetBasicSchdNameStruct(con_id);
	if(pbasic_name_struct==NULL)
		return "";

	CString name="";
	if(schd_type == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		name=pbasic_name_struct->basic_schd.name;
	}
	else if(schd_type == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		name=pbasic_name_struct->basic_prev_schd.name;
	}
	else if(schd_type == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		int nSelBackupSchdIndex = pbasic_name_struct->nSelBackupSchdIndex;
		if(0<=nSelBackupSchdIndex && nSelBackupSchdIndex<12)
		{
			name=pbasic_name_struct->basic_backup_schd[nSelBackupSchdIndex].name;
		}
	}
	
	return name;
}

CString basic_schd_config::GetBasicNameTime(int con_id, int schd_type)
{
	SECTION_BASIC_NAME* pbasic_name_struct = GetBasicSchdNameStruct(con_id);
	if(pbasic_name_struct==NULL)
		return "";

	CString name="";
	if(schd_type == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		CTime tm(pbasic_name_struct->basic_schd.preusetm);
		name = "启用时间:"+tm.Format("%Y-%m-%d %H:%M");
	}
	else if(schd_type == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		CTime tm(pbasic_name_struct->basic_prev_schd.preusetm);
		name = "预计投入时间:"+tm.Format("%Y-%m-%d %H:%M");
	}
	else if(schd_type == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		int nSelBackupSchdIndex = pbasic_name_struct->nSelBackupSchdIndex;

		if(0<=nSelBackupSchdIndex && nSelBackupSchdIndex<12)
		{
			CTime tm(pbasic_name_struct->basic_backup_schd[nSelBackupSchdIndex].starttm),
				  tm1(pbasic_name_struct->basic_backup_schd[nSelBackupSchdIndex].endtm);
			name = "启用时间:"+tm.Format("%Y-%m-%d %H:%M") + "终止时间:"+tm1.Format("%Y-%m-%d %H:%M");
		}
	}
	
	return name;
}

BOOL basic_schd_config::InitBasicSchdName(int con_id)
{
	SECTION_BASIC_NAME* pbasic_name_struct = GetBasicSchdNameStruct(con_id);
	if(pbasic_name_struct==NULL)
	{
		pbasic_name_struct = NewBasicSchdNameStruct();
		if(pbasic_name_struct==NULL)
			return FALSE;
	}

	pbasic_name_struct->Init(con_id);
	
	return TRUE;
}

BOOL basic_schd_config::SetBasicSchdName(int con_id, int schd_type, char* name, ULONG sttm, ULONG edtm, ULONG tm)
{
	SECTION_BASIC_NAME* pbasic_name_struct = GetBasicSchdNameStruct(con_id);
	if(pbasic_name_struct==NULL)
		return FALSE;

	if(schd_type == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		pbasic_name_struct->basic_schd.starttm=sttm;
		pbasic_name_struct->basic_schd.endtm=edtm;
		pbasic_name_struct->basic_schd.preusetm=tm;

		pbasic_name_struct->basic_schd.type=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;
		memset(pbasic_name_struct->basic_schd.name, 0, MAX_SCHEDULE_NAME_LEN);
		strncpy(pbasic_name_struct->basic_schd.name, name, MAX_SCHEDULE_NAME_LEN-1);
		
		return TRUE;
	}
	else if(schd_type == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		pbasic_name_struct->basic_prev_schd.starttm=sttm;
		pbasic_name_struct->basic_prev_schd.endtm=edtm;
		pbasic_name_struct->basic_prev_schd.preusetm=tm;

		pbasic_name_struct->basic_prev_schd.type=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV;
		memset(pbasic_name_struct->basic_prev_schd.name, 0, MAX_SCHEDULE_NAME_LEN);
		strncpy(pbasic_name_struct->basic_prev_schd.name, name, MAX_SCHEDULE_NAME_LEN-1);

		return TRUE;
	}
	else
	{
		int i=0;
		// 更新
		for(i=0; i<12; i++)
		{
			if(pbasic_name_struct->basic_backup_schd[i].type==schd_type)
			{
				pbasic_name_struct->basic_backup_schd[i].starttm=sttm;
				pbasic_name_struct->basic_backup_schd[i].endtm=edtm;
				pbasic_name_struct->basic_backup_schd[i].type=schd_type;
				memset(pbasic_name_struct->basic_backup_schd[i].name, 0, MAX_SCHEDULE_NAME_LEN);
				strncpy(pbasic_name_struct->basic_backup_schd[i].name, name, MAX_SCHEDULE_NAME_LEN-1);
				break;
			}
		}
		if(i<12) 
		{
			return TRUE;
		}
		// 新增
		for(i=0; i<12; i++)
		{
			if(pbasic_name_struct->basic_backup_schd[i].type==0)
			{
				pbasic_name_struct->num++;
				pbasic_name_struct->basic_backup_schd[i].starttm=sttm;
				pbasic_name_struct->basic_backup_schd[i].endtm=edtm;
				pbasic_name_struct->basic_backup_schd[i].type=schd_type;
				memset(pbasic_name_struct->basic_backup_schd[i].name, 0, MAX_SCHEDULE_NAME_LEN);
				strncpy(pbasic_name_struct->basic_backup_schd[i].name, name, MAX_SCHEDULE_NAME_LEN-1);

				return TRUE;
			}
		}
	}
	
	return FALSE;
}