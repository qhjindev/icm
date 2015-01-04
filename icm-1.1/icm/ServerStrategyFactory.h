#ifndef ICM_SERVER_STRATEGY_FACTORY_H
#define ICM_SERVER_STRATEGY_FACTORY_H

class ServerStrategyFactory
{
public:
  ServerStrategyFactory()
  : mActivateServerConnections(0)
  {}

  int activateServerConnections (void)
  {
    return this->mActivateServerConnections;
  }

protected:

  int mActivateServerConnections;

};

#endif //ICM_SERVER_STRATEGY_FACTORY_H
