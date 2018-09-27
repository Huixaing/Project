#ifndef SOCK_STRU_INCLUDE
#define SOCK_STRU_INCLUDE
#include "basictypes.h"

 typedef struct{
  IWORD    msg_attrib;      //  消息属性
  IDWORD   dest_id;         // 目的地址
  IDWORD   source_id;       // 始发地址
  struct{
	  IDWORD    tm;         // time_t
      IDWORD    subtm;      // microsec
  }generate_time;           // 消息始发时刻
  union{
	  struct{
		IWORD   gen_time;   // 50 ms 精度的时间
		IWORD   check_sum;  // == check_sum
	  }label_data;
	  IDWORD  label;
  };
  IWORD   data_len;
  IWORD   check_sum;
  union{
    IBYTE   seq; 
	IDWORD  spare;
  };
 }L3_MSG_HEADER;

#define  MSG_ATTRIB_NEED_ACK       0x01
#define  MSG_ATTRIB_NO_HEADER      0x02
#define  MSG_ATTRIB_ACK            0x04
#define  MSG_ATTRIB_CHECK_DUP      0x08
#define  MSG_ATTRIB_NO_TIME_CHECK  0x10

#define  MSG_ATTRIB_DISCARD        0x40
#define  MSG_ATTRIB_COMMAND        0x80
//////// DISCARD 原因标志 ////////////
#define  MSG_ATTRIB_DISCARD_DUP    0x08
#define  MSG_ATTRIB_DISCARD_DELAY  0x10

#endif
