//
#ifndef TG_BLOCK_H
#define TG_BLOCK_H

#include "tg_structdef.h"

typedef DWORD MARK_KEY;
//使用slow_time字段低二位表示图表注解中外围圆圈的位置
//00表示没有圆圈，01表示蓝圈，10表示红圈
#define SetSLWRoundNone(slow_time)   ((slow_time) = (0xFFF8&(slow_time))|0x0000)
#define SetRectBlue(slow_time)       ((slow_time) = (0xFFF8&(slow_time))|0x0001)
#define SetRectRed(slow_time)        ((slow_time) = (0xFFF8&(slow_time))|0x0002)
#define SetCircleRed(slow_time)      ((slow_time) = (0xFFF8&(slow_time))|0x0003)
#define SetCircleBlue(slow_time)     ((slow_time) = (0xFFF8&(slow_time))|0x0004)

#define IsSLWRoundNone(slow_time)   ((0x0007&(slow_time)) == 0x0000)
#define IsRectBlue(slow_time)		((0x0007&(slow_time)) == 0x0001)
#define IsRectRed(slow_time)		((0x0007&(slow_time)) == 0x0002)
#define IsCircleRed(slow_time)		((0x0007&(slow_time)) == 0x0003)
#define IsCircleBlue(slow_time)		((0x0007&(slow_time)) == 0x0004)

//bit 4 用来表示字体颜色
//0表示 红色  1表示 蓝色
#define SetWordRed(slow_time)       (slow_time &= ~0x0010)
#define SetWordBlue(slow_time)      (slow_time |= 0x0010)
#define IsWordBlue(slow_time)		(slow_time & 0x0010)

//文字位置用到的宏
//使用line的高位字节中的低二位来设置线条位置
//00上方  01默认是设在中间  10下方
//设置站内封锁，缓行，天窗修的文字位置
//文字位置为上方、下方，左，中，右.
#define SetLineLinePosTop(line)    ((line) = (0xFCFF&(line))|0x0000)
#define SetLineLinePosMiddle(line) ((line) = (0xFCFF&(line))|0x0100)
#define SetLineLinePosBottom(line) ((line) = (0xFCFF&(line))|0x0200)

#define IsLineLinePosTop(line)	    ((0x0300&(line)) == 0x0000)
#define IsLineLinePosMiddle(line)	((0x0300&(line)) == 0x0100)
#define IsLineLinePosBottom(line)	((0x0300&(line)) == 0x0200)

//文字位置使用高位字节的低第三位
//0表示上方，缺省方式,1表示下方
#define SetLineTextPosTop(line)		((line) = (0xE3FF&(line))|0x0000)
#define SetLineTextPosBottom(line)  ((line) = (0xE3FF&(line))|0x0400)

#define SetLineTextPosLeft(line)    ((line) = (0xE3FF&(line))|0x1000)
#define SetLineTextPosMiddle(line)  ((line) = (0xE3FF&(line))|0x1400)
#define SetLineTextPosRight(line)   ((line) = (0xE3FF&(line))|0x1800)

#define IsLineTextPosTop(line)		((0x1C00&(line)) == 0x0000)
#define IsLineTextPosBottom(line)	((0x1C00&(line)) == 0x0400)
#define IsLineTextPosLeft(line)		((0x1C00&(line)) == 0x1000)
#define IsLineTextPosMiddle(line)	((0x1C00&(line)) == 0x1400)
#define IsLineTextPosRight(line)	((0x1C00&(line)) == 0x1800)


//是否是继续画线的标志使用高位字节的低第四位
//0表示实线，缺省方式,1表示虚线
#define SetLineSolid(line) ((line) = (0xF7FF&(line))|0x0000)
#define SetLineDash(line) ((line) = (0xF7FF&(line))|0x0800)

#define IsLineSolide(line)	((0x0800&(line)) == 0x0000)
#define IsLineDash(line)	((0x0800&(line)) == 0x0800)

//使用低字节的低二位
//00上行，01下行，10上下行缺省，11无方向，表示选择了一条不能说明方向的线
#define SetLineDirectionUp(line) ((line) = (0xFFFC&(line))|0x0000)
#define SetLineDirectionDown(line) ((line) = (0xFFFC&(line))|0x0001)
#define SetLineDirectionDual(line) ((line) = (0xFFFC&(line))|0x0002)
#define SetLineDirectionNone(line) ((line) = (0xFFFC&(line))|0x0003)

#define IsLineDirectionUp(line) ((0x0003&(line)) == 0x0000)
#define IsLineDirectionDown(line) ((0x0003&(line)) == 0x0001)
#define IsLineDirectionDual(line) ((0x0003&(line)) == 0x0002)
#define IsLineDirectionNone(line) ((0x0003&(line)) == 0x0003)

//使用低字节的低三四位
//00区间封锁，01轻型车辆，其他保留
#define SetLineFlagInhibit(line) ((line) = (0xFFF3&(line))|0x0000)
#define SetLineFlagLightTrain(line) ((line) = (0xFFF3&(line))|0x0004)

#define IsLineFlagInhibit(line) ((0x000C&(line)) == 0x0000)
#define IsLineFlagLightTrain(line) ((0x000C&(line)) == 0x0004)


#define SetUpdateByOtherGPC(block) ( ((block)->dwFlag) |= 0x1)
#define SetDeleteByOtherGPC(block) ( ((block)->dwFlag) |= 0x2)
#define IsUpdateByOtherGPC(block)  ( (((block)->dwFlag)&0x1) == 0x1)
#define IstDeleteByOtherGPC(block) ( (((block)->dwFlag)&0x2) == 0x2)

#define SetBlockPlan(adjust_status) ((adjust_status) = (0xFFF8&(adjust_status))|0x0000)
#define SetBlockActive(adjust_status) ((adjust_status) = (0xFFF8&(adjust_status))|0x0001)
#define SetBlockComplete(adjust_status) ((adjust_status) = (0xFFF8&(adjust_status))|0x0002)

#define IsBlockPlan(adjust_status)  ((0x0007&(adjust_status)) == 0x0000)
#define IsBlockActive(adjust_status) ((0x0007&(adjust_status)) == 0x0001)
#define IsBlockComplete(adjust_status) ((0x0007&(adjust_status)) == 0x0002)


#define SetSlowSpeed(speed) ((speed)/5)
#define GetSlowSpeed(speed) ((speed)*5)

#define MAX_SCHD_BLOCK_ELE_COUNT 512
#define MAX_SCHD_BLOCK_TEXT_LEN  256

class CTg_BLOCK
{
public:
	DWORD     combstation;
	MARK_KEY  mark_index;
	long	  start_time;
	long      end_time;
	BYTE      type;           
	BYTE      flag;  
	USHORT    line;
	USHORT    station;
	USHORT    index;
	USHORT    position;       
	USHORT    count;
	USHORT	  ele_number[MAX_SCHD_BLOCK_ELE_COUNT];
	char	  text[MAX_SCHD_BLOCK_TEXT_LEN];

	USHORT    s_meter;
	USHORT    e_meter;
	USHORT    e_kilo;
	USHORT    slow_time;

	// 1. 标记来源不是gpc,比如TD时,用来根据它们绘画
	// 2. 告诉用户当前标记所在的上下选中车站
	USHORT    station1;
	USHORT    station2; 
	BYTE	  direction; // 1: 上行 2: 下行 3: 上下行
	
	// add your code here
	bool	  bFlag; 
    CRect     myPosition;

	// 作为标志使用
	DWORD     dwFlag;  // bit0: 该标记已经被其它GPC更新 1:是 0:否
					   // bit1: 该标记已经被其它GPC删除 1:是 0:否

	// 用于表示施工符号的状态信息
	DWORD	 adjust_status; // // bit 1-0:  0 计划、1 执行、2 实际
		                   
public:
	CTg_BLOCK();
	CTg_BLOCK(const CTg_BLOCK& rhs);
	CTg_BLOCK& operator=(const CTg_BLOCK& rhs);
	~CTg_BLOCK();
	void Clear();
	
public:
	CString toXML(DWORD nResult=0);

	CString toString();
private:
	void Copy(const CTg_BLOCK& rhs);

public:
	bool IfInTimeScope(TIME stime, TIME etime) const;
	void RestoreDrawFlag();
	void SetColor(COLORREF  newColor);
	void SetDrawText(CString &text);
	CString GetKiloMDesp();
	void SetReasonTextFromTDMS();
	bool GetKiloMValue(ULONG& beginKilo, ULONG& endKilo);
public:
	BOOL bMoveCenter,bMoveRight,bMoveLeft,bMoveTop,bMoveBottom;
	BOOL m_bFocus;
	COLORREF mColor;
	CRect cRect,lRect,rRect,tRect,bRect,m_drawRect;

public:
	void OffsetPosition(int x);
	// 自动滚动模式下,记录上次列车绘制时的开始时间
	int m_nUpdateTime;
};

#endif