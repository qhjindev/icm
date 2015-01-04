
#include "HelloI.h"
#include <iostream>
#include <sstream>

using namespace std;

::std::string demo::HelloI::sayHello(const ::std::string& msg, Short u, Long& v)
{
  ostringstream oss;
  oss << "receive:" << " msg:" << msg << " u:" << u << " v:" << v ;
  std::string tmp = oss.str();
  cout << tmp;

  v = 0x1234 + v;

  return tmp;
}

::std::string demo::HelloI::sayBye(const ::std::string& msg, Short u, Long& v)
{
  ostringstream oss;
  oss << "receive:" << " msg:" << msg << " u:" << u << " v:" << v ;
  std::string tmp = oss.str();
  cout << tmp;

  v = 0x1234 + v;

  return tmp;
}
