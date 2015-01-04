#ifndef ICM_ENDPOINT_H
#define ICM_ENDPOINT_H

#include <string>

class InputStream;
class OutputStream;

class Endpoint
{
public:
  Endpoint (const char* prot, const char* h, unsigned short p) :
    protocol (prot), host (h), port (p)
  {
  }

  Endpoint (void) :
    protocol (""), host (""), port (0)
  {
  }

  Endpoint (unsigned short p) :
    protocol ("TCP"), host ("127.0.0.1"), port (p)
  {

  }

  Endpoint (const Endpoint& endpoint);

  Endpoint (InputStream* s);

  void set(const char* prot, const char* h, unsigned short p);

  void write(OutputStream* s) const;

  void read(InputStream* s);

public:

  std::string protocol;
  std::string host;
  unsigned short port;
};

#endif //ICM_ENDPOINT_H
