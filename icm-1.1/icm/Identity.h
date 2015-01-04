#ifndef ICM_IDENTITY_H
#define ICM_IDENTITY_H

class InputStream;
class OutputStream;

struct Identity
{
  Identity ()
  {
    name = "";
    category = "";
  }

  Identity (const Identity& id)
  {
    name = id.name;
    category = id.category;
  }

  Identity (const char* n, const char* c = "")
  {
    name = n; category = c;
  }

  Identity (const std::string& n, const std::string& c = "")
  {
    name = n; category = c;
  }

  void write(OutputStream* os) const;

  void read(InputStream* is);

  std::string name;
  std::string category;
};


#endif //ICM_IDENTITY_H
