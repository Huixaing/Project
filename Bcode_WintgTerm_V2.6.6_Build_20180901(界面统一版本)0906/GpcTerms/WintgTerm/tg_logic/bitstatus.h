#ifndef MISC_UTILITY_INCLUDE_123
#define MISC_UTILITY_INCLUDE_123

class  CBitStatus{
	enum {MAX_BIT_COUNT=32};
	enum {MAX_BIT_EXTRA_COUNT=32};
	unsigned long   m_value;
	unsigned long   m_valueExtra;
public:
	   CBitStatus();
	   bool operator== (const CBitStatus &src) const;
	   unsigned long   GetLongValue() const;
	   unsigned long   GetLongValueExtra(unsigned long &pValueExtra) const;
	   void  SetLongValue(unsigned long va);
	   void  SetLongValueExtra(unsigned long va,unsigned long valueExtra);
	   void  SetBit(int bit,int va);
	   void  SetMaskBit(unsigned long  mask,int va);
	   void  SetMaskBitExtra(unsigned long  mask,unsigned long maskExtra,int va);
	   int  GetBit(int bit) const;
  	  int   IsAllBitZero() const;
	  void  SetAllBitStatus(int va);
	  int   GetSubValue(int frombit,int tobit) const;
	  void  SetSubValue(int frombit,int tobit,int va);
	  int   Get1BitsCount() const;
      int   Get0BitsCount() const;
	  int   CheckMultiBitValue(unsigned long mask,int va) const;
	  int	CheckMultiBitValueExtra(unsigned long  mask,unsigned long maskExtra,int va) const;
};
#endif
