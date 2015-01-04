
#include "icc/Log.h"
#include "icc/InetAddr.h"
#include "icm/Pluggable.h"

#include "icm/TransportCacheManager.h"


int
TransportCacheManager::cacheTransport (InetAddr& addr, IcmTransport* transport)
{
  LogDebug << "cache transport for addr " << addr.getHostAddr() << ":" << addr.getPortNumber() << endl;
  this->mCacheMap.insert (std::make_pair (addr, transport));
  return 0;
}

int
TransportCacheManager::findTransport (InetAddr& addr, IcmTransport*& transport)
{
  int result = -1;

  HashMapIter iter = this->mCacheMap.find(addr);
  if (iter != this->mCacheMap.end()) {
    transport = iter->second;
    result = 0;
  }

  return result;
}

int
TransportCacheManager::closeTransport (InetAddr& addr)
{
  LogDebug << "uncache transport for addr " << addr.getHostAddr() << ":" << addr.getPortNumber() << endl;
  this->mCacheMap.erase (addr);
  return 0;
}

int
TransportCacheManager::purgeEntry (InetAddr& addr)
{
  return -1;
}
