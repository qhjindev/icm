#ifndef ICM_TRANSPORT_CACHE_MANAGER_H
#define ICM_TRANSPORT_CACHE_MANAGER_H

//#include "icc/InetAddr.h"
//#include "icc/Pluggable.h"

#include <map>

class InetAddr;
class IcmTransport;

class TransportCacheManager
{
public:
  
  typedef std::map<InetAddr, IcmTransport*> HashMap;
  typedef std::map<InetAddr, IcmTransport*>::iterator HashMapIter;

public:

  int cacheTransport (InetAddr& addr, IcmTransport* transport);

  int findTransport (InetAddr& addr, IcmTransport*& transport);

  int closeTransport (InetAddr& addr);

  int purgeEntry (InetAddr& addr);


private:

  HashMap mCacheMap;

};

#endif //ICM_TRANSPORT_CACHE_MANAGER_H
