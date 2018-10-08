#include "stdafx.h"
#include "stationDef.h"

#define  STATION_TOKEN       "[station_no]"
#define  SIDE_TOKEN          "side"
#define  SIDE_GROUP_TOKEN    "SideGroup"
#define  ENTRY_GROUP_TOKEN   "OtherMainSide"
#define  ENTRY_TOKEN         "entry"
#define  NO_CRH_SIDE         "no_crh_side"

#define  IS_VALID_SIDE(x)  (x>0 && x<255) // 共有四处

CStationDef::CStationDef(STATION_NO station_no,  const char* strFileName)
	:m_nNo(station_no)
{
	init();
	FLoadStation(strFileName);
}

CStationDef::~CStationDef()
{
	if(NULL != m_side)
	{
		delete[] m_side;
		m_side = NULL;
	}

	if ( NULL != m_entry )
	{
		delete [] m_entry;
		m_entry = NULL;
	}

	if ( NULL != m_back_group )
	{
		delete [] m_back_group;
		m_back_group = NULL;
	}
}

STATION_NO CStationDef::GetStationNo()
{
	return m_nNo;
}

void CStationDef::init()
{
	m_nSideCnt = 0;
	m_nCurSide = 0;
	m_nReceDeptSideCnt = 0;

	m_side  = NULL;
	m_entry = NULL;
	m_back_group = NULL;

	m_nCurEntry  = 0;
	m_nEntryCnt  = 0;

	m_nBackupCnt = 0;
	m_nCurBackupGroup = 0;
}

short CStationDef::ASSERT_SIDE(SIDE_NO side_no)
{
	if(side_no <= 0) return STA_FAILED;
	SIDE_T side;
	if(0 == GetSideInfo(side_no, side)) return STA_FAILED;
	return STA_SUCCESS;
}

CStationDef::READ_FILE_RESULT CStationDef::GetReadFileRs(short *line_no)
{
	if (NULL != line_no) *line_no = m_LineNo;

	return m_LoadFlag;
}
const char* CStationDef::GetReadFileReason(READ_FILE_RESULT rs)
{
	     if (READ_FILE_SUCC                   == rs) return "READ_FILE_SUCC";
	else if (OPEN_FILE_ERR                    == rs) return "OPEN_FILE_ERR";
	else if (TOO_LONG_LINE                    == rs) return "TOO_LONG_LINE";
	else if (STATION_NOT_FOUND                == rs) return "STATION_NOT_FOUND";
	else if (READ_UNKNOWN_SECTION             == rs) return "READ_UNKNOWN_SECTION";
	else if (READ_STA_NO                      == rs) return "READ_STA_NO";
	else if (READ_STA_NAME                    == rs) return "READ_STA_NAME";
	else if (READ_TLE_NAME                    == rs) return "READ_TLE_NAME";
	else if (READ_STA_PROP                    == rs) return "READ_STA_PROP";
	else if (READ_SIDE_CNT                    == rs) return "READ_SIDE_CNT";
	else if (READ_STA_GRADE                   == rs) return "READ_STA_GRADE";
	else if (READ_STA_TECK                    == rs) return "READ_STA_TECK";
	else if (READ_STA_OPER                    == rs) return "READ_STA_OPER";
	else if (READ_STA_BACK_GROUP              == rs) return "READ_STA_SIDE_GROUP";
	else if (READ_STA_ENTRY_CNT               == rs) return "READ_STA_ENTRY_CNT";
	else if (READ_SIDE_CNT_SMALL              == rs) return "READ_SIDE_CNT_SMALL";
	else if (READ_SIDE_OVERFLOW               == rs) return "READ_SIDE_OVERFLOW";
	else if (READ_SIDE_NO                     == rs) return "READ_SIDE_NO";
	else if (READ_SIDE_NO_DUPLICATE           == rs) return "READ_SIDE_NO_DUPLICATE";
	else if (READ_SIDE_DISPLAY_NAME           == rs) return "READ_SIDE_DISPLAY_NAME";
	else if (READ_SIDE_TLE_NAME               == rs) return "READ_SIDE_TLE_NAME";
	else if (READ_SIDE_IN_TLE                 == rs) return "READ_SIDE_IN_TLE";
	else if (READ_SIDE_PROP                   == rs) return "READ_SIDE_PROP";
	else if (READ_SIDE_REGION                 == rs) return "READ_SIDE_REGION";
	else if (READ_SIDE_DIR                    == rs) return "READ_SIDE_DIR";
	else if (READ_SIDE_PRIO_DIR               == rs) return "READ_SIDE_PRIO_DIR";
	else if (READ_SIDE_TRAIN_TYPE             == rs) return "READ_SIDE_TRAIN_TYPE";
	else if (READ_SIDE_EXCEED                 == rs) return "READ_SIDE_EXCEED";
	else if (READ_SIDE_LEN                    == rs) return "READ_SIDE_LEN";
	else if (READ_SIDE_STORE_CAR              == rs) return "READ_SIDE_STORE_CAR";
	else if (READ_SIDE_CATENARY               == rs) return "READ_SIDE_CATENARY";
	else if (READ_SIDE_PLATFORM               == rs) return "READ_SIDE_PLATFORM";
	else if (READ_SIDE_PASSENGER              == rs) return "READ_SIDE_PASSENGER";
	else if (READ_SIDE_UPSLOPE                == rs) return "READ_SIDE_UPSLOPE";
	else if (READ_SIDE_CURVE                  == rs) return "READ_SIDE_CURVE";
	else if (READ_SIDE_START                  == rs) return "READ_SIDE_START";
	else if (READ_SIDE_SEPARATE               == rs) return "READ_SIDE_SEPARATE";
	else if (READ_SIDE_CIRCUIT                == rs) return "READ_SIDE_CIRCUIT";
	else if (READ_SIDE_SPEEDLIMIT             == rs) return "READ_SIDE_SPEEDLIMIT";
	else if (READ_SIDE_ENTRY1                 == rs) return "READ_SIDE_ENTRY1";
	else if (READ_SIDE_ENTRY2                 == rs) return "READ_SIDE_ENTRY2";
	else if (READ_SIDE_NEIGBOR1               == rs) return "READ_SIDE_NEIGBOR1";
	else if (READ_SIDE_WIDTH1                 == rs) return "READ_SIDE_WIDTH1";
	else if (READ_SIDE_NEIGBOR2               == rs) return "READ_SIDE_NEIGBOR2";
	else if (READ_SIDE_WIDTH2                 == rs) return "READ_SIDE_WIDTH2";
	else if (READ_SIDE_SHUNT_SLIDE            == rs) return "READ_SIDE_SHUNT_SLIDE";
	else if (READ_SIDE_SHUNT_REQ              == rs) return "READ_SIDE_SHUNT_REQ";
	else if (READ_SIDE_SHUNT_EXCETIME         == rs) return "READ_SIDE_SHUNT_EXCETIME";
	else if (READ_SIDE_SHUNT_LIGHT            == rs) return "READ_SIDE_SHUNT_LIGHT";
	else if (READ_SIDE_SHUNT_WATER            == rs) return "READ_SIDE_SHUNT_WATER";
	else if (READ_SIDE_SHUNT_FREIGHT          == rs) return "READ_SIDE_SHUNT_FREIGHT";
	else if (READ_SIDE_SHUNT_PREVENT          == rs) return "READ_SIDE_SHUNT_PREVENT";
	else if (READ_SIDE_SHUNT_REMARK           == rs) return "READ_SIDE_SHUNT_REMARK";
	else if (READ_SIDE_ENTRY_EXIST            == rs) return "READ_SIDE_ENTRY_EXIST";
	else if (READ_ENTRY_CNT_SMALL             == rs) return "READ_ENTRY_CNT_SMALL";
	else if (READ_ENTRY_OVERFLOW              == rs) return "READ_ENTRY_OVERFLOW";
	else if (READ_ENTRY_NO                    == rs) return "READ_ENTRY_NO";
	else if (READ_ENTRY_NO_DUPLICATE          == rs) return "READ_ENTRY_NO_DUPLICATE";
	else if (READ_ENTRY_NAME                  == rs) return "READ_ENTRY_NAME";
	else if (READ_ENTRY_CATENARY              == rs) return "READ_ENTRY_CATENARY";
	else if (READ_ENTRY_DIR                   == rs) return "READ_ENTRY_DIR";
	else if (READ_ENTRY_DISCONN_SIDE_OVERFLOW == rs) return "READ_ENTRY_DISCONN_SIDE_OVERFLOW";
	else if (READ_BACKUP_CNT_SMALL            == rs) return "READ_SIDE_GROUP_CNT_SMALL";
	else if (READ_BACKUP_OVERFLOW             == rs) return "READ_SIDE_GROUP_OVERFLOW";
	else if (READ_BACKUP_TRAINTYPE            == rs) return "READ_SIDE_GROUP_TRAINTYPE";
	else if (READ_BACKUP_MAIN_SIDE_NO         == rs) return "READ_SIDE_GROUP_MAIN_SIDE_NO";
	else if (READ_BACKUP_SIDE_OVERFLOW        == rs) return "READ_SIDE_GROUP_BACK_OVERFLOW";
	else if (READ_OMSIDE_OVERFLOW             == rs) return "READ_OMSIDE_OVERFLOW";
	else if (READ_OMSIDE_ID                   == rs) return "READ_OMSIDE_ID";
	else if (READ_OMSIDE_ENTRY1               == rs) return "READ_OMSIDE_ENTRY1";
	else if (READ_OMSIDE_ENTRY2               == rs) return "READ_OMSIDE_ENTRY2";
	else if (READ_OMSIDE_NOT_EXIST            == rs) return "READ_OMSIDE_NOT_EXIST";
	else return "UNKNOWN";
}
short CStationDef::GetStationName(STAT_NAME& station_name)
{
	memset(station_name, 0, sizeof(station_name));
	strncpy(station_name, m_sName, sizeof(station_name)-1);
	return STA_SUCCESS;
}

bool CStationDef::IsValidEntry(ENTRY_ID entry_id) const
{
	ENTRY_T entry;
	if (STA_SUCCESS == GetEntryInfo(entry_id, entry))
		return true;
	return false;
}

short CStationDef::GetSideInfo(SIDE_NO side_no, SIDE_T& side) const
{
	memset(&side, 0, sizeof(side));

	for(int i = 0; i < m_nSideCnt; i++)
	{
		if(side_no == m_side[i].no)
		{
			memcpy(&side, &m_side[i], sizeof(SIDE_T));
			return STA_SUCCESS;
		}
	}
	return STA_FAILED;
}

short CStationDef::GetSideInfo(SIDE_NAME side_name, SIDE_T& side) const
{
	memset(&side, 0, sizeof(side));
	if(strlen(side_name) < 1) return STA_FAILED;
	for(int nIndex=0; nIndex < m_nSideCnt; nIndex++)
	{
		if(!strncmp(side_name, m_side[nIndex].display_name, strlen(side_name)))
		{
			GetSideInfo(m_side[nIndex].no, side);
			return true;
		}
	}
	return STA_FAILED;
}

short CStationDef::GetSideNo(int no, int& sideNo) const // add new linecfg
{
	for(int i = 0; i < m_nSideCnt; i++)
	{
		if(no == i)
		{
			sideNo = m_side[i].no;
			return STA_SUCCESS;
		}
	}
	return STA_FAILED;
}

short CStationDef::GetEntryInfo(ENTRY_ID entry_id, ENTRY_T& entry) const
{
	memset(&entry, 0, sizeof(entry));
	for ( int i = 0; i < m_nEntryCnt; i++ )
	{
		if ( entry_id == m_entry[i].entry_id )
		{
			memcpy(&entry, &m_entry[i], sizeof(ENTRY_T));
			return STA_SUCCESS;
		}
	}
	return STA_FAILED;
}

short CStationDef::GetSideInfoByTleName(SIDE_NAME side_name, SIDE_T& side) const
{
	memset(&side, 0, sizeof(side));
	if(strlen(side_name) < 1) return STA_FAILED;
	for(int nIndex=0; nIndex < m_nSideCnt; nIndex++)
	{
		if(!strncmp(side_name, m_side[nIndex].tle_side_name, strlen(side_name)))
		{
			GetSideInfo(m_side[nIndex].no, side);
			return true;
		}
	}
	return STA_FAILED;
}

short CStationDef::GetSideCount()
{
	return m_nSideCnt;
}

void CStationDef::_ResetReadCount()
{
	m_nSideCnt   = 0;
	m_nBackupCnt = 0;
	m_nEntryCnt  = 0;
}
void CStationDef::_ResetReadCursor()
{
	m_nCurSide = 0;
	m_nCurEntry = 0;
	m_nCurBackupGroup=0;
}
void CStationDef::_CreateStruct()
{
	int i;

	m_side       = new SIDE_T[m_nSideCnt];
	m_back_group = new MAIN_SIDE_BACKUP_GROUP[m_nBackupCnt];
	if (m_nEntryCnt > 0 )
		m_entry  = new ENTRY_T[m_nEntryCnt];

	for ( i = 0; i < m_nSideCnt; i++ )
	{
		m_side[i].no = -1;
		m_side[i].rela_entry_group_count = 0;
	}
	for ( i = 0; i < m_nBackupCnt; i++ )
	{
		m_back_group[i].main_side_no = -1;
		m_back_group[i].count = 0;
	}
	for ( i = 0; i < m_nEntryCnt; i++ )
	{
		m_entry[i].entry_id = -1;
		m_entry[i].count = 0;
	}

	_ResetReadCursor();
}

#define READ_ONE_LINE                                                      \
{                                                                          \
	m_LineNo ++;                                                           \
	if (NULL == (ptr = fgets(strLine, sizeof(strLine), fpSide))) continue; \
	if (strlen(strLine) == sizeof(strLine)-1)                              \
	{                                                                      \
		m_LoadFlag = TOO_LONG_LINE;                                        \
		rs = STA_FAILED;                                                   \
		break;                                                             \
	}                                                                      \
	while (*ptr == ' ' || *ptr == '\t') ptr ++;                            \
	if (*ptr == ';' || *ptr == '*' || *ptr == 0 || *ptr == '\n' || *ptr == '\r') continue; \
}

short CStationDef::FLoadStation(const char *szFileName)
{
	FILE *fpSide = fopen(szFileName, "r");
	if (NULL == fpSide)
	{
		m_LoadFlag = OPEN_FILE_ERR;
		return STA_FAILED;
	}

	char *ptr, strLine[1024];
	short  rs = STA_SUCCESS;
	m_LineNo = 0;

	while (!feof(fpSide))
	{
		READ_ONE_LINE;

		if ((*ptr != '[')
		  || strncmp(ptr, STATION_TOKEN, strlen(STATION_TOKEN))) 
			continue;

		ptr += strlen(STATION_TOKEN);
		STATION_NO sta = (STATION_NO)-1;

		_ResetReadCount();
		rs = _FLoadStation(ptr, sta);
		if (sta != m_nNo) 
		{
			m_LoadFlag = STATION_NOT_FOUND;
			rs = STA_FAILED;
			continue;
		}
		else if (STA_FAILED == rs) 
		{
			break;
		}

		_CreateStruct();

		// load all related data
		while (!feof(fpSide))
		{
			READ_ONE_LINE;

			if (*ptr == '[') 
			{
				 // loop to next station
				break;
			}
			else if (!strncmp(ptr, SIDE_TOKEN, strlen(SIDE_TOKEN)))
			{
				rs = _FLoadStationSide(ptr+strlen(SIDE_TOKEN));
				if (STA_FAILED == rs) break;
			}
			else if (!strncmp(ptr, SIDE_GROUP_TOKEN, strlen(SIDE_GROUP_TOKEN)))
			{
				rs = _FLoadBackupGroup(ptr+strlen(SIDE_GROUP_TOKEN));
				if (STA_FAILED == rs) break;
			}
			else if (!strncmp(ptr, ENTRY_GROUP_TOKEN, strlen(ENTRY_GROUP_TOKEN)))
			{
				rs = _FLoadEntryGroup(ptr+strlen(ENTRY_GROUP_TOKEN));
				if (STA_FAILED == rs) break;
			}
			else if (!strncmp(ptr, ENTRY_TOKEN, strlen(ENTRY_TOKEN)))
			{
				rs = _FLoadStationEntry(ptr+strlen(ENTRY_TOKEN));
				if (STA_FAILED == rs) break;
			}
			else if (!strncmp(ptr, NO_CRH_SIDE, strlen(NO_CRH_SIDE)))
			{
				rs = _FLoadNotCRHSide(ptr+strlen(NO_CRH_SIDE));
				if (STA_FAILED == rs) break;
			}
/*		else
			{
				m_LoadFlag = READ_UNKNOWN_SECTION;
				rs = STA_FAILED;
			}*/
		}

		break;
	}

	fclose(fpSide);

	if (STA_FAILED == rs)
		return STA_FAILED;

	if (m_nSideCnt != m_nCurSide)
	{
		m_LoadFlag = READ_SIDE_CNT_SMALL;
		return STA_FAILED;
	}

	if (m_nEntryCnt != m_nCurEntry)
	{
		m_LoadFlag = READ_ENTRY_CNT_SMALL;
		return STA_FAILED;
	}

	if (m_nBackupCnt != m_nCurBackupGroup)
	{
		m_LoadFlag = READ_BACKUP_CNT_SMALL;
		return STA_FAILED;		
	}

	_ResetReadCursor();

	m_LoadFlag = READ_FILE_SUCC;
	return STA_SUCCESS;
}

short CStationDef::_FLoadStation(char *ptr, STATION_NO& sta)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	short sta_tmp;
	if (STA_FAILED == GET_FIRST_KEY_PROCESS(ptr, sta_tmp, seps, token))
	{
		m_LoadFlag = READ_STA_NO;
		return STA_FAILED;
	}
	sta = sta_tmp;

	if (STA_FAILED == GET_KEY_PROCESS(m_sName, seps, token))
	{
		m_LoadFlag = READ_STA_NAME;
		return STA_FAILED;
	}

	if (STA_FAILED == GET_KEY_PROCESS(m_sTleName, seps, token))
	{
		m_LoadFlag = READ_TLE_NAME;
		return STA_FAILED;
	}
	
	if (STA_FAILED == GET_KEY_PROCESS(m_nProperty, seps, token))
	{
		m_LoadFlag = READ_STA_PROP;
		return STA_FAILED;
	}
	
	if (STA_FAILED == GET_KEY_PROCESS(m_nSideCnt, seps, token))
	{
		m_LoadFlag = READ_SIDE_CNT;
		return STA_FAILED;
	}
	
	if (STA_FAILED == GET_KEY_PROCESS(m_nGrade, seps, token))
	{
		m_LoadFlag = READ_STA_GRADE;
		return STA_FAILED;
	}
	
	if (STA_FAILED == GET_KEY_PROCESS(m_nTechnical, seps, token))
	{
		m_LoadFlag = READ_STA_TECK;
		return STA_FAILED;
	}
	
	if (STA_FAILED == GET_KEY_PROCESS(m_nOperation, seps, token))
	{
		m_LoadFlag = READ_STA_OPER;
		return STA_FAILED;
	}
	
	if (STA_FAILED == GET_KEY_PROCESS(m_nBackupCnt, seps, token))
	{
		m_LoadFlag = READ_STA_BACK_GROUP;
		m_nBackupCnt = 0;
	}

	if (STA_FAILED == GET_KEY_PROCESS(m_nEntryCnt, seps, token))
	{
		m_LoadFlag = READ_STA_ENTRY_CNT;
		m_nEntryCnt = 0;
	}

	return STA_SUCCESS;
}

short CStationDef::_FLoadStationSide(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	
	if(m_nCurSide >= m_nSideCnt)
	{
		m_LoadFlag = READ_SIDE_OVERFLOW;
		return STA_FAILED;
	}

	SIDE_NO no;
	if (STA_FAILED == GET_FIRST_KEY_PROCESS(ptr, no, seps, token))
	{
		m_LoadFlag = READ_SIDE_NO;
		return STA_FAILED;
	}
	for(int i=0;i<m_nCurSide;i++)
	{
		if(no == m_side[i].no)
		{
			m_LoadFlag = READ_SIDE_NO_DUPLICATE;
			return STA_FAILED;
		}
	}

	if(!IS_VALID_SIDE(no))  // 20161105 读取站细时,遇到不合法股道报警
	{
		CString str;
		str.Format("在stationside.def配置文件中, 车站 %s 的股道号非法(合法值是1<= side <= 254),请修改.", m_sName);
		AfxMessageBox(str);
		return STA_FAILED;
	}

	m_side[m_nCurSide].no = no;

	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].display_name, seps, token))
	{
		m_LoadFlag = READ_SIDE_DISPLAY_NAME;
		return STA_FAILED;
	}

	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].tle_side_name, seps, token))
	{
		m_LoadFlag = READ_SIDE_TLE_NAME;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].is_tle_side, seps, token))
	{
		m_LoadFlag = READ_SIDE_IN_TLE;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].side_property, seps, token))
	{
		m_LoadFlag = READ_SIDE_PROP;
		return STA_FAILED;
	}

	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].side_region_flag, seps, token))
	{
		m_LoadFlag = READ_SIDE_REGION;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].rece_depa_dir, seps, token))
	{
		m_LoadFlag = READ_SIDE_DIR;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].prio_rece_depa_dir, seps, token))
	{
		m_LoadFlag = READ_SIDE_PRIO_DIR;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].rece_train_type, seps, token))
	{
		m_LoadFlag = READ_SIDE_TRAIN_TYPE;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].exceed_delimitation, seps, token))
	{
		m_LoadFlag = READ_SIDE_EXCEED;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].len, seps, token))
	{
		m_LoadFlag = READ_SIDE_LEN;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].max_store_car_cnt, seps, token))
	{
		m_LoadFlag = READ_SIDE_STORE_CAR;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].catenary, seps, token))
	{
		m_LoadFlag = READ_SIDE_CATENARY;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].platform, seps, token))
	{
		m_LoadFlag = READ_SIDE_PLATFORM;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].passenger_equipment, seps, token))
	{
		m_LoadFlag = READ_SIDE_PASSENGER;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].up_slope, seps, token))
	{
		m_LoadFlag = READ_SIDE_UPSLOPE;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].curve_radius, seps, token))
	{
		m_LoadFlag = READ_SIDE_CURVE;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].starting_difficuent, seps, token))
	{
		m_LoadFlag = READ_SIDE_START;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].separate, seps, token))
	{
		m_LoadFlag = READ_SIDE_SEPARATE;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].circuit, seps, token))
	{
		m_LoadFlag = READ_SIDE_CIRCUIT;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].speed_limit, seps, token))
	{
		m_LoadFlag = READ_SIDE_SPEEDLIMIT;
		return STA_FAILED;
	}

	if (m_side[m_nCurSide].rela_entry_group_count > 0)
	{
		m_LoadFlag = READ_SIDE_ENTRY_EXIST;
		return STA_FAILED;
	}
	m_side[m_nCurSide].rela_entry_group_count = 1;
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].rela_entry_group[0].entry_1, seps, token))
	{
		m_LoadFlag = READ_SIDE_ENTRY1;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].rela_entry_group[0].entry_2, seps, token))
	{
		m_LoadFlag = READ_SIDE_ENTRY2;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].neighbor_side_1, seps, token))
	{
		m_LoadFlag = READ_SIDE_NEIGBOR1;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].width_1, seps, token))
	{
		m_LoadFlag = READ_SIDE_WIDTH1;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].neighbor_side_2, seps, token))
	{
		m_LoadFlag = READ_SIDE_NEIGBOR2;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(m_side[m_nCurSide].width_2, seps, token))
	{
		m_LoadFlag = READ_SIDE_WIDTH2;
		return STA_FAILED;
	}

	if(m_side[m_nCurSide].side_property == 1 || m_side[m_nCurSide].side_property == 2) m_nReceDeptSideCnt++;
	m_side[m_nCurSide].no_crh_side = 0;

	m_nCurSide++;
	return STA_SUCCESS;
}

short CStationDef::_FLoadStationEntry(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	if(m_nCurEntry >= m_nEntryCnt)
	{
		m_LoadFlag = READ_ENTRY_OVERFLOW;
		return STA_FAILED;
	}

	ENTRY_ID entry_id;
	if (STA_FAILED == GET_FIRST_KEY_PROCESS(ptr, entry_id, seps, token))
	{
		m_LoadFlag = READ_ENTRY_NO;
		return STA_FAILED;
	}
	for(int i=0;i<m_nCurEntry;i++)
	{
		if(entry_id == m_entry[i].entry_id)
		{
			m_LoadFlag = READ_ENTRY_NO_DUPLICATE;	
			break;
		}
	}

	ENTRY_T & entry = m_entry[m_nCurEntry];
	entry.entry_id = entry_id;
	entry.catenary_count = 0;

	if (STA_FAILED == GET_KEY_PROCESS(entry.entry_name, seps, token))
	{
		m_LoadFlag = READ_ENTRY_NAME;
		return STA_FAILED;
	}
	char catenary_str[50];
	memset( catenary_str, 0, sizeof(catenary_str) );
	if (STA_FAILED == GET_KEY_PROCESS(catenary_str, seps, token))
	{
		m_LoadFlag = READ_ENTRY_CATENARY;
		return STA_FAILED;
	}
	
	if (STA_FAILED == GET_KEY_PROCESS(entry.dir, seps, token))
	{
		m_LoadFlag = READ_ENTRY_DIR;
		return STA_FAILED;
	}

	entry.count = 0;
	SIDE_NO side;
	while (STA_SUCCESS == GET_KEY_PROCESS(side, seps, token))
	{
		if (entry.count >= MAX_DISCONN_SIDE_NUM)
		{
			m_LoadFlag = READ_ENTRY_DISCONN_SIDE_OVERFLOW;
			return STA_FAILED;
		}

		entry.disconn_sides[entry.count] = side;
		entry.count++;
	}

	char* cat_tok = strtok( catenary_str, "#\0" );
	while ( cat_tok != NULL && entry.catenary_count < 5 )
	{
		entry.catenary[entry.catenary_count] = atoi(cat_tok);
		if ( entry.catenary[entry.catenary_count] > 0 )
			++entry.catenary_count;
		cat_tok = strtok( NULL, "#\0" );
	}
	++m_nCurEntry;
	return STA_SUCCESS;
}

short CStationDef::_FLoadBackupGroup(char *ptr)
{
	/* SideGroup=0,  1,   3,   5 */
	char *token;
	char seps[]=" ,\t\\/=\n";

    if(m_nCurBackupGroup >= m_nBackupCnt)
	{
		m_LoadFlag = READ_BACKUP_OVERFLOW;
		return STA_FAILED;
	}
	
	MAIN_SIDE_BACKUP_GROUP &back = m_back_group[m_nCurBackupGroup];
	if (STA_FAILED == GET_FIRST_KEY_PROCESS(ptr, back.train_type, seps, token))
	{
		m_LoadFlag = READ_BACKUP_TRAINTYPE;
		return STA_FAILED;
	}

	SIDE_NO side;
	if (STA_FAILED == GET_KEY_PROCESS(side, seps, token))
	{
		m_LoadFlag = READ_BACKUP_MAIN_SIDE_NO;
		return STA_FAILED;
	}
	back.main_side_no = side;

	back.count = 0;
	while (STA_SUCCESS == GET_KEY_PROCESS(side, seps, token)) 
	{
		if (back.count >= MAX_BACKUP_SIDE_NUM)
		{
			m_LoadFlag = READ_BACKUP_SIDE_OVERFLOW;
			return STA_FAILED;
		}

		back.backup_sides[back.count] = side;
		back.count ++;
	} 

	m_nCurBackupGroup++;
	return STA_SUCCESS;
}

short CStationDef::_FLoadEntryGroup(char* ptr)
{
	/* OtherMainSide=1,   1,   5 */

	char *token;
	char seps[]=" ,\t\\/=\n";

	SIDE_NO side_id;

	if (STA_FAILED == GET_FIRST_KEY_PROCESS(ptr, side_id, seps, token))
	{
		m_LoadFlag = READ_OMSIDE_ID;
		return STA_FAILED;
	}

	ENTRY_ID entry1, entry2;

	if (STA_FAILED == GET_KEY_PROCESS(entry1, seps, token))
	{
		m_LoadFlag = READ_OMSIDE_ENTRY1;
		return STA_FAILED;
	}
	if (STA_FAILED == GET_KEY_PROCESS(entry2, seps, token))
	{
		m_LoadFlag = READ_OMSIDE_ENTRY2;
		return STA_FAILED;
	}

	for(int si = 0; si < m_nSideCnt; si++)
	{
		if ( side_id == m_side[si].no )
		{
			for ( int ei = 0; ei < m_side[si].rela_entry_group_count; ei++ )
			{
				if ( m_side[si].rela_entry_group[ei].entry_1 == entry1 && m_side[si].rela_entry_group[ei].entry_2 == entry2 ||
					m_side[si].rela_entry_group[ei].entry_1 == entry2 && m_side[si].rela_entry_group[ei].entry_2 == entry1 )
					return STA_SUCCESS;
			}

			if ( m_side[si].rela_entry_group_count >= MAX_SIDE_ENTRY_GROUP_COUNT )
			{
				m_LoadFlag = READ_OMSIDE_OVERFLOW;
				return STA_FAILED;
			}

			m_side[si].rela_entry_group[m_side[si].rela_entry_group_count].entry_1 = entry1;
			m_side[si].rela_entry_group[m_side[si].rela_entry_group_count].entry_2 = entry2;
			++m_side[si].rela_entry_group_count;
			return STA_SUCCESS;
		}
	}

	m_LoadFlag = READ_OMSIDE_NOT_EXIST;
	return STA_FAILED;
}

short CStationDef::_FLoadNotCRHSide(char* ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	SIDE_NO side_id = 0;

	if (STA_FAILED == GET_FIRST_KEY_PROCESS(ptr, side_id, seps, token))
	{
		return STA_SUCCESS;
	}

	do
	{
		for ( int i = 0; i < m_nCurSide; i++ )
		{
			if ( m_side[i].no == side_id )
			{
				m_side[i].no_crh_side = 1;
				break;
			}
		}
	}
	while (STA_SUCCESS == GET_KEY_PROCESS(side_id, seps, token));

	return STA_SUCCESS;

}

void CStationDef::ResetStation()
{
	m_nCurSide = 0;
}

short CStationDef::GetNextSideName(SIDE_NAME& side_name)
{
	if(m_nCurSide < 0 || m_nCurSide >= m_nSideCnt) return STA_FAILED;

	memset(side_name, 0x00, sizeof(SIDE_NAME));
	strncpy(side_name, m_side[m_nCurSide].display_name, sizeof(SIDE_NAME)-1);
	
	m_nCurSide++;
	return STA_SUCCESS;
}

short CStationDef::GetReceDeptSideCnt() const
{
	return m_nReceDeptSideCnt;
}

short CStationDef::GetSideProperty(SIDE_NO side_no, short& property, short& direct)
{
	SIDE_T side;																				
	if(STA_FAILED == GetSideInfo(side_no, side)) 
		return STA_FAILED;		
	
	property = side.side_property;
	direct = side.prio_rece_depa_dir;

	return STA_SUCCESS;
}

short CStationDef::GetReceDeptSideNameAndNo(SIDE_NAME* side_name, SIDE_NO* side_no, short& side_cnt) const
{
	memset(side_name, 0, sizeof(SIDE_NAME)*side_cnt);
	memset(side_no, 0, sizeof(SIDE_NO)*side_cnt);

	if(side_cnt < 1) return STA_FAILED;
	short nIndex = 0;
	short nCnt = (side_cnt <= m_nReceDeptSideCnt) ? side_cnt : m_nSideCnt;
	for(int i = 0; i < m_nSideCnt; i++)
	{
		if(m_side[i].side_property == 1 || m_side[i].side_property == 2)
		{
			strcpy(side_name[nIndex], m_side[i].display_name);
			side_no[nIndex] = m_side[i].no;
			nIndex++;
			if(nIndex >= nCnt) break;
		}
	}
	side_cnt = nIndex;
	return (nIndex > 0) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::GetReceDeptSideName(SIDE_NAME* side_name, short& side_cnt) const
{
	memset(side_name, 0, sizeof(SIDE_NAME)*side_cnt);
	if(side_cnt < 1) return STA_FAILED;
	short nIndex = 0;
	short nCnt = (side_cnt <= m_nReceDeptSideCnt) ? side_cnt : m_nSideCnt;
	for(int i = 0; i < m_nSideCnt; i++)
	{
		if(m_side[i].side_property == 1 || m_side[i].side_property == 2)
		{
			strcpy(side_name[nIndex], m_side[i].display_name);
			nIndex++;
			if(nIndex >= nCnt) break;
		}
	}
	side_cnt = nIndex;
	return (nIndex > 0) ? STA_SUCCESS : STA_FAILED;
}
short CStationDef::GetReceDeptSideNo(SIDE_NO* side_no, short& side_cnt) const
{
	memset(side_no, 0, sizeof(SIDE_NO)*side_cnt);
	if(side_cnt < 1) return STA_FAILED;
	short nIndex = 0;
	short nCnt = (side_cnt <= m_nReceDeptSideCnt) ? side_cnt : m_nSideCnt;
	for(int i = 0; i < m_nSideCnt; i++)
	{
		if(m_side[i].side_property == 1 || m_side[i].side_property == 2)
		{
			side_no[nIndex] = m_side[i].no;
			nIndex++;
			if(nIndex >= nCnt) break;
		}
	}
	side_cnt = nIndex;
	return (nIndex > 0) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::GetReceDeptSideNo(short &index, SIDE_NO& side) const
{
	if (index < 0 || index >= m_nSideCnt)
		return STA_FAILED;

	for(int i=index; i<m_nSideCnt; i++)
	{
		if(m_side[i].side_property == 1 || m_side[i].side_property == 2)
		{
			side = m_side[i].no;
			index = i+1;
			return STA_SUCCESS;
		}
	}

	index = -1;
	return STA_FAILED;
}

short CStationDef::GetSideName(SIDE_NAME* side_name, short& side_cnt)
{
	memset(side_name, 0, sizeof(side_name));
	if(side_cnt < 1) return STA_FAILED;
	short nSideCnt = (side_cnt <= m_nSideCnt) ? side_cnt : m_nSideCnt;
	if(side_cnt <= m_nSideCnt) nSideCnt = side_cnt;
	for(int i = 0; i < side_cnt; i++)
	{
		strcpy(side_name[i], m_side[i].display_name);
	}
	side_cnt = m_nSideCnt;
	return STA_SUCCESS;
}

short CStationDef::GetPreSideName(SIDE_NAME& side_name)
{
	if(m_nCurSide < 0 || m_nCurSide >= m_nSideCnt) return STA_FAILED;

	SIDE_NAME SideName;
	strcpy(SideName, m_side[m_nCurSide].display_name);
	if(strlen(SideName) < 1) return STA_FAILED;
	strcpy(side_name, SideName);

	m_nCurSide--;
	return STA_SUCCESS;
}

short CStationDef::GetSideNo(SIDE_NAME side_name, SIDE_NO& side_no) const
{
	GET_SIDE_PROPERTY_NUM(side_name, side.no, side_no);
}

short CStationDef::GetSideProperty(SIDE_NO side_no, short& side_property) const
{
	GET_SIDE_PROPERTY_NUM(side_no, side.side_property, side_property);
}

short CStationDef::GetSideName(SIDE_NO side_no, SIDE_NAME& side_name) const
{
	GET_SIDE_PROPERTY_CHAR(side_no, side.tle_side_name, side_name);
}

short CStationDef::SetCurSide(SIDE_NAME side_name)
{
	SIDE_T side;
	if(0 == GetSideInfo(side_name, side)) return STA_FAILED;

	for(int i=0; i < m_nSideCnt; i++)
	{
		if(m_side[i].no == side.no)
		{
			m_nCurSide = i;
			return STA_SUCCESS;
		}
	}
	return STA_FAILED;
}

short CStationDef::SetCurSide(SIDE_NO side_no)
{
	SIDE_T side;
	if(0 == GetSideInfo(side_no, side)) return STA_FAILED;

	for(int i=0; i < m_nSideCnt; i++)
	{
		if(m_side[i].no == side_no)
		{
			m_nCurSide = i;
			return STA_SUCCESS;
		}
	}
	return STA_FAILED;
}

short CStationDef::isTleSide(SIDE_NO side_no) const
{
	JUDGE_SIDE_PROPERTY(side_no, side.is_tle_side, 1);
}

short CStationDef::isMainSide(SIDE_NO side_no) const
{
	return isTypeOfSide(side_no, 1);
}

short CStationDef::isMainSide(SIDE_NO side_no, short entry) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	if(side.side_property != 1) return STA_FAILED;
	if(entry < 1) return STA_SUCCESS;
	return isRelaEntry(side, entry);
}

short CStationDef::isMainSide(SIDE_NO side_no, short entry, short exit) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	if(side.side_property != 1) return STA_FAILED;
	if(entry < 1 && exit < 1) return isMainSide(side_no);
	if(entry < 1 && exit >= 1) return isMainSide(side_no, exit);
	if(entry >= 1 && exit < 1) return isMainSide(side_no, entry);
	return isRelaEntryGroup(side, entry, exit);
}

short CStationDef::isStopSide(SIDE_NO side_no) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	if(side.side_property == 1) return STA_FAILED;
	return STA_SUCCESS;
}

short CStationDef::isStopSide(SIDE_NO side_no, short entry) const
{
	if(STA_FAILED == isStopSide(side_no)) return STA_FAILED;

	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	if(entry < 1) return (side.side_property == 1) ? STA_FAILED : STA_SUCCESS;
	if ( side.side_property == 1 && isRelaEntry(side, entry) )
		return STA_FAILED;
	return STA_SUCCESS;
}

short CStationDef::isStopSide(SIDE_NO side_no, short entry, short exit) const
{
	if(STA_FAILED == isStopSide(side_no)) return STA_FAILED;

	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	if(entry < 1 && exit < 1) return isStopSide(side_no);
	if(entry < 1 && exit >= 1) return isStopSide(side_no, exit);
	if(exit < 1 &&  entry >= 1) return isStopSide(side_no, entry);
	if ( side.side_property == 1 && isRelaEntryGroup(side, entry, exit) )
		return STA_FAILED;
	return STA_SUCCESS;
}

short CStationDef::isReceDeptSide(SIDE_NO side_no) const
{
	return isTypeOfSide(side_no, 2);
}

short CStationDef::isFreightSide(SIDE_NO side_no) const
{
	return isTypeOfSide(side_no, 3);
}

short CStationDef::isLocomotiveMovseSide(SIDE_NO side_no) const
{
	return isTypeOfSide(side_no, 6);
}

short CStationDef::isTypeOfSide(SIDE_NO side_no, short type) const
{
	JUDGE_SIDE_PROPERTY(side_no, side.side_property, type);
}

short CStationDef::isRegion(SIDE_NO side_no, SIDE_REGION_FLAG region) const
{
	JUDGE_SIDE_PROPERTY(side_no, side.side_region_flag, region);
}

short CStationDef::isCRHSide(SIDE_NO side_no) const
{
	JUDGE_SIDE_PROPERTY(side_no, side.no_crh_side, 0);
}

short CStationDef::isUpReceTrain(SIDE_NO side_no) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.rece_depa_dir == 1 || side.rece_depa_dir == 3)  ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isDownReceTrain(SIDE_NO side_no) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.rece_depa_dir == 2 || side.rece_depa_dir == 3)  ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isUpPrioReceTrain(SIDE_NO side_no) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.prio_rece_depa_dir == 1 || side.prio_rece_depa_dir == 3)  ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isDownPrioReceTrain(SIDE_NO side_no) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.prio_rece_depa_dir == 2 || side.prio_rece_depa_dir == 3)  ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isReceiveTrain(SIDE_NO side_no, BYTE TrainType) const
{
/*
	short nTrainType = (short)TrainType;
	if(nTrainType < 0) return STA_FAILED;
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;

	RECE_TRAIN_TYPE sideReceTrainType, train_type, train_type_1; 
	sideReceTrainType = side.rece_train_type;
	train_type = 1 << nTrainType;
	train_type_1 = sideReceTrainType & train_type;
	if(train_type_1 == train_type) return STA_SUCCESS;
	return STA_FAILED;
*/
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	if (side.side_property != 1 && side.side_property != 2)
		return STA_FAILED;
	if (side.rece_train_type == RECV_NONE)
		return STA_FAILED;
	return (side.rece_train_type <= TrainType) ? STA_SUCCESS : STA_FAILED;
//	return (side.rece_depa_dir) > 0 ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isReceiveTrain(SIDE_NO side_no, BYTE TrainType, RECEIVE_TRAIN_DIR dir) const
{
	if(STA_FAILED == isReceiveTrain(side_no, TrainType)) return STA_FAILED;
	if(dir == UP_DIR)
		return isUpReceTrain(side_no);
	else if(dir == DOWN_DIR)
		return isDownReceTrain(side_no);
	return (isUpReceTrain(side_no) == isDownReceTrain(side_no) && isUpReceTrain(side_no) == STA_SUCCESS) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isUpDepartTrain(SIDE_NO side_no) const
{
	return isUpReceTrain(side_no);
}

short CStationDef::isDownDepartTrain(SIDE_NO side_no) const
{
	return isDownReceTrain(side_no);
}

short CStationDef::isUpPrioDepartTrain(SIDE_NO side_no) const
{
	return isUpPrioReceTrain(side_no);
}

short CStationDef::isDownPrioDepartTrain(SIDE_NO side_no) const
{
	return isDownPrioReceTrain(side_no);
}

short CStationDef::isDepartTrain(SIDE_NO side_no, BYTE TrainType) const
{
	return isReceiveTrain(side_no, TrainType);
}

short CStationDef::GetSideLen(SIDE_NO side_no, short& side_len) const
{
	GET_SIDE_PROPERTY_NUM(side_no, side.len, side_len);
}

short CStationDef::GetSideMaxStoreCar(SIDE_NO side_no, short& max_store_car, short caclute_type) const
{
	SIDE_T side;
	max_store_car = 0;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	if(caclute_type == 10)
		max_store_car = side.max_store_car_cnt;
	else if(caclute_type != 13)
		return STA_FAILED;
	else
		max_store_car = (side.len < 1) ? 0 : (short)(side.len * 10 / 13);
	return STA_FAILED;
}

short CStationDef::isReceiveLengthTrain(SIDE_NO side_no, short len) const
{
	short nSideLen;
	if(STA_FAILED == GetSideLen(side_no, nSideLen)) return STA_FAILED;
	return (nSideLen >= len) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isReceiveExceedTrain(SIDE_NO side_no, short exceed_grade) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.exceed_delimitation <= exceed_grade && side.exceed_delimitation != 0) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isCatenary(SIDE_NO side_no) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.catenary > 0) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::GetSideCatenaryNumber(SIDE_NO side_no, short& nNumber) const
{
	SIDE_T side;
	if ( STA_FAILED == GetSideInfo(side_no, side) )
		return STA_FAILED;
	nNumber = side.catenary;
	return STA_SUCCESS;
}

short CStationDef::isPassenerEquipment(SIDE_NO side_no) const
{
	JUDGE_SIDE_PROPERTY(side_no, side.passenger_equipment, 1);
}

short CStationDef::isFreightEquipment(SIDE_NO side_no) const
{
	JUDGE_SIDE_PROPERTY(side_no, side.freight_equipment, 1);
}

short CStationDef::GetSideUpSlope(SIDE_NO side_no, float& slope) const
{
	GET_SIDE_PROPERTY_NUM(side_no, side.up_slope, slope);
}

short CStationDef::GetSideCurveRadius(SIDE_NO side_no, short& curve_radius) const
{
	GET_SIDE_PROPERTY_NUM(side_no, side.curve_radius, curve_radius);
}

short CStationDef::isUpStartDifficuent(SIDE_NO side_no, short weight) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.starting_difficuent == 1 || side.starting_difficuent == 3) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isDownStartDifficuent(SIDE_NO side_no, short weight) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.starting_difficuent == 2 || side.starting_difficuent == 3) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isUpSeparate(SIDE_NO side_no) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.separate == 1 || side.separate == 3) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isDownSeparate(SIDE_NO side_no) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.separate == 2 || side.separate == 3) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isCircuit(SIDE_NO side_no) const
{
	JUDGE_SIDE_PROPERTY(side_no, side.circuit, 1);
}

short CStationDef::isSpeedLimit(SIDE_NO side_no, short& speed) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	speed = side.speed_limit;
	if(speed > 0) return STA_SUCCESS;
	return STA_FAILED;
}

short CStationDef::GetNeighborSide1(SIDE_NO side_no, SIDE_NO& neighbor_side_no) const
{
	GET_SIDE_PROPERTY_NUM(side_no, side.neighbor_side_1, neighbor_side_no);
}

short  CStationDef::GetNeighborSideWidth1(SIDE_NO side_no, float width) const
{
	GET_SIDE_PROPERTY_NUM(side_no, side.width_1, width);
}

short  CStationDef::GetNeighborSide2(SIDE_NO side_no, SIDE_NO& neighbor_side_no) const
{
	GET_SIDE_PROPERTY_NUM(side_no, side.neighbor_side_2, neighbor_side_no);
}

short  CStationDef::GetNeighborSideWidth2(SIDE_NO side_no, float width) const
{
	GET_SIDE_PROPERTY_NUM(side_no, side.width_2, width);
}

short CStationDef::isPlatform(SIDE_NO side_no) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.platform > 0) ? STA_SUCCESS : STA_FAILED;
}

short CStationDef::isHighPlatform(SIDE_NO side_no) const
{
	SIDE_T side;
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;
	return (side.platform > 1) ? STA_SUCCESS : STA_FAILED;
}




short CStationDef::GetKey(short& nKey, char * ptr)
{
	nKey = 0;
	if (NULL == ptr) return STA_FAILED;

	nKey =  (short)atoi(ptr);
	return STA_SUCCESS;
}

short CStationDef::GetKey(float& fKey, char * ptr)
{
	fKey = 0;
	if (NULL == ptr) return STA_FAILED;

	fKey = (float)atof(ptr);
	return STA_SUCCESS;
}

short CStationDef::GetKey(ULONG& ulKey, char * ptr)
{
	ulKey = 0;
	if (NULL == ptr) return STA_FAILED;

	ulKey = (ULONG)atol(ptr);
	return STA_SUCCESS;
}

short CStationDef::GetKey(char* sKey, char * ptr)
{
	*sKey = 0;
	if (NULL == ptr) return STA_FAILED;

	strcpy(sKey, ptr);
	return STA_SUCCESS;
}

short CStationDef::GetEntryLineName(ENTRY_ID entry_id, char* strName) const
{
	ENTRY_T entry;
	if ( STA_SUCCESS == GetEntryInfo(entry_id, entry) )
	{
		if (NULL != strName) strcpy( strName, entry.entry_name );
		return STA_SUCCESS;
	}
	return STA_FAILED;
}

short CStationDef::isCatenaryEntry(ENTRY_ID entry_id) const
{
	ENTRY_T entry;
	if ( STA_SUCCESS == GetEntryInfo(entry_id, entry) )
	{
		return (entry.catenary > 0) ?  STA_SUCCESS : STA_FAILED;
	}
	return STA_FAILED;
}

short CStationDef::GetEntryCatenaryNumber(ENTRY_ID entry_id, short nzNumber[], int& nCount) const
{
	ENTRY_T entry;
	if ( STA_SUCCESS == GetEntryInfo(entry_id, entry) )
	{
		nCount = entry.catenary_count;
		for ( int i = 0; i < nCount; i++ )
			nzNumber[i] = entry.catenary[i];
		return STA_SUCCESS;
	}
	return STA_FAILED;
}

short CStationDef::isUpEntry(ENTRY_ID entry_id) const
{
	ENTRY_T entry;
	if ( STA_SUCCESS == GetEntryInfo(entry_id, entry) )
		if (2 == entry.dir || 4 == entry.dir) return STA_SUCCESS;

	return STA_FAILED;
}
short CStationDef::isDownEntry(ENTRY_ID entry_id) const
{
	ENTRY_T entry;
	if ( STA_SUCCESS == GetEntryInfo(entry_id, entry) )
		if (1 == entry.dir || 3 == entry.dir) return STA_SUCCESS;

	return STA_FAILED;
}
short CStationDef::isUpDownEntry(ENTRY_ID entry_id) const
{
	ENTRY_T entry;
	if ( STA_SUCCESS == GetEntryInfo(entry_id, entry) )
		if (5 == entry.dir) return STA_SUCCESS;

	return STA_FAILED;
}
short CStationDef::GetEntryDir(ENTRY_ID entry_id) const
{
	ENTRY_T entry;
	if ( STA_SUCCESS == GetEntryInfo(entry_id, entry) )
		return entry.dir;

	return 0;
}
short CStationDef::isEntrySideConnect(SIDE_NO side_no, ENTRY_ID entry_id) const
{
	ENTRY_T entry;
	if ( STA_SUCCESS == GetEntryInfo(entry_id, entry) )
	{
		for (int i=0; i<entry.count; i++)
		{
			if (entry.disconn_sides[i] == side_no)
				return STA_FAILED;
		}
		return STA_SUCCESS;
	}
	else
	{
		return STA_SUCCESS; // 没有配置认定为可以连通
	}
}
short CStationDef::GetBackupSideInGroup(short train_type,const SIDE_NO  side1, SIDE_NO backup[MAX_BACKUP_SIDE_NUM]) const
{
    int i,j,rt;
	for(i=0; i < m_nBackupCnt; i++)
	{
		if(m_back_group[i].train_type==train_type)
		{
			if(m_back_group[i].main_side_no == side1)
				break;
		    for(j=0;j<m_back_group[i].count;j++)
			{
				if(m_back_group[i].backup_sides[j]==side1)
					break;
			}
			if(j<m_back_group[i].count)
				break;
		}
	}
    if(i >= m_nBackupCnt)
		return -1;
	rt=0;
	if(m_back_group[i].main_side_no != side1)
          backup[rt++]=m_back_group[i].main_side_no;
	for(j=0;j<m_back_group[i].count;j++)
	{
		if(m_back_group[i].backup_sides[j]!=side1)
  		    backup[rt++]=m_back_group[i].backup_sides[j];
	}
	return rt;
}

short CStationDef::GetBackupSideInGroupByMainSide(short train_type,const SIDE_NO  side1, SIDE_NO backup[MAX_BACKUP_SIDE_NUM]) const
{
	int i,j,rt;
	int tmp=-1;
	for(i=0; i < m_nBackupCnt; i++)
	{
		if(m_back_group[i].train_type==train_type)
		{
			if(m_back_group[i].main_side_no == side1)
				break;

			if (-1 != tmp) continue;

		    for(j=0;j<m_back_group[i].count;j++)
			{
				if(m_back_group[i].backup_sides[j]==side1)
				{
					tmp = i;
					break;
				}
			}
		}
	}
	if(i >= m_nBackupCnt)
	{
		if (-1 == tmp) 
			return -1;
		else
			i=tmp;
	}

	rt=0;
	if(m_back_group[i].main_side_no != side1)
          backup[rt++]=m_back_group[i].main_side_no;

	for(j=0;j<m_back_group[i].count;j++)
	{
		if(m_back_group[i].backup_sides[j]!=side1)
  		    backup[rt++]=m_back_group[i].backup_sides[j];
	}
	return rt;
}

short	CStationDef::isRelaEntry( const SIDE_T& rSide, short entry ) const
{
	if ( entry < 1 )
		return STA_FAILED;
	for ( int i = 0; i < rSide.rela_entry_group_count; i++ )
	{
		if ( entry == rSide.rela_entry_group[i].entry_1 || entry == rSide.rela_entry_group[i].entry_2 )
			return STA_SUCCESS;
	}
	return STA_FAILED;
}

short	CStationDef::isRelaEntryGroup( const SIDE_T& rSide, short entry1, short entry2 ) const
{
	if ( entry1 < 1 || entry2 < 1 )
		return STA_FAILED;
	for ( int i = 0; i < rSide.rela_entry_group_count; i++ )
	{
		if ( entry1 == rSide.rela_entry_group[i].entry_1 && entry2 == rSide.rela_entry_group[i].entry_2 ||
			entry1 == rSide.rela_entry_group[i].entry_2 && entry2 == rSide.rela_entry_group[i].entry_1 )
			return STA_SUCCESS;
	}
	return STA_FAILED;
}

void CStationDef::ResetStationSide()
{
	m_nSideCnt = 0;
	m_nCurSide = 0;

	m_side  = NULL;
}

void CStationDef::InitStationSideInfo()
{
	m_nSideCnt = 0;
	m_nCurSide = 0;
	m_nReceDeptSideCnt = 0;

	m_nCurEntry  = 0;
	m_nEntryCnt  = 0;

	m_nBackupCnt = 0;
	m_nCurBackupGroup = 0;
	memset(m_sName, 0, sizeof(m_sName));
	memset(m_sTleName, 0, sizeof(m_sTleName));
}
