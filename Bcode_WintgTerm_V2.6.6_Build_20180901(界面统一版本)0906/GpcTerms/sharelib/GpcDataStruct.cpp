
#include "stdafx.h"

#include "GpcDataStruct.h"

int GPCSTATION::loadContent(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n*";

	// load station number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	this->nStation = (int)atoi(token);

	// load station name
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strncpy(this->szStationName, token, sizeof(this->szStationName));
	// load station page number
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	//no used now this->nDispPage = (int)atoi(token);

	// load page width and height
	token = strtok(NULL, seps); 
	if(token==NULL) return 0;
	this->sizeDisp.cx = atoi(token);
	token = strtok(NULL, seps); 
	if(token==NULL) return 0;
	this->sizeDisp.cy = atoi(token);

	// load station distance sign 
	token = strtok(NULL, seps); 
	if(token==NULL) return 0;
	strncpy(this->fStaDistance, token, 63);
	fStaDistance[63]=0;

 	// load draw draw train start or arrive time flag
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	this->bDrawTrainTime= (USHORT)atoi(token);
	
	// load station class
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	this->nStationClass= (USHORT)atoi(token);

	//车站性质://车站性质:-1, 无设备, 0,TDCS, 1, CTC
	token = strtok(NULL,seps);
	if(token== NULL) 
	{
		this->nProperty = -1;
	}
	//return 0;
	else
      this->nProperty = (int)atoi(token);


	strcpy(szRtServerName, "");
	token=strtok(NULL,seps);
	if(token)
	{
		int nRtNum = atoi(token);
		for(int i=0; i<nRtNum; i++)
		{
			token=strtok(NULL,seps);
			if(0==i)
				strncpy(szRtServerName, token, sizeof(szRtServerName));
		}
	}

	//static  int nStationNum = 0;
	//this->nId  = nStationNum;
	//nStationNum ++;

	return 1;
}



int GPCDIRECTIONSTRING::loadContent(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	// load section ID
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	this->iSectionID = atoi(token);
	// load Station id 
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	this->iStationID = (int)atoi(token);
	
	// load Visible String
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	this->sDirString = token;

	// success return
    return 1;
}


int GPCPAGE::loadContent(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i;

//	[page] = 1, 1/1
	// load page id
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	this->nPageNO = atoi(token);
	// load page station num;
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	this->nStationNum = atoi(token);
	for(i=0;i < this->nStationNum;i++)
	{
		// load page station list;
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
		this->nStationList[i] = atoi(token);
	}

	// load page neighbour station num;
	token = strtok(NULL, seps);
	if(token==NULL)
		this->nNeighbourNum = 0;
	else
		this->nNeighbourNum = atoi(token);
	for(i=0;i < this->nNeighbourNum;i++)
	{
		// load page neighbour station list;
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
		this->nNeighbourList[i] = atoi(token);
	}

	return 1;
}
