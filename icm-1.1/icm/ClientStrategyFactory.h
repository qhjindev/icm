
#ifndef ICM_CLIENT_STRATEGY_FACTORY_H
#define ICM_CLIENT_STRATEGY_FACTORY_H

class TransportMuxStrategy;
class WaitStrategy;
class IcmTransport;

class ClientStrategyFactory
{
public:
  ClientStrategyFactory (void);

  virtual ~ClientStrategyFactory (void);

  virtual int init ();

  TransportMuxStrategy* createTransportMuxStrategy (IcmTransport* transport);

  WaitStrategy* createWaitStrategy (IcmTransport* transport);

private:

  enum TransportMuxStrategyEnum
  {
    ICM_MUXED_TMS,
    ICM_EXCLUSIVE_TMS
  };

  TransportMuxStrategyEnum mTransportMuxStrategy;

  enum WaitStrategyEnum
  {
    ICM_WAIT_ON_REACTOR,
    ICM_WAIT_ON_READ
  };

  WaitStrategyEnum mWaitStrategy;

};

#endif //ICM_CLIENT_STRATEGY_FACTORY_H
