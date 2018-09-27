#include "stdafx.h"

struct tagCharTable{
	char text1[3];
	char text2[2];
}DmisCharTable[]=
{{"£Á","A"},{"£Â","B"},{"£Ã","C"},
{"£Ä","D"},{"£Å","E"},{"£Æ","F"},
{"£Ç","G"},{"£È","H"},{"£É","I"},
{"£Ê","J"},{"£Ë","K"},{"£Ì","L"},
{"£Í","M"},{"£Î","N"},{"£Ï","O"},
{"£Ð","P"},{"£Ñ","Q"},{"£Ò","R"},
{"£Ó","S"},{"£Ô","T"},{"£Õ","U"},
{"£Ö","V"},{"£×","W"},{"£Ø","X"},
{"£Ù","Y"},{"£Ú","Z"},{"£á","A"},
{"£â","B"},{"£ã","C"},{"£ä","D"},
{"£å","E"},{"£æ","F"},{"£ç","G"},
{"£è","H"},{"£é","I"},{"£ê","J"},
{"£ë","K"},{"£ì","L"},{"£í","M"},
{"£î","N"},{"£ï","O"},{"£ð","P"},
{"£ñ","Q"},{"£ò","R"},{"£ó","S"},
{"£ô","T"},{"£õ","U"},{"£ö","V"},
{"£÷","W"},{"£ø","X"},{"£ù","Y"},
{"£ú","Z"},{"£±","1"},{"£²","2"},
{"£³","3"},{"£´","4"},{"£µ","5"},
{"£¶","6"},{"£·","7"},{"£¸","8"},
{"£¹","9"},{"£°","0"},{"£«","+"},
{"£­","-"}};

CString ChangeAlpha(CString &input){
	CString temp = input;
	CString tmp;
	CString t_sReturnValue("");

	while(!temp.IsEmpty()){
		if(IsCharAlphaNumeric(temp.GetAt(0))){
			tmp = temp.Left(1);
			t_sReturnValue += tmp;
			temp = temp.Mid(1);
		}else if((BYTE)temp.GetAt(0)>=0xA0){
			tmp = temp.Left(2);
			for(int i = 0;i<sizeof(DmisCharTable)/sizeof(DmisCharTable[0]);i++){
				CString t_stmp;
				t_stmp.Format("%s",DmisCharTable[i].text1);
				if(t_stmp == tmp){
					tmp.Format("%s",DmisCharTable[i].text2);
					break;
				}
			}
			t_sReturnValue += tmp;
			temp = temp.Mid(2);
		}else{
			tmp = temp.Left(1);
			t_sReturnValue += tmp;
			temp = temp.Mid(1);
		}
	}
	return t_sReturnValue;
}

CString ChangeFromHalf2Full(CString &input){
	CString temp = input;
	CString tmp;
	CString t_sReturnValue("");

	while(!temp.IsEmpty()){
		if(IsCharAlphaNumeric(temp.GetAt(0))){
			tmp = temp.Left(1);
			t_sReturnValue += tmp;
			temp = temp.Mid(1);
		}else if((BYTE)temp.GetAt(0)>=0xA0){
			tmp = temp.Left(2);
			for(int i = 0;i<sizeof(DmisCharTable)/sizeof(DmisCharTable[0]);i++){
				CString t_stmp;
				t_stmp.Format("%s",DmisCharTable[i].text1);
				if(t_stmp == tmp){
					tmp.Format("%s",DmisCharTable[i].text2);
					break;
				}
			}
			t_sReturnValue += tmp;
			temp = temp.Mid(2);
		}else{
			tmp = temp.Left(1);
			t_sReturnValue += tmp;
			temp = temp.Mid(1);
		}
	}
	return t_sReturnValue;
}