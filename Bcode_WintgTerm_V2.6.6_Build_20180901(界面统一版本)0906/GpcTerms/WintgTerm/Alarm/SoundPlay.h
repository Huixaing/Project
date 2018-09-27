#pragma once
void PlaySimpleAlarmSound(int nAlarmID,int source);
BOOL LoadPlaySoundConfig(const char *sndfilename);
void EnableSoundPlay(BOOL va);
BOOL SoundPlayEnabled();