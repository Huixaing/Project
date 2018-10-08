
/* messages.h */

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define		SCHD_START				1
#define     SCHD_ACCEPT             2
#define		SCHD_TERMINAL			3
#define     SCHD_GIVEOUT            4
#define		SCHD_PASS				5
#define		SCHD_STOP				6
#define		STOP_ABS				10
#define		STOP_SIGNAL				11

#define ARRIVAL_POSITION        1
#define DEPART_POSITION		    2
#define UNION_POSITION          3
#define ABS_ARRIVAL_POSITION    4
#define ABS_DEPART_POSITION     5
#define ABS_START_POSITION      6

#define UP_DIRECT                       	0
#define DOWN_DIRECT                     	1
#define DUAL_DIRECT                         2

#define UM_CHANGE_STATION_STATUS	     	23
#define FOCUS_SECTION_CHANGE			    14
#define GRAPH_ROOM_CHANGE					15
#define GRAPH_BEGIN_TIME_CHANGE				16
#define MSG_MOUSE_MOVE_OUTSIDE      		555
#define MSG_DOUBLE_CLICK_STATION      		231
#define MSG_RE_LAYOUT_BMP_SIZE              2355
#define OPERATOR_STATUS_CHANGE          	6
#define VERTICAL_POSITION_CHANGE_WORK       3
#define ADD_TRAIN_FREE                      1
#define HORIZONTAL_POSITION_CHANGE      	2

#define	CLR_DARKBLUE		RGB(0,0,250)
#define CLR_WHITE		RGB(255,255,255)	//RGB(255,255,255)
#define CLR_BLACK		RGB(0,0,0)
#define CLR_DARKBROWN	RGB(24,114,96)
#define CLR_PALEGRAY	RGB(210,210,210)
#define CLR_GRAY		RGB(127,127,127)
#define CLR_DARKGRAY		RGB(96,96,96)
#define CLR_RED			RGB(255,0,0)
#define CLR_ORANGE		RGB(255,128,0)
#define CLR_INDIANRED   	RGB(255,128,128)
#define CLR_GREEN		RGB(0,255,0)
#define CLR_DARKGREEN		RGB(0,100,0)
#define CLR_SPRINGGREEN 	RGB(0,255,128)
#define CLR_LAWNGREEN		RGB(0,200,0)
#define CLR_LIGHTRED		RGB(255,128,128)
#define CLR_LIGHTGREEN  	RGB(128,255,0)
#define CLR_PRINT          RGB(50,255,50)
//
#define CLR_BLUE		RGB(0,0,255)
#define CLR_CYAN		RGB(0,255,255)
#define	CLR_DARKCYAN		RGB(127,255,255)
#define	CLR_DARKPINK		RGB(255,20,147)
#define	CLR_PINK		RGB(255,127,80)
#define CLR_LIGHTCYAN		RGB(128,255,255)
#define CLR_DARKYELLOW		RGB(255,255,0)
#define CLR_YELLOW		RGB(255,255,0)
#define CLR_MAGENTA		RGB(255,0,255)
#define CLR_PURPLE		RGB(165,62,204 )
#define CLR_SKYBLUE		RGB(126,192,238)
#define CLR_NAVY		RGB(0,0,128)
#define CLR_GOLD		RGB(255,215,0)
#define CLR_BROWN		RGB(165,42,42)
//
#define CLR_LAST_PLAN_REGION   RGB(206,206,206)
#define CLR_CLOSE_UP_REGION    RGB(226,226,226)
#define CLR_ADJUST_REGION      RGB(240,240,240)

#endif
 
