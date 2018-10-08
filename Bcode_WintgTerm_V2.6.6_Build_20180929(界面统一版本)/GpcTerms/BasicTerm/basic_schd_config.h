#pragma once

struct SCHD_NAME
{
	int  type;
	char name[256];
	ULONG starttm;
	ULONG endtm;
	ULONG preusetm;
};

class SECTION_BASIC_NAME
{
public:
	SECTION_BASIC_NAME();
	void Init(int conid);

public:
	int  con_id;
	int  num;
	int  nSelBackupSchdIndex;

	SCHD_NAME basic_schd;
	SCHD_NAME basic_prev_schd;
	SCHD_NAME basic_backup_schd[12];
};

class basic_schd_config
{
public:
	basic_schd_config(void);
	~basic_schd_config(void);

public:
	SECTION_BASIC_NAME* GetBasicSchdNameStruct(int con_id);

	int  GetCurSelBackupBasicType(int con_id);
	void SetCurSelBackupBasicTypeIndex(int con_id, int schd_type);

	int  GetNewBackupBasicType(int con_id);
	
	CString GetBasicNameByType(int con_id, int schd_type);
	CString GetBasicNameTime(int con_id, int schd_type);

	BOOL InitBasicSchdName(int con_id);
	BOOL SetBasicSchdName(int con_id, int schd_type, char* name, ULONG sttm, ULONG edtm, ULONG tm);

private:
	SECTION_BASIC_NAME* NewBasicSchdNameStruct();

private:
	SECTION_BASIC_NAME m_section_basic_name[64];
};
