
/*
* TCC 实现内部接口定义
*
*/
#ifndef __TCC_C3_BASE_H__
#define __TCC_C3_BASE_H__

#include "Tcc.h"
#include <map>

#include <list>
using namespace std;

#pragma once

namespace TCC_C3 {

	BOOL TccInitialize(ITccClientInfoProvider *pInfoProvider);

	BOOL LoadTccEnv(LPCSTR szFilename);
	BOOL LoadCommandIDInfo(LPCSTR szFilename);
	BOOL SaveCommandIDInfo(LPCSTR szFilename);
	BOOL LoadTccConfig(LPCSTR szFilename);

	//BOOL CanEditLimitSpeedCommand(DWORD nStatus);
	//BOOL CanReSendLimitSpeedCommand(DWORD nStatus);
	//BOOL CanDeleteLimitSpeedCommand(DWORD nStatus);

	BOOL GetTccStatus(DWORD nTccID, TCC_STATUS *pStatus);
	void SetTccStatus(DWORD nTccID, TCC_STATUS& status);
	void ResetTccStatus();

	HWND GetTccDialogWndHandle();

	BOOL LoadRawStockCommand(
		MSXML2::IXMLDOMNodePtr pCmdNode, 
		RAW_STOCK_COMMAND &cmd);
	BOOL SaveRawStockCommand(
		MSXML2::IXMLDOMNodePtr pCmdNode, 
		RAW_STOCK_COMMAND &cmd);
	BOOL DeletecCommandNode(MSXML2::IXMLDOMDocumentPtr pDOMDoc, sysid_t& sysid);
	BOOL AddCommandNode(
		MSXML2::IXMLDOMDocumentPtr pDOMDoc, 
		RAW_STOCK_COMMAND& cmd, 
		BOOL bInsert);

	BOOL DeleteCommand(RAW_STOCK_COMMAND& cmd, LPCSTR szFilename);
	void DumpFile(LPCSTR szFilename);
	void EraseZeroFileInDir(LPCSTR szDirectory);
	BOOL IsOldCommandFile(LPCSTR szFilename);

	CString GetDifferenceReason(RAW_STOCK_COMMAND& lhs, RAW_STOCK_COMMAND& rhs, ITccClientInfoProvider *pInfoProvider);

	//一条线路
	struct LINE_STRUCT 
	{
		int builtin_id;	//线路内编号
		CString szName;	//线路名称
		dir_t nOrientation;	//线路方向
		bool bKmmarkAscend; //线路正向公里标是升序的
		vector<int> speeds, speeds_R;

		//公里标系
		struct _KM_CODE {
			CString szPrex;	//前缀
			char    nCode;	//代码
			bool	bInc;	//升序(公里标越来越大)
			DWORD	nMIN,	nMAX;	//数值范围
			_KM_CODE()
				: nCode(0)
				, bInc(true)
				, nMIN(0), nMAX(-1)
			{
			}
			_KM_CODE(const _KM_CODE& rhs) 
			{
				szPrex = rhs.szPrex;
				nCode = rhs.nCode;

				bInc = rhs.bInc;
				nMIN = rhs.nMIN;
				nMAX = rhs.nMAX;
			}
			_KM_CODE& operator=(const _KM_CODE& rhs)
			{
				if(this == &rhs)
					return (*this);

				szPrex = rhs.szPrex;
				nCode = rhs.nCode;

				bInc = rhs.bInc;
				nMIN = rhs.nMIN;
				nMAX = rhs.nMAX;

				return (*this);
			}
		};
		vector<_KM_CODE> km_codes;
		CString szDefKmsysPrex;

		kmmark_t nKmBegin, nKmEnd;		//整条线路可用公里标
		kmmark_t nKmBegin_R, nKmEnd_R;	//            反向公里标
		stano_t nLineFirstTccID;	//线路的第一个站
		stano_t nLineLastTccID;	//线路的最后一个站

		//站、中继站归档公里标列表
		struct _STA_GD 
		{
			CString szName;		//站名称，中继站无站号
			struct _SEC_GD 
			{
				kmmark_t nKmBegin, nKmEnd;
				long nLength;
				CString szName;	//轨道电路区段名称
				_SEC_GD() : nLength(0) 
				{
				}
				_SEC_GD(const _SEC_GD& rhs) 
				{
					nKmBegin = rhs.nKmBegin;
					nKmEnd = rhs.nKmEnd;

					nLength = rhs.nLength;
					szName = rhs.szName;
				}
				_SEC_GD& operator=(const _SEC_GD& rhs)
				{
					if(this == &rhs)
						return (*this);

					nKmBegin = rhs.nKmBegin;
					nKmEnd = rhs.nKmEnd;

					nLength = rhs.nLength;
					szName = rhs.szName;

					return (*this);
				}
			};
			vector<_SEC_GD> secs, secs_r;

			_STA_GD()
			{
			}
			_STA_GD(const _STA_GD& rhs)
			{
				szName = rhs.szName;

				for(int i=0;i<rhs.secs.size();i++)
					secs.push_back(rhs.secs[i]);

				for(int i=0;i<rhs.secs_r.size();i++)
					secs_r.push_back(rhs.secs_r[i]);
			}
			_STA_GD& operator=(const _STA_GD& rhs)
			{
				if(this == &rhs)
					return (*this);

				secs.clear();
				secs_r.clear();

				szName = rhs.szName;
				for(int i=0;i<rhs.secs.size();i++)
					secs.push_back(rhs.secs[i]);

				for(int i=0;i<rhs.secs_r.size();i++)
					secs_r.push_back(rhs.secs_r[i]);

				return (*this);
			}
		};
		vector<_STA_GD> gd_stas;

		//站间公里标列表
		struct _FULLSEC {
			//int builtin_id;
			kmmark_t nKmBegin, nKmEnd;
			_FULLSEC() { }
		};
		vector<_FULLSEC> fullsecs;

		LINE_STRUCT()
		{
			builtin_id = 0;
			nOrientation = DIR_UNKNOWN;
			bKmmarkAscend = true;
			nLineFirstTccID = nLineLastTccID = 0;
		}

		LINE_STRUCT(const LINE_STRUCT& rhs)
		{
			builtin_id = rhs.builtin_id;	//线路内编号
			szName = rhs.szName;	//线路名称
			nOrientation = rhs.nOrientation;	//线路方向
			bKmmarkAscend  = rhs.bKmmarkAscend; //线路正向公里标是升序的

			for(int i=0;i<rhs.speeds.size();i++)
				speeds.push_back(rhs.speeds[i]);

			for(int i=0;i<rhs.speeds_R.size();i++)
				speeds_R.push_back(rhs.speeds_R[i]);
				
			for(int i=0;i<rhs.km_codes.size();i++)
				km_codes.push_back(rhs.km_codes[i]);
			
			szDefKmsysPrex = rhs.szDefKmsysPrex;

			nKmBegin = rhs.nKmBegin;
			nKmEnd = rhs.nKmEnd;

			nKmBegin_R = rhs.nKmBegin_R;
			nKmEnd_R = rhs.nKmEnd_R;

			nLineFirstTccID = rhs.nLineFirstTccID;
			nLineLastTccID = rhs.nLineLastTccID;

			for(int i=0;i<rhs.gd_stas.size();i++)
				gd_stas.push_back(rhs.gd_stas[i]);
				
			for(int i=0;i<rhs.fullsecs.size();i++)
				fullsecs.push_back(rhs.fullsecs[i]);
		}

		LINE_STRUCT& operator=(const LINE_STRUCT& rhs)
		{
			if(this == &rhs)
				return (*this);

			speeds.clear();
			speeds_R.clear();
			km_codes.clear();

			gd_stas.clear();
			fullsecs.clear();

			builtin_id = rhs.builtin_id;	//线路内编号
			szName = rhs.szName;	//线路名称
			nOrientation = rhs.nOrientation;	//线路方向
			bKmmarkAscend  = rhs.bKmmarkAscend; //线路正向公里标是升序的

			for(int i=0;i<rhs.speeds.size();i++)
				speeds.push_back(rhs.speeds[i]);

			for(int i=0;i<rhs.speeds_R.size();i++)
				speeds_R.push_back(rhs.speeds_R[i]);
				
			for(int i=0;i<rhs.km_codes.size();i++)
				km_codes.push_back(rhs.km_codes[i]);
			
			szDefKmsysPrex = rhs.szDefKmsysPrex;

			nKmBegin = rhs.nKmBegin;
			nKmEnd = rhs.nKmEnd;

			nKmBegin_R = rhs.nKmBegin_R;
			nKmEnd_R = rhs.nKmEnd_R;

			nLineFirstTccID = rhs.nLineFirstTccID;
			nLineLastTccID = rhs.nLineLastTccID;

			for(int i=0;i<rhs.gd_stas.size();i++)
				gd_stas.push_back(rhs.gd_stas[i]);
				
			for(int i=0;i<rhs.fullsecs.size();i++)
				fullsecs.push_back(rhs.fullsecs[i]);
				
			return (*this);
		}
	};

	//一个列控站
	struct TCC_STRUCT 
	{
		stano_t nTccID;	//列控站号
		stano_t nParentStaID;
		CString szName;

		//正线或支线列表
		struct _LINE 
		{
			int builtin_id;
			lineid_t tccline_id;	//列控线路号
			stano_t nPrevTccID, nNextTccID;	//前后站，公里标大的是后站
			kmmark_t nKmBegin, nKmEnd;
			kmmark_t nKmBegin_R, nKmEnd_R;
			_LINE() { memset(this, 0, sizeof(*this)); }
		};
		map<int, _LINE> line_map;

		//侧线列表
		struct _BLOCK 
		{
			lineid_t tccline_id;
			CString szName;
			vector<int> speeds;
			_BLOCK() : tccline_id(0) { }

			_BLOCK(const _BLOCK& rhs)
			{
				tccline_id = rhs.tccline_id;
				szName = rhs.szName;

				for(int i=0; i<rhs.speeds.size(); i++)
				{
					speeds.push_back(rhs.speeds[i]);
				}
			}

			_BLOCK& operator=(const _BLOCK& rhs)
			{
				if(this == &rhs)
					return *this;
				
				speeds.clear();

				tccline_id = rhs.tccline_id;
				szName = rhs.szName;

				for(int i=0; i<rhs.speeds.size(); i++)
				{
					speeds.push_back(rhs.speeds[i]);
				}

				return *this;
			}
		};
		vector<_BLOCK> blocks;

		
		
		TCC_STRUCT()
		{
			nTccID = 0;
			nParentStaID = 0;
		}
		TCC_STRUCT(const TCC_STRUCT& rhs)
		{
			nTccID = rhs.nTccID;	//列控站号
			nParentStaID = rhs.nParentStaID;
			szName = rhs.szName;	

			map<int, _LINE>::const_iterator it = rhs.line_map.begin();
			for(; it != rhs.line_map.end(); it++)
			{
				line_map.insert( make_pair(it->first, it->second)); 
			}

			for(int i=0; i<rhs.blocks.size(); i++)
			{
				blocks.push_back(rhs.blocks[i]);
			}
		}
		TCC_STRUCT& operator=(const TCC_STRUCT& rhs)
		{
			if(this == &rhs)
				return *this;
			
			line_map.clear();
			blocks.clear();

			nTccID = rhs.nTccID;	//列控站号
			nParentStaID = rhs.nParentStaID;
			szName = rhs.szName;	

			map<int, _LINE>::const_iterator it = rhs.line_map.begin();
			for(; it != rhs.line_map.end(); it++)
			{
				line_map.insert( make_pair(it->first, it->second)); 
			}

			for(int i=0; i<rhs.blocks.size(); i++)
			{
				blocks.push_back(rhs.blocks[i]);
			}

			return *this;
		}
	};

	struct KCL_MAP {
		LPCSTR szKcl;
		char   n;
	};
	KCL_MAP* GetKclMap();
	extern char StringtoKcl(CString str);
	extern CString KcltoString(char kcl);

	AFX_EXT_API bool parseKmMark(const LINE_STRUCT& line, CString szKmMark, kmmark_t *pKmMark);
	AFX_EXT_API bool parseKmMark(
		CString szKmMark, 
		CString& szPrex, CString& szKm, CString& szKcl,
		CString& szM, CString& szMcl);
	AFX_EXT_API const LINE_STRUCT* GetLineStruct(int builtin_id);

	// 服务器确认命令号
	#define UM_SERVER_CONFIRM_COMMAND_ID	(WM_USER + 101)
	struct SERVER_CONFIRM_COMMAND_ID_STRUCT
	{
		bool bValid;

		int nConsoleID;
		cmdid_t nCmdID;
		DWORD dwCookie;
	};

	void RegisterConfirmCommandIDReceiver(HWND hWnd);
	void UnRegisterConfirmCommandIDReceiver(HWND hWnd);
};

#endif
