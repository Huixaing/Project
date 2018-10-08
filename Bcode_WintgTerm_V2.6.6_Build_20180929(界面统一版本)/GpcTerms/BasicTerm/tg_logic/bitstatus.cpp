#include "stdafx.h"
#include "bitstatus.h"

CBitStatus::CBitStatus()
{
	m_value=0;
	m_valueExtra=0;
}

bool CBitStatus::operator== (const CBitStatus &src) const
{
	if(m_value==src.m_value && m_valueExtra==src.m_valueExtra)
		return true;
	else
		return false;
}

unsigned long   CBitStatus::GetLongValue() const
	   {
		   return m_value;
	   }

unsigned long CBitStatus::GetLongValueExtra(unsigned long &pValueExtra) const
{
	pValueExtra=m_valueExtra;
	return m_value;
}

void  CBitStatus::SetLongValue(unsigned long va)
	   {
		   m_value=va;
	   }

void CBitStatus::SetLongValueExtra(unsigned long va,unsigned long valueExtra)
{
	m_value=va;
	m_valueExtra=valueExtra;
}

void  CBitStatus::SetBit(int bit,int va)
{
	if (bit<MAX_BIT_COUNT && bit >=0)	//0~31
	{
		if(va)
			  m_value|=(unsigned long)1<<bit;
		else
			  m_value&=~((unsigned long)1<<bit);
	}
	if (bit>=MAX_BIT_COUNT && bit<(MAX_BIT_EXTRA_COUNT+MAX_BIT_COUNT) )  //32~63
	{
		int bitExtra=bit -MAX_BIT_COUNT;
		if (va)
			m_valueExtra|=(unsigned long )1<<bitExtra;
		else
			m_valueExtra&=~((unsigned long)1<<bitExtra);
	}
		
}

void  CBitStatus::SetMaskBit(unsigned long  mask,int va)
		{
			if(va)
			  m_value|=mask;
			else
			  m_value&=~mask;
	   }

void  CBitStatus::SetMaskBitExtra(unsigned long  mask,unsigned long maskExtra,int va)
{
	if(va)
	{
		m_value|=mask;
		m_valueExtra|=maskExtra;
	}
	else
	{
		m_value&=~mask;
		m_valueExtra&=~maskExtra;
	}
}

int  CBitStatus::GetBit(int bit) const
{
	if (bit<MAX_BIT_COUNT && bit >=0)	//0~31
		return  ((m_value >>bit)&0x01); 
	if (bit>=MAX_BIT_COUNT && bit<(MAX_BIT_EXTRA_COUNT+MAX_BIT_COUNT) )  //32~63
	{
		int bitExtra=bit -MAX_BIT_COUNT;
		return  ((m_valueExtra >>bitExtra)&0x01); 
	}
	return 0;   //·Ç0~63 ·µ»Ø0
}	

void  CBitStatus::SetAllBitStatus(int va)
{
   if(va)
   {
	   m_value=0xFFFFFFFF;
	   m_valueExtra=0xFFFFFFFF;
   }
   else
   {
	   m_value=0;
	   m_valueExtra=0;
   }
}

int  CBitStatus::IsAllBitZero() const
	{
	  if(m_value==0 && m_valueExtra==0)
		   return 1;
	  else
		   return 0;
	}

void  CBitStatus::SetSubValue(int frombit,int tobit,int va)
	{
	   int i,bit;
	   for(i=0,bit=frombit;bit<=tobit;i++,bit++)
	   {
		  if((va >>i)&1)
		     SetBit(bit,1);
		  else
			 SetBit(bit,0);
	   }
	}

int   CBitStatus::GetSubValue(int frombit,int tobit) const
	{
	   int bit;
	   int retv;
	   retv=0;
	   for(bit=frombit;bit<=tobit;bit++)
	   {
		  if(GetBit(bit))
		     retv|=1;
		  retv<<=1;
	   }
	   return retv;
	}

int    CBitStatus::Get1BitsCount() const
{
    int rc=0;
	for(int i=0;i<MAX_BIT_COUNT+MAX_BIT_EXTRA_COUNT;i++)
	{
		if(GetBit(i))
			rc++;
	}
	return rc;
}
int    CBitStatus::Get0BitsCount() const
{
	int rc=0;
	for(int i=0;i<MAX_BIT_COUNT+MAX_BIT_EXTRA_COUNT;i++)
	{
		if(!GetBit(i))
			rc++;
	}
	return rc;
}

int   CBitStatus::CheckMultiBitValue(unsigned long mask,int va) const
{
	if(va)
	{
		if((GetLongValue() & mask)==mask)
			return 1;
		else
			return 0;
	}
	else
	{
        if(((~GetLongValue()) & mask)==mask)
			return 1;
		else
			return 0;
	}
	
}

int   CBitStatus::CheckMultiBitValueExtra(unsigned long  mask,unsigned long maskExtra,int va) const
{
	unsigned long  value,valueExtra;
	value = GetLongValue();
	valueExtra=0;//GetLongValueExtra();
	if (va)
	{
		if( ((value & mask) ==mask) && ((valueExtra & maskExtra) ==maskExtra) )
			return 1;
		else 
			return 0;
	}
	else
	{
		if( ((~value & mask) ==mask) && ((~valueExtra & maskExtra) ==maskExtra) )
			return 1;
		else
			return 0;
	}
}




