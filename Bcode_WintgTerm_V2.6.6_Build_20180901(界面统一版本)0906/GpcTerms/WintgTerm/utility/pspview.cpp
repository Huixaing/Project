#include "stdafx.h"
#include <afxcmn.h>
#include "../PropTable/PropTree.h"
#include "pspview.h"


BOOL  CCTCPspCtrl::InitProperty(struct PropertyInfoPara *pInfo0,int infoCount0)
{
	CPropTreeItem* pRoot=NULL;
	int i;
	if(infoCount0>MAX_PSP_NUM)
		return FALSE;
	infoCount=infoCount0;;
	for(i=0;i<infoCount;i++)
	{
	  pInfo[i]=pInfo0[i];
	  if(pInfo[i].flag==0)
	  {
	     pRoot =InsertItem(new CPropTreeItem());
	     pRoot->SetLabelText(pInfo[i].caption);
	     pRoot->Expand(); // have this item expanded by default
	     pItem[i]=pRoot;
	  }
	  else
	  if(pInfo[i].flag==1)
	  {
		 CPropTreeItem* pItemStatic;
		 pItemStatic = InsertItem(new CPropTreeItemStatic(), pRoot);
		 pItemStatic->SetLabelText(pInfo[i].caption);
		 pItem[i]=pItemStatic;
	  }
	  else
	  if(pInfo[i].flag==2)
	  {
		CPropTreeItemEdit* pEdit;
		pEdit = (CPropTreeItemEdit*)InsertItem(new CPropTreeItemEdit(), pRoot);
		pEdit->SetLabelText(pInfo[i].caption);
		pItem[i]=pEdit;
	  }
	  else
	  if(pInfo[i].flag==3)
	  {
		  CPropTreeItemCombo* pCombo;
	  	  pCombo = (CPropTreeItemCombo*)InsertItem(new CPropTreeItemCombo(), pRoot);
		  pCombo->SetLabelText(pInfo[i].caption);
	      	//pCombo->CreateComboBoxBool();	// create the ComboBox control and auto fill with TRUE/FALSE values
	      DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VSCROLL        //| WS_VISIBLE
                    | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST; // for grid style only
		  pCombo->CreateComboBox(dwStyle);
		  int tmpindex=0;
		  int rela_value=-1;
		  while(1)
		  {
			 const char *ptmpstr=GetCTCTypeChoiceString(pInfo[i].data_type,tmpindex++,&rela_value);
			 if(!ptmpstr)
				 break;
			 pCombo->AddStringWithRelaValue(ptmpstr,rela_value);
			 tmpindex++;
		  }
		  pItem[i]=pCombo;
	  }
	  else
	  if(pInfo[i].flag==4)
	  {
		  CPropTreeItemDateTime *pdatactrl;
		  
	  	  pdatactrl = (CPropTreeItemDateTime*)InsertItem(new CPropTreeItemDateTime(), pRoot);
		  pdatactrl->SetLabelText(pInfo[i].caption);		  
	      	//pCombo->CreateComboBoxBool();	// create the ComboBox control and auto fill with TRUE/FALSE values
	      DWORD dwStyle = WS_BORDER | DTS_TIMEFORMAT;
		  pdatactrl->CreateCtrl(dwStyle);	
		  pItem[i]=pdatactrl;	
	  }
	}
	//set group info
	CStringArray itemArray;
	CString tmp;

	for ( i = 0; i < infoCount; i++)
	{
		tmp += pInfo[i].caption;
		tmp+= "%";
	}
	itemArray.Add(tmp);
	
	
	SetGroupInfo(itemArray);
	return TRUE;
}

int  CCTCPspCtrl::GetInfoIndex(int id)
{
	for(int i=0;i<infoCount;i++)
	{
		if(pInfo[i].id==id)
			return i;
	}
	return -1;

}

const char *CCTCPspCtrl::GetValue_String(void *p,int id)
{
	return NULL;
}

int    CCTCPspCtrl::GetValue_Integer(void *p,int id)
{
	return -1;
}

 const char *CCTCPspCtrl::GetItemListChoice(void *p,int id,int index,int *value)
 {
	int i=GetInfoIndex(id);
	if(i<0)
		return NULL;
	 return GetCTCTypeChoiceString(pInfo[i].data_type,index,value);
 }

 BOOL  CCTCPspCtrl::SaveValue_String(void *p,int id,const char *value)
 {
	 return FALSE;
 }

 BOOL CCTCPspCtrl::SaveValue_Integer(void *p,int id,int value)
 {
	 return FALSE;
 }

BOOL  CCTCPspCtrl::LoadProperty(void *p)
{
	int i;
	for(i=0;i<infoCount;i++)
	{
	  CPropTreeItem* pItem=GetItemByTextLabel(pInfo[i].caption);
	  if(!pItem)
		  continue;
	  if(!p)
	  {
		if(pInfo[i].flag==4 || pInfo[i].flag==3) // time
			pItem->SetRelaItemValue((LPARAM)-1);
	    else
			pItem->SetItemValue((LPARAM)"");
	  }
	  else
	  {
	  if(pInfo[i].flag==4 || pInfo[i].flag==3) // time
		 pItem->SetRelaItemValue(GetValue_Integer(p,pInfo[i].id));
	  else
	   {
	     const char *strvalue=GetValue_String(p,pInfo[i].id);
		 int  intvalue=GetValue_Integer(p,pInfo[i].id);
		 if(strvalue) //优先使用字符串形式
			 pItem->SetItemValue((LPARAM)strvalue);
		 else
		 {
			 if(intvalue>=0)
		        pItem->SetRelaItemValue(intvalue);
		 }
	   }
	  }
	}
	Invalidate();
	return TRUE;
}

BOOL  CCTCPspCtrl::SaveProperty(void *p)
{
     int i;
	 for( i=0;i<infoCount;i++)
	 {
      
      if(pInfo[i].flag==4 || pInfo[i].flag==3) // time
	  {
		 int value=pItem[i]->GetRelaItemValue();
		 SaveValue_Integer(p,pInfo[i].id,value);		 
	  }
	  else
	  {
		 CString value=(const char *)pItem[i]->GetItemValue();
		 SaveValue_String(p,pInfo[i].id,value);		 		 
	  }
	 }
	 return TRUE;
}