
/*
 * TCC客户端(CTC-GPC/STPC, DMIS-CPC/STPC)接口定义
 * 
 * 两个重要接口: 
 *     ITccClientInfoProvider--客户端信息提供者
 *     ITccAdviseSink--客户端包装TCC事件(发送、取消限速命令)
 *
 * 两个重要结构:
 *     TCC_LIMITSPEED_CMD--限速命令内容
 *     TCC_STATUS--列控中心的状态
 *
 * Mar 28, 2006, wangshug
 * Copyright (c) 2006 CASCO LTD.
 * 
 */

/*
 * TCC提供缺省的限速命令号产生(见command-id.xml), 一般建议客户
 * 端重写命令号相关部分
 * 
 * 自律机返回的限速命令状态必须转换为TCC定义的相应状态
 * 
 */

#ifndef __TCC_H__
#define __TCC_H__

#pragma warning( disable:4786 )

#pragma once

#import <msxml4.dll> //raw_interfaces_only
#include <vector>

#pragma pack(push, 8)
namespace TCC {
#undef AFX_EXT_API  // CUIHU!!!
#define AFX_EXT_API 

	//Check if MSXML 4.0 is installed
	bool isMSXMLInstalled();

	class ITccClientInfoProvider;
	class ITccAdviseSink;

	typedef unsigned short stano_t;	//站号类型
	typedef unsigned short cmdid_t;	//列控命令号类型
	typedef unsigned short lineid_t;	//列控线路号类型

	typedef UUID sysid_t;

	//公里标类型
	struct  kmmark_t {
		DWORD val : 24;	//公里
		DWORD km_cl: 6;	//公里 长短链ChainLength
		DWORD m_cl : 2;	//米   长短链ChainLength

	public:
		kmmark_t(DWORD dw = 0);
		kmmark_t& operator=(const kmmark_t& rhs);
		DWORD GetDWORD();
		
		bool operator!=(const kmmark_t& rhs) const;
		bool operator==(const kmmark_t& rhs) const;
		bool operator<(const kmmark_t& rhs) const;
		bool operator<=(const kmmark_t& rhs) const;
		bool operator>(const kmmark_t& rhs) const;
		bool operator>=(const kmmark_t& rhs) const;

		CString GetKcl() const;
		CString GetMcl() const;

		CString toString() const;
		long DistanceTo(const kmmark_t& rhs) const;
	} ;


	//调度命令号类型
	typedef struct __DCMD_ID {
		char sz[17];

		__DCMD_ID(const char *lpsz = NULL){ memset(sz, 0, sizeof(sz)); if(lpsz) strncpy(sz, lpsz, sizeof(sz) -1); }
		__DCMD_ID(const __DCMD_ID& rhs){ operator=(rhs); }
		__DCMD_ID& operator=(const __DCMD_ID& rhs){ memcpy(sz, rhs.sz, sizeof(sz)); return *this;}
		operator const char* () const { return sz; }
	} dcmd_id_t;
	AFX_EXT_API inline bool operator==(const dcmd_id_t& lhs, const dcmd_id_t& rhs){ return strncmp(lhs.sz, rhs.sz, sizeof(lhs.sz)) == 0; }
	AFX_EXT_API inline bool operator!=(const dcmd_id_t& lhs, const dcmd_id_t& rhs){ return !operator==(lhs, rhs); }

	enum dir_t {
		DIR_UNKNOWN = 0,
		DIR_DOWN = 1,
		DIR_UP   = 2,
		DIR_DUAL = 3,
	};

	// sysid比较
	AFX_EXT_API bool operator==(sysid_t& lhs, sysid_t& rhs);
	AFX_EXT_API bool operator!=(sysid_t& lhs, sysid_t& rhs);

	// sysid生成
	AFX_EXT_API void SysidCreate(sysid_t *sysid);
	AFX_EXT_API void SysidCreateNil(sysid_t *sysid);

	// sysid与字符串相互转换
	AFX_EXT_API CString SysidToString(sysid_t);
	AFX_EXT_API sysid_t SysidFromString(CString str);

	// sysid与字节数组相互转换
	AFX_EXT_API int SysidToByteArray(const sysid_t& sysid, BYTE arr[], int num);
	AFX_EXT_API int SysidFromByteArray(sysid_t& sysid, const BYTE arr[], int num);
	// 命令状态
	enum {
		CONFIRM   = 0x0001,	// 确认状态
		EXECUTING = 0x0002,	// 执行中
		CANCELED  = 0x0003,	// 已取消
	};

	struct  TCC_COMMAND;
	struct  STOCK_COMMAND;
	struct  RAW_STOCK_COMMAND;
	
// 与自律机交互的限速命令格式
struct  TCC_COMMAND {
	WORD		nConsoleID;		//调度台 台号
	sysid_t		sys_id;			//内部标识
	stano_t		nStaID[2];		//开始、结束站
	lineid_t	nLineID;	//线路
	BOOL		bFullLimit;	//是否全线限速
	kmmark_t	nKmMark[2];		//始末公里标(以米为单位)
	BOOL		bCancelLimit;	//是否取消命令

	cmdid_t		nCmdID;		//命令号(列控保存)
	WORD		nSpeedMaxVal;	//限速值
	BOOL		bStartNow;		//是否立即开始
	BOOL		bEndless;		//是否永久有效
	time_t		nTime[2];		//开始、结束时间

	stano_t		nTccID;		//受令站(列控保存)
	lineid_t	nTccLineID;	//受令站线路号(列控保存)
	DWORD		nTccSenderID;	//受令站发令人

	TCC_COMMAND();
	bool operator==(const TCC_COMMAND& rhs);
	bool operator!=(const TCC_COMMAND& rhs);

	//组成一个RAW_STOCK_COMMAND
	void ComposeRawStcokCommand(RAW_STOCK_COMMAND &raw_cmd);
	CString toString(ITccClientInfoProvider *pInfoProvider);
};

// 服务器保存的确认命令格式
struct  STOCK_COMMAND {
	dcmd_id_t	dcmd_id;	//调度命令号

	//因为回找调度命令时需要下面的两项索引, 调度命令号循环使用的, 不能作为索引使用
	DWORD	  dcmd_send_entity;	//调度命令索引
	time_t	  dcmd_create_time;

	WORD		nConsoleID;	//调度台 台号
	sysid_t		sys_id;		//内部标识
	cmdid_t		nCmdID;		//命令号
	stano_t		nStaID[2];		//开始、结束站
	lineid_t	nLineID;		//线路
	CString		szTrainID;		//车次号
	WORD		nSpeedMaxVal;	//限速值

	BOOL		bFullLimit;	//是否全线限速
	kmmark_t	nKmMark[2];		//始末公里标(以米为单位)
	BOOL		bStartNow;		//是否立即开始
	BOOL		bEndless;		//是否永久有效
	time_t		nTime[2];		//开始、结束时间

	BOOL		bCancelLimit;	//是否取消命令

	DWORD		nCreatorID;	//草拟者
	DWORD		nCheckorID;	//中心校验者
	DWORD		nConfirmorID;	//确认分解者
	sysid_t		raw_sysid;		//原始命令标识
	time_t		nCreateTime;	//命令创建时间

	struct _TCC_STRUCT {
		stano_t		nID;		//受令站
		lineid_t	nTccLineID;//线路
		DWORD		nCheckorID;//车站校验者(无人站填确认分解者)
		WORD		nStatus;	//受令站命令状态
		DWORD		nSenderID;	//受令站命令发令人
		time_t		nTime;		//受令站命令生效时间
		_TCC_STRUCT()
		{
			nID = 0;
			nTccLineID = 0;
			nCheckorID = 0;
			nStatus = 0;
			nSenderID = 0;
			nTime = 0;
		}
	};
	std::vector<_TCC_STRUCT> tcc;//受令站列表

	STOCK_COMMAND();
	bool operator==(const STOCK_COMMAND& rhs);
	bool operator!=(const STOCK_COMMAND& rhs);

	WORD GetTccStatus(stano_t nTccID);
	DWORD GetTccSender(stano_t nTccID);

	BOOL GetTccCommand(TCC_COMMAND *pcmd, stano_t nTccID);
	// 分解为列控命令， 返回值为有效命令个数 
	INT  Decompose(TCC_COMMAND cmd[], INT num);
	//组成一个RAW_STOCK_COMMAND
	void ComposeRawStcokCommand(RAW_STOCK_COMMAND &raw_cmd);

	CString toString(ITccClientInfoProvider *pInfoProvider);
};

// 本地保存的原始限速命令
struct  RAW_STOCK_COMMAND
{
	dcmd_id_t	dcmd_id;	//调度命令号

	//BYTE     nCmdType;	//命令类型  1=STATION_LIMIT 2=ABSAPP_LIMIT 3=ABSAPP_FULL_LIMIT 4=KM_LIMIT
	//BYTE     nDirection;//限速方向  1下行 2上行 3双向

    WORD		nConsoleID;		//调度台 台号
	sysid_t		raw_sysid;	//原始命令标识
	cmdid_t		nCmdID;	//命令号
	stano_t		nStaID[2];	//开始、结束站
	lineid_t	nLineID;	//线路

	BOOL		bFullLimit;//是否全线限速
	kmmark_t	nKmMark[2];		//始末公里标(以米为单位)
	BOOL		bStartNow;	//是否立即开始
	BOOL		bEndless;	//是否永久有效
	time_t		nTime[2];	//开始、结束时间

	BOOL		bCancelLimit;	//是否取消命令

	DWORD		nCreatorID;		//草拟者
	DWORD		nCheckorID;		//中心校验者
	DWORD		nConfirmorID;	//确认分解者
	time_t		nCreateTime;	//命令创建时间

	struct _TCC_STRUCT {
		stano_t		nID;			//受令站
		lineid_t	nTccLineID;	//线路
		DWORD		nCheckorID;	//车站校验者(无人站填确认分解者)
		BYTE		bTccValidate;	//受令站需校验否
		_TCC_STRUCT()
		{
			nID = 0;
			nTccLineID = 0;
			nCheckorID = 0;
			bTccValidate = 1;
		}
	} ;
	std::vector<_TCC_STRUCT> tcc;//受令站列表

	struct _TRAINCMD {
		sysid_t sys_id;			//每趟车一个命令，一个系统标识
		CString szTrainID;		//车次号
		WORD	nSpeedMaxVal;	//限速值
		WORD	nSpeed;			//限速值
        
        //每趟车命令，各受令站状态
		struct _TCC_STRUCT {
            WORD	nStatus;	//受令站命令状态
            DWORD	nSenderID;	//受令站命令发令人
			time_t	nTime;		//受令站命令生效时间
		} ;
		std::vector<_TCC_STRUCT> tcc;

		_TRAINCMD();
	} train_cmd[5];

	BOOL bGenCancelCommand;
	RAW_STOCK_COMMAND();
	RAW_STOCK_COMMAND& operator=(const RAW_STOCK_COMMAND& rhs);
	bool operator==(const RAW_STOCK_COMMAND& rhs);
	bool operator!=(const RAW_STOCK_COMMAND& rhs);

    BOOL SetStatus(sysid_t& sysid, stano_t nTccID, WORD nStatus);
    BOOL SetSender(sysid_t& sysid, stano_t nTccID, DWORD nSenderID);
    BOOL SetTime(sysid_t& sysid, stano_t nTccID, DWORD nTime);
    BOOL SetTccCheckor(stano_t nTccID, DWORD nCheckor);
    
    // 分解为STOCK_COMMAND， 返回值为有效命令个数
    int Decompose(STOCK_COMMAND cmd[], int num);
	CString toString(ITccClientInfoProvider *pInfoProvider);
};

//enum {
//	TCC_UNKNOWN            = 0x0000,	// 未知
//
//	LIRC_RECEIVED          = 0x0001,	// 自律机收到
//	LIRC_SEND_FAIL         = 0x0002,	// 自律机发送失败(协议帧错误)
//	TCC_RECEIVED           = 0x0003,	// 列控收到(命令正确)
//	TCC_ERR_KM             = 0x0004,	// 公里标非法
//	TCC_ERR_SPEED          = 0x0005,	// 限速非法
//	TCC_ERR_REGION_LONG    = 0x0006,	// 区域太长
//	TCC_ERR_REGION_EXIST   = 0x0007,	// 区域已存在
//	TCC_ERR_REGION_INVALID = 0x0008,	// 区域非法
//	TCC_ERR_DUP_CMDNO      = 0x0009,	// 命令号重复
//	TCC_ERR_TIME_FAIL      = 0x000a,	// 时间校验失败
//
//	TCC_ACCEPT             = 0x000b,    // 列控接受（正确）
//	TCC_WAITFOR_EXEC       = 0x000c,	// 排队，等待执行
//	TCC_EXECUTING          = 0x000d,	// 正在执行
//	TCC_ZOMBIE             = 0x000e,	// 执行结束(结束时间到)
//	TCC_CANCELED           = 0x000f,	// 已取消(人工, 正常消亡)
//	
//	LOC_WAITFOR_CANCEL     = 0x0010,	// 等待取消
//	LOC_EDIT               = 0x0011,	// 编辑命令
//	LOC_SENDED             = 0x0012,	// 本地命令已下发
//	LOC_SEND_FAIL          = 0x0013,	// 本地命令下发错
//
//	TCC_CONFIRM_OK         = 0x0014,	// 被确认
//	TCC_CONFIRM_CANCEL     = 0x0015,	// 被取消
//	TCC_CONFIRM_TIMEOUT    = 0x0016,	// 超时，未确认
//};

// Interface
class  ITccInterface
{
public:
	ITccInterface();
	virtual ~ITccInterface();
	void AddRef();
	void Release();
private:
	LONG nRefCount;
};

// Out Interface
// 客户端必须重新实现该接口
class  ITccAdviseSink : public ITccInterface
{
public:
	// 向一个列控中心发送限速命令，发送结果由函数值返回(0:成功，1:失败)
	virtual int OnSendTccCommand(TCC_COMMAND& cmd, HWND hTccDlg) = 0;
	virtual int OnSendInitCommand(stano_t nTccID, lineid_t nTccLineID, dir_t dir, HWND hTccDlg) = 0;
	// 申请限速命令列表
	virtual int OnRequestTccCmdList(stano_t nTccID, HWND hTccDlg) = 0;

	// 在实时列表上取消限速
	virtual int OnCancelTccCommand(TCC_COMMAND& cmd, STOCK_COMMAND *pStockCmd, HWND hTccDlg) = 0;
	// 在确认列表上取消限速
	virtual int OnCancelStockCommand(STOCK_COMMAND& cmd, HWND hTccDlg) = 0;

	// 发送确认命令
	virtual int OnSendStockCommand(STOCK_COMMAND& cmd, HWND hTddDlg) = 0;
	// 删除某受令站确认命令
	virtual int OnDelStockCommand(STOCK_COMMAND& cmd, stano_t nTccID, HWND hTccDlg) = 0;

	virtual INT OnRequestStockCommandList(stano_t nTccID, HWND hTccDlg, int flag) = 0;
	virtual int OnRequestStockCommandList(WORD nConsoleID, stano_t nTccID, const dcmd_id_t& dcmd_id, time_t tmScopeBegin, time_t tmScopeEnd, HWND hTccDlg, int flag) = 0;

	virtual int OnTccCommandFileChanged(
					LPCSTR szFilename, 
					DWORD dwOper) = 0;

	// 预留
	virtual int OnTccCommandStatusChanged(
					TCC_COMMAND &cmd,
					DWORD status,
					DWORD dwParam) = 0;

	enum {
		SELF_CONFIRM_COMMAND_ID,
		OTHER_CONFIRM_COMMAND_ID,
	};
	virtual BOOL ConfirmCommandID(int nConsoleID, cmdid_t nCommandID, cmdid_t nMinCommandID, cmdid_t nMaxCommandID, int& who_confirm_cmdid, DWORD dwCookie, HWND hTccDlg);
};

// 客户端必须重新实现该接口以向TCC提供本地信息
class  ITccClientInfoProvider : public ITccInterface
{
public:
	// TCC数据目录(如../data/DDML/tcc), 用于存放Tcc原始命令
	virtual CString GetTccDataDirectory() = 0;
	// TCC配置目录(如../data/tcc), 用于存放Tcc的配置数据
	virtual CString GetTccConfigDirectory() = 0;

	// 得到车站列表，返回值为实际取得的列表数目
	virtual void GetStationList(std::vector<stano_t>& staList) = 0;
	// 根据站号得到站名
	virtual CString GetStationName(stano_t nStaID) = 0;

	// 得到发令单位和发令人
	virtual CString GetOrgSender() = 0;
	virtual CString GetPersonSender() = 0;

	virtual WORD  GetConsoleID() = 0;

	// Absline编号对每一个与其相连的站来说都是唯一的

	// 得到相邻站的Absline列表
	//virtual void GetAbslineList(
	//				stano_t nStaID, 
	//				stano_t nStaID2, 
	//				std::vector<lineid_t>& nAbslineID) = 0;
	// 根据Absline编号得到其中文名称
	//virtual CString GetAbslineName(
	//					stano_t nStaID, 
	//					stano_t nStaID2, 
	//					lineid_t nAbslineID) = 0;
	// 根据站号和Absline编号得到其中文名称
	//virtual CString GetAbslineName(
	//					stano_t nStaID, 
	//					lineid_t nAbslineID) = 0;

	// 判断nStaID与nStaID2 是否相邻站
	virtual BOOL IsLinkedStation(
						stano_t nStaID, 
						stano_t nStaID2) = 0;

	// 站nStaID2相对于站nStaID的区间方向性(上行方向, 下行方向)
	virtual dir_t GetStationDirection(
						stano_t nStaID, 
						stano_t nStaID2) = 0;
	// 得到站nStaID的相邻站列表
	virtual void GetLinkedStationList(
						stano_t nStaID, 
						std::vector<stano_t>& staList) = 0;
	// 根据站号, Tcc线路号和方向(即区间)得到相邻站
	virtual stano_t GetLinkedStation(
						stano_t nStaID, 
						lineid_t nTccLineID,
						dir_t   dir) = 0;


		virtual void GetTccBlockLineList(stano_t nStaID, std::vector<lineid_t>& lines) = 0;
		virtual CString GetTccBlockLineName(stano_t nStaID, lineid_t nTccLineID) = 0;
		virtual BOOL IsTccBlockLine(stano_t nStaID, lineid_t nTccLineID) = 0;

	virtual void GetTccLineList(stano_t nStaID, std::vector<lineid_t>& lines) = 0;
	// 取得站nStaID的TccLine列表中与站nStaID2相连的那些TccLine
	virtual void GetTccLineList(
					stano_t nStaID,
					stano_t nStaID2,
					std::vector<lineid_t>& lines) = 0;
	// 取得站nStaID中与站nStaID2的线路nTccLineID2相匹配的TccLine
	virtual lineid_t GetTccLine(
					stano_t nStaID,
					stano_t nStaID2,
					lineid_t nTccLineID2) = 0;
	// 取得站nStaID的nTccLineID的名称
	virtual CString GetTccLineName(stano_t nStaID, lineid_t nTccLineID) = 0;
	// 取得目标站nStaID的TCC线路nTccLineID所跨区间
	// DIR_UP 上行区间
	// DIR_DOWN 下行区间
	// DIR_DUAL 上下行区间
	// DIR_UNKNOWN 线路号不存在
	virtual dir_t GetTccLineAbsNum(stano_t nStaID, lineid_t nTccLineID) = 0;
	// 取得TccLine的方向值(上下行或双向)
	virtual dir_t GetTccLineDirection(stano_t nStaID, lineid_t nTccLineID) = 0;

	// 取得内部线路号
	virtual int GetLineID(TCC::stano_t nTccID, TCC::lineid_t nTccLineID) = 0;
	// 根据公里标取得相关站
	virtual std::vector<TCC::stano_t> GetTccList(int builtin_id, kmmark_t nKmBegin, kmmark_t nKmEnd) = 0;

	// 校验站号的合法性
	virtual BOOL ValidateStationID(stano_t nStaID) = 0;

	virtual BOOL IsStationListChanged() = 0;

	// 得到可用命令号数值范围
	virtual BOOL  IsValidCommandID(cmdid_t nCmdID);
	virtual cmdid_t GetInvalidCommandID();
	virtual cmdid_t GetMinCommandID();
	virtual cmdid_t GetMaxCommandID();
	virtual cmdid_t GetCommandID();
	virtual void    NextCommandID();
	virtual cmdid_t SaveCommandID(cmdid_t nCommandID);

	virtual bool IsTCCReadOnly()=0;
	virtual bool IsTCCExecute()=0;
	virtual bool IsCTCGPC()=0;
	
};


enum {
	STATIONLIST_CHANGED,
	LOC_COMMAND_STORE_CHANGED,
};
// 列控命令管理界面启动
AFX_EXT_API HWND TccDialogOpen();
AFX_EXT_API void TccDialogHide();
AFX_EXT_API void TccDialogClose();
AFX_EXT_API void TccDialogUpdate(DWORD dwReason);
AFX_EXT_API HWND TccDialog(
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink, 
		CWnd *pParent, 
		LPCSTR szCaption = NULL);

// 注册 注销列控消息
void RegisterTCCMsgProcHandleWnd();
void UnRegisterTCCMsgProcHandleWnd();

/*
  列控状态
 */
struct  TCC_STATUS {
	BYTE comm_active;	// 列控通信状态	(0=断;1=单机;2=双机)
	BYTE stcc_master[2];	// 列控主备状态
	BYTE ilock_channel[2];	// 联锁通道状态
	BYTE ctc_channel[2];	// CTC通道状态

	BYTE leu_channel[4];	// LEU 通道状态
	BYTE leu_ack[16];	// LEU应答器状态, 一个leu有4个应答器
	BYTE init_section_num;	// 初始化区间个数(最大为8)
	lineid_t init_line_no[8];	// 区间所在线路号
	dir_t    init_direction[8]; // 对应的线路方向
	TCC_STATUS() { memset(this, 0, sizeof(TCC_STATUS)); }
};
/*
  显示列控报告的状态
 */
//AFX_EXT_API 
//BOOL Receive_TccReport(DWORD nTccID, TCC_STATUS& status);

/*
  将列控命令显示在实时命令列表中
  Input:
     cmd 列控实时命令
	 nStatus 预留
  Output:
     TRUE 成功显示
	 FALSE 拒绝显示(可能站号不一致)
 */
//AFX_EXT_API
//BOOL Receive_TccCommand(TCC_COMMAND& cmd, DWORD nStatus);

/*
  显示下发列控命令时的操作状态
  Input:
     cmd 下达的列控命令
	 nStatus 自律机定义的列控状态(目前预留)
	 strComment 操作结果描述
 */
//AFX_EXT_API
//void Receive_TccCommandResult(
//	TCC_COMMAND& cmd, 
//	DWORD nStatus, 
//	LPCSTR strComment);

/*
  显示列控线路限速初始化操作的结果
  Input:
     nTccID 站号
	 nTccLineID 线路号
	 dir 线路区间(上行方向, 下行方向)
	 nStatus 预留
	 strComment 初始化结果描述
 */
//AFX_EXT_API
//void Receive_TccLineInitialCommandResult(
//	stano_t nTccID, 
//	lineid_t nTccLineID, 
//	dir_t dir, 
//	DWORD nStatus, 
//	LPCSTR strComment);


// 服务器返回的确认命令列表
//AFX_EXT_API BOOL Receive_StockCommand(STOCK_COMMAND& cmd);
//AFX_EXT_API BOOL Receive_HistoryCommand(STOCK_COMMAND& cmd);

/*
  原始限速命令输入
  Input:
     pcmd 提供限速命令的返回空间
	 pInfoProvider 提供客户端参数
	 pParent 父窗口
  Output:
	 TRUE 成功修正
	 FALSE 放弃修正
     pcmd 返回输入的限速命令
 */
AFX_EXT_API
BOOL RawStockCommandInput(
	RAW_STOCK_COMMAND *pcmd, 
	ITccClientInfoProvider *pInfoProvider, 
	ITccAdviseSink *pAdviseSink,
	CWnd *pParent=NULL);

/*
  原始限速命令修正
  Input:
     pcmd 待修正的限速命令
	 pInfoProvider 提供客户端参数
	 pParent 父窗口
  Output:
	 TRUE 成功修正
	 FALSE 放弃修正
     pcmd 修正后的限速命令
 */
AFX_EXT_API
BOOL RawStockCommandReEdit(
	RAW_STOCK_COMMAND *pcmd, 
	ITccClientInfoProvider *pInfoProvider, 
	ITccAdviseSink *pAdviseSink,
	CWnd *pParent=NULL);

/*
  原始限速命令校验
  Input:
     pcmd 待校验的限速命令
	 pInfoProvider 提供客户端参数
	 pParent 父窗口
  Output:
	 TRUE 校验通过
	 FALSE 校验失败
 */
AFX_EXT_API
BOOL RawStockCommandValid(
	RAW_STOCK_COMMAND *pcmd, 
	ITccClientInfoProvider *pInfoProvider, 
	ITccAdviseSink *pAdviseSink,
	CWnd *pParent=NULL);


/*
  原始限速命令的文件操作
  一个文件内只读取一条原始命令,所以保存文件时必须注意文件名
  与命令号和SYSID的对应
 */ 
AFX_EXT_API
BOOL LoadCommand(RAW_STOCK_COMMAND& cmd, LPCSTR szFilename);

AFX_EXT_API
BOOL SaveCommand(RAW_STOCK_COMMAND& cmd, LPCSTR szFilename);


// 返回命令号和SYSID组成的文件名称
AFX_EXT_API
CString GetCommandExtraFileName(sysid_t& sysid, cmdid_t& nCmdID);

AFX_EXT_API
CString GetCommandExtFileName();

//增加长链后需解析公里标
AFX_EXT_API 
CString ComposeKMStr(DWORD km);
//得到KM的标志索引
AFX_EXT_API
BYTE GetLongKmFlag(DWORD km);
//得到M的标志索引
AFX_EXT_API
BYTE GetLongMFlag(DWORD km);

//
AFX_EXT_API
BOOL ReceiveCommandIDConfirmation(bool bValid, int nConsoleID, cmdid_t nCmdID, DWORD dwCookie);


};

#pragma pack(pop)

#endif
