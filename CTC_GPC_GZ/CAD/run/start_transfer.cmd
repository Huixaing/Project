taskkill /F /IM svr_transfer.exe /T
start "数据转发" svr_transfer.exe ../config/config.transfer
