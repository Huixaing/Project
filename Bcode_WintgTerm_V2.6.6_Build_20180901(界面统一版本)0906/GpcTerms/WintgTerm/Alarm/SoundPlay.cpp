#include "stdafx.h"
#include "windowsx.h"
#include "Mmsystem.h"
#include "alarm/AlarmSoundConfig.h"
#include "memshare.h"
#include "gpcdef.h"
static CAlarmSoundConfig  g_alarmSoundConfig;
static BOOL   g_enableSound;
BOOL SoundPlayEnabled()
{
	return g_enableSound;
}
void EnableSoundPlay(BOOL va)
{
	g_enableSound=va;
}

BOOL LoadPlaySoundConfig(const char *sndfilename)
{
	g_alarmSoundConfig.loadProfile(sndfilename);
	g_enableSound=TRUE;
	return TRUE;
}
void PlaySimpleAlarmSound(int nAlarmID,int source)
{
	if(!g_enableSound)
		return;
	CString ret_filename;
	ret_filename = gpcdef.GetDataPathName();
	ret_filename += "\\Sound\\alarm\\";
	CString  sndfile=g_alarmSoundConfig.GetSoundFileName(nAlarmID);
	if(sndfile.IsEmpty())
		ret_filename+="Alarm.wav";
	else
		ret_filename+=sndfile;
	PlaySound(ret_filename, NULL, SND_FILENAME|SND_ASYNC);
}
