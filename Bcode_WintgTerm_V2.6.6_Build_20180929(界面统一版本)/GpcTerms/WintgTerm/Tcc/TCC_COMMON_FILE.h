#ifndef TCC_COMMON_FILE
#define TCC_COMMON_FILE

#include <TCC\C2\GpcInfoProvider.h>
#include <TCC\C3\gpcinfoprovider.h>
#include <baseMsg\msgid.h>
#include <baseMsg\gpc_msgid.h>
#include <BaseMsg/clbth.h>

void SendCancelDCmdC3(TCC_C3::RAW_STOCK_COMMAND& c3, int con_id, int verstion);
void SendCancelDCmdC2(TCC::RAW_STOCK_COMMAND& c2, int con_id);

#endif