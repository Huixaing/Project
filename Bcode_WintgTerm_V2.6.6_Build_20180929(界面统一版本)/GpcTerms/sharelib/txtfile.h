#ifndef  TXTFILEINCLD
#define  TXTFILEINCLD
#define  MAXLINELEN 256
#define  MAXSTRLEN  40
////namespace LAYOUTFORM_IMPL{
class CTxtFile{
      protected:
	 int  CanUse;
	 int  LineCount;
	 FILE *fpFile;
	 char aLine[MAXLINELEN];
	 int  Parse1Line(const char *line,char *buf,char *key);
      public:
	 char  *GetLine(char *buf,int len);
	 char  *GetLine(void);
	 virtual void Reset();
	 int  GetLineCount();
	 int  IsOK();
         int  FileOK(){return IsOK();}
	 static int is_tap_char(int ch,int tapchar=-1);
	 static char *sgetstr(const char *p,char *dest,int len,int tapchar=-1);
	 static int   GetSubStr(const char *s,char strings[][MAXSTRLEN],int len,int tapchar=-1);
	 CTxtFile(){fpFile=NULL;};
	 CTxtFile(const char *file);
	 ~CTxtFile();
	 int OpenFile(const char *fname);
	 void CloseFile();
	 int  IsCommentLine(const char *s);
	 int  GetLineX(char *buf,char *key);
	 long GetFileLen();
	 int  GetCharFromFile();
	
     };
////};
////using namespace LAYOUTFORM_IMPL;
#endif
