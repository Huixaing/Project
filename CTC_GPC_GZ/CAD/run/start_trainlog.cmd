taskkill /F /IM svr_trainlog.exe /T
start "数据转发" svr_trainlog.exe ../config/config.trainlog
