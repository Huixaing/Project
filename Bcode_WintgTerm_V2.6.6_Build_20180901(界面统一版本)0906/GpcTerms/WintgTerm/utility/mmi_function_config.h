#ifndef __INCLUDE_MMI_FUNCTION_CONFIG___
#define __INCLUDE_MMI_FUNCTION_CONFIG___
#include <vector>
struct ConfigItemIntegerStruct{
	char szConfigName[32];
	int  lConfigValue;
	ConfigItemIntegerStruct()
	{
		memset(szConfigName,0,sizeof(szConfigName));
		lConfigValue=-1;
	}
};

class CMMIFunctionConfig{
private:
	vector<ConfigItemIntegerStruct> m_aConfigItems;
public:
	CMMIFunctionConfig();
	BOOL  InitData(const char *cfgfile);
	int   GetConfigIntegerItem(const char *name) const;
};

CMMIFunctionConfig *GetMMIFunctionConfig();
////////////////////
#define CONFIG_SHOW_LOCONO			"SHOW_LOCO_NO"
#define CONFIG_EDIT_ACTUAL			"EDIT_ACTUAL"
#define CONFIG_COMB_NOTEPAD			"COMB_NOTEPAD"
#define CONFIG_DEFAULT_START		"DEFAULT_START"
#define CONFIG_DEFAULT_TERMINAL		"DEFAULT_TERMINAL"
#define CONFIG_SHOW_THIN_LINE		"SHOW_THIN_LINE"
#define CONFIG_FREE_ADDTRAIN		"FREE_ADDTRAIN"
#define CONFIG_ADD_TRAIN_BY_BASIC	"ADD_TRAIN_BY_BASIC"
#define CONFIG_SHOW_ELEC_ENGINE_NO		"SHOW_ELEC_ENGINE_NO"
#define CONFIG_SHOW_EARLY_LATE_TIME		"SHOW_EARLY_LATE_TIME"
#define CONFIG_NO_SHOW_REVERSE		"NO_SHOW_REVERSE"
#define CONFIG_CHECK_MANUAL_OPER		"CHECK_MANUAL_OPER"
#define CONFIG_ADD_TRAIN_SEL_GOOD		"ADD_TRAIN_SEL_GOOD"
#define CONFIG_ADD_TRAIN_SEL_NONE_START	"ADD_TRAIN_SEL_NONE_START"

#define CONFIG_JIAOBAN				"ENABLE_JIAOBAN"
#define CONFIG_SHOW_CZH_LINE		"SHOW_CZH_LINE"
#define CONFIG_MOVE_SAME_CZ_AFTER_TRAIN	"MOVE_SAME_CZ_AFTER_TRAIN"
////////////////////
#endif

