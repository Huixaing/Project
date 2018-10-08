#ifndef _ITEM_NAME_
#define _ITEM_NAME_
/* some log use chinese name */

#define SEND_STPLAN_ORDER     \
	"%s-%s 下达发送计划命令：车站号 %02d 台号 %02d 接受设备号%04d 发送设备号%04d 发送者%s 发送时间 %s-%s 时间范围 (%s-%s %s-%s)"

#define REQUEST_STPLAN        \
	"%s-%s 收到计划请求命令：车站号 %02d 台号 %02d 请求设备号%04d 时间范围 (%s-%s %s-%s)"

#define SEND_STPLAN_BEGIN     \
	"%s-%s 开始发送计划： 车站号 %02d 台号 %02d 发送设备号%04d 接受设备号%04d 发送者%s 发送时间 %s-%s"

#define SEND_STPLAN_INSERT    \
	"%s-%s 发送增加计划： 车站号 %02d 台号 %02d 发送设备号%04d 接受设备号%04d 列车索引 %d-%d 列车类型%d 运行等级%d 到达车次 %s 出发车次 %s 到达时间 %s-%s(图定到达 %d) 出发时间 %s-%s(图定出发 %d) 入口 %d 股道 %d %d 出口 %d 最小停站时间 %d 允许提前发车时间 %x 会让个数 %d 状态位(status) %02X 停车状态(stop_condition) %02X 标志位(flag) %02X"

#define SEND_BASICSTPLAN_INSERT    \
	"发送基本图计划： 车站号 %02d 接受设备号%04d 到达车次 %s 出发车次 %s 到达时间 %s(%d) 出发时间 %s(%d) 入口 %d 股道 %d 出口 %d 前一站 %d 后一站 %d"

#define SEND_STPLAN_MEET_SAME_DIR    \
	"让车 索引 %d-%d "

#define SEND_STPLAN_MEET_DIFF_DIR    \
	"会车 索引 %d-%d "

#define DEPART_TRAIN_BY_PLAN    \
	"按计划发车 %d "

#define SEND_STPLAN_DELETE    \
	"%s-%s 发送删除计划： 车站号 %02d 台号 %02d 发送设备号%04d 接受设备号%04d 列车索引 %d-%d 到达车次 %s 出发车次 %s 到达时间 %s-%s 出发时间 %s-%s"

#define SEND_STPLAN_END       \
	"%s-%s 结束发送计划： 车站号 %02d 台号 %02d 发送设备号%04d 接受设备号%04d 增加个数 %d 删除个数 %d"

#define SEND_STPLAN_ANSWER_PLAN_RECV          \
	"%s-%s 发送计划应答： 车站号 %02d 台号 %02d 接受设备号%04d 计划接收"

#define SEND_STPLAN_ANSWER_PLAN_SIGN          \
	"%s-%s 发送计划应答： 车站号 %02d 台号 %02d 接受设备号%04d 计划签收"

#define SEND_STPLAN_ANSWER_PLAN_SENDING       \
	"%s-%s 发送计划应答： 车站号 %02d 台号 %02d 接受设备号%04d 计划发送中"

#define SEND_STPLAN_ANSWER_PLAN_NO_CHANGE     \
	"%s-%s 发送计划应答： 车站号 %02d 台号 %02d 接受设备号%04d 计划无变化"

#define SEND_STPLAN_ANSWER_PLAN_NO_DATA       \
	"%s-%s 发送计划应答： 车站号 %02d 台号 %02d 接受设备号%04d 无计划数据"

#define SEND_STPLAN_ANSWER_PLAN_ENTITY_DEAD   \
	"%s-%s 发送计划应答： 车站号 %02d 台号 %02d 接受设备号%04d 接受设备连接中断"

#define SEND_STPLAN_ANSWER_PLAN_CONFIG_WRONG  \
	"%s-%s 发送计划应答： 车站号 %02d 台号 %02d 接受设备号%04d 下计划配置错误"

#define SEND_STPLAN_ANSWER_ERR                \
	"%s-%s 发送计划应答： 车站号 %02d 台号 %02d 接受设备号%04d 错误状态"

#define SEND_STPLAN_RECV     \
	"%s-%s 接受计划回执： 车站号 %02d 台号 %02d 接受"

#define SEND_STPLAN_SIGN     \
	"%s-%s 接受计划回执： 车站号 %02d 台号 %02d 签收"

#define SEND_STPLAN_RECV_ALL \
	"%s-%s 接受计划回执： 车站号 %02d 台号 %02d 全部接受"

#define SEND_STPLAN_SIGN_ALL \
	"%s-%s 接受计划回执： 车站号 %02d 台号 %02d 全部签收"

#define SEND_STPLAN_RECV_WRONG \
	"%s-%s 接受计划回执： 车站号 %02d 台号 %02d 错误"

#define UPDATE_STPLAN_SIDE \
	"%s-%s 接受修改计划股道： 车站号 %02d 台号 %02d 列车索引 %d-%d 发送设备号%04d"

#define UPDATE_BASIC_SCHEDULE \
	"%s-%s 台号 %02d %d->%d 设备号%04d 更新基本图 %s"

#define UPDATE_BASIC_SCHEDULE_ERR_INVALID_SCHDTYPE \
	"%s-%s 台号 %02d %d->%d 更新基本图错误：无效的运行图类型"

#define UPDATE_BASIC_SCHEDULE_ERR_SAVE_OLD \
	"%s-%s 台号 %02d %d->%d 更新基本图错误：保存旧运行图"

#define UPDATE_BASIC_SCHEDULE_ERR_UPDATE_NEW \
	"%s-%s 台号 %02d %d->%d 更新基本图错误：转换基本图失败"

#define UPDATE_BASIC_SCHEDULE_OK \
	"%s-%s 台号 %02d %d->%d 设备号%04d 更新基本图成功"

#define ENTITY_ACTIVE \
	"%s-%s 设备%04d 连接"

#define ENTITY_INACTIVE \
	"%s-%s 设备%04d 断开"

#define TCC_SVR_ALARM "存在未执行的列控命令: "

#define COPY_RIGHT         "卡斯柯信号有限公司版权所有"

#endif /** THE END **/
