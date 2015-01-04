#include "icm/Endpoint.h"
#include <icm/IcmStream.h>

Endpoint::Endpoint(const Endpoint& endpoint) {
	this->protocol = endpoint.protocol;
	this->host = endpoint.host;
	this->port = endpoint.port;
}

Endpoint::Endpoint(InputStream* s) :
		protocol(""), host(""), port(0) {
	s->read_string(protocol);
	s->read_string(host);
	s->read_ushort(port);
}

void
Endpoint::set(const char* prot, const char* h, unsigned short p) {
  this->protocol = prot;
  this->host = h;
  this->port = p;
}

void
Endpoint::write(OutputStream* s) const {
	s->write_string(protocol);
	s->write_string(host);
	s->write_ushort(port);
}

void
Endpoint::read(InputStream* s) {
  s->read_string(protocol);
  s->read_string(host);
  s->read_ushort(port);
}
