
#include "icc/Configuration.h"
#include "icc/Log.h"
#include <string>
//#include <iostream>

using namespace std;

int main (int argc, char *argv[])
{
  Configuration config;
  if (config.open ("data.ini") != 0)
    return -1;

  string value1 = config.getConfig ("SERVER", "listenPort");
  //cout << value1 << endl;
  LogMsg << "SERVER listen port:" << value1 << endl;

  string value2 = config.getConfig ("REGISTERSERVER", "registerServerIp");
  //cout << value2 << endl;

  return 0;
}
