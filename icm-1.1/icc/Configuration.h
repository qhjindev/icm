#ifndef ICC_CONFIGURATION_H
#define ICC_CONFIGURATION_H

#include <map>
#include <string>

class Configuration
{
  typedef std::map<std::string, std::string> SectionMap;
  typedef std::map<std::string, SectionMap*> ConfigMap;

public:
  int open (const char *fileName);
  int close ();

  std::string getConfigStr (const char *sectionKey, const char * name);
  short getConfigShort(const char* sectionKey, const char* name);
  int getConfigInt(const char* sectionKey, const char* name);
  bool getConfigBool(const char* sectionKey, const char* name);

protected:
  ConfigMap m_configMap;

  char *squish (char *);
};

#endif //ICC_CONFIGURATION_H
