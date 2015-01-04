#ifndef _ICC_ADDR_H_
#define _ICC_ADDR_H_

#include "os/ACE_export.h"

class ACE_Export Addr
{
public:
  Addr (int type = -1, int size = -1);

  virtual ~Addr (void) {};

  int getSize (void) const;

  void setSize (int size);

  int getType (void) const;

  void setType (int type);

  virtual void *getAddr (void) const;

  virtual void setAddr (void*, int len);

  void baseSet (int type, int size);

  static const Addr sapAny;

  bool operator == (const Addr &addr) const;

  bool operator != (const Addr &addr) const;

protected:
  int m_addrType;
  int m_addrSize;
};

#endif //_ICC_ADDR_H_
