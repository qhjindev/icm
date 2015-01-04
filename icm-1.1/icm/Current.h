#ifndef ICM_CURRENT_H
#define ICM_CURRENT_H

#include <string>
#include "Object.h"
#include "ObjectAdapter.h"
#include "Connection.h"

struct Current
{
  ObjectAdapter* adapter;
  //Connection* con;
  Identity* id;
  std::string operation;
  Context ctx;
};

#endif //ICM_CURRENT_H
