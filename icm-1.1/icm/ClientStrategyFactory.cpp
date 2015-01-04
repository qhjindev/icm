#include "icm/ClientStrategyFactory.h"
#include "icm/WaitStrategy.h"
#include "icm/TransportMuxStrategy.h"
#include "icm/Pluggable.h"

ClientStrategyFactory::ClientStrategyFactory()
{
//  this->mWaitStrategy = ICM_WAIT_ON_REACTOR;
  this->mWaitStrategy = ICM_WAIT_ON_READ;

  this->mTransportMuxStrategy = ICM_MUXED_TMS;
}

ClientStrategyFactory::~ClientStrategyFactory (void)
{

}

int
ClientStrategyFactory::init ()
{
  return 0;
}

TransportMuxStrategy*
ClientStrategyFactory::createTransportMuxStrategy(IcmTransport *transport)
{
  TransportMuxStrategy* tms = 0;

  if (this->mTransportMuxStrategy == ICM_MUXED_TMS)
    tms = new MuxedTms (transport);
  else
    tms = new ExclusiveTms (transport);

  return tms;
}

WaitStrategy*
ClientStrategyFactory::createWaitStrategy(IcmTransport *transport)
{
  WaitStrategy* ws = 0;

  if (this->mWaitStrategy == ICM_WAIT_ON_READ)
    ws = new WaitOnRead (transport);
  else if (this->mWaitStrategy == ICM_WAIT_ON_REACTOR)
    ws = new WaitOnReactor (transport);

  return ws;
}
