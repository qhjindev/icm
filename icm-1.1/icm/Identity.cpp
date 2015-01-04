#include <string>
#include <map>
#include "icm/Identity.h"
#include "icm/IcmStream.h"

void Identity::write(OutputStream* os) const {
	os->write_string(name);
	os->write_string(category);
}

void Identity::read(InputStream* is) {
	is->read_string(name);
	is->read_string(category);
}
