
#include <sstream>
#include "icc/Configuration.h"
#include "icc/Log.h"
#include "os/OS_Errno.h"
#include "os/OS_NS_stdio.h"
#include "os/OS_NS_string.h"

using namespace std;

int
Configuration::open (const char *fileName)
{
  if (0 == fileName)  {
      errno = EINVAL;
      return -1;
  }
  FILE* in = OS::fopen (fileName, ACE_TEXT ("r"));
  if (!in)
    return -1;

  char buffer[4096];
  SectionMap *section = 0;
  while (OS::fgets (buffer, sizeof buffer, in))
    {
      char *line = this->squish (buffer);
      // Check for a comment and blank line
      if (line[0] ==  ';'  ||
          line[0] ==  '#'  ||
          line[0] == '\0')
        continue;

      if (line[0] == '[')  {
          // We have a new section here, strip out the section name
          char* end = OS::strrchr (line, ']');
          if (!end) {
              OS::fclose (in);
              return -3;
          }

          // edited by robert, change to memmove to handle overlapping memory
//          OS::memcpy (line, line + 1, OS::strlen (line) - 2);
          OS::memmove (line, line + 1, OS::strlen (line) - 2);
          line[OS::strlen (line) -2] = 0;
          //*end = 0;

          section = new SectionMap;
          this->m_configMap.insert (pair<string, SectionMap*> (line,section));

          continue;
      }

      // We have a line; name ends at equal sign.
      char *end = OS::strchr (line, '=');
      if (end == 0)   {
          OS::fclose (in);
          return -3;
      }
      *end++ = '\0';
      char *name = this->squish (line);

      // Now find the start of the value
      char *value = this->squish (end);
      //size_t value_len = OS::strlen (value);

      section->insert (pair<string, string> (name, value));

  } // end while fgets

  if (ferror (in))  {
      OS::fclose (in);
      return -1;
  }

  OS::fclose (in);
  return 0;
}

int
Configuration::close ()
{
  //erase all the element in the map.

  return 0;
}

char *
Configuration::squish (char *src)
{
  char *cp = 0;

  if (src == 0)
    return 0;

  // Start at the end and work backwards over all whitespace.
  for (cp = src + OS::strlen (src) - 1; cp != src; --cp)
    if (!isspace (*cp))
      break;

  cp[1] = '\0';          // Chop trailing whitespace

  // Now start at the beginning and move over all whitespace.
  for (cp = src; isspace (*cp); ++cp)
    continue;

  return cp;
}

std::string
Configuration::getConfigStr (const char *sectionKey, const char * name)
{
  if ((sectionKey == 0) || (name == 0))
    return "";

  map<string, SectionMap*>::iterator it = this->m_configMap.find (sectionKey);
  if (it != this->m_configMap.end ()) {
    SectionMap* sectionMap = it->second;
    if (sectionMap == 0) {
      LogMsg << "err find the section " << sectionKey << endl;
      return "";
    }

    map<string, string>::iterator it2 = sectionMap->find (name);
    if (it2 != sectionMap->end ()) {
      return it2->second;
    } else {
      return "";
    }
  } else {
    return "";
  }
}

short
Configuration::getConfigShort(const char* sectionKey, const char* name) {
  string str = getConfigStr(sectionKey, name);
  if(str.empty())
    return -1;

  istringstream iss(str);
  short tmp;
  iss >> tmp;
  return tmp;
}

int
Configuration::getConfigInt(const char* sectionKey, const char* name) {
  string str = getConfigStr(sectionKey, name);
  if(str.empty())
    return -1;

  istringstream iss(str);
  int tmp;
  iss >> tmp;
  return tmp;
}

bool
Configuration::getConfigBool(const char* sectionKey, const char* name) {
  string str = getConfigStr(sectionKey, name);
  if(str.empty())
    return -1;

  istringstream iss(str);
  bool tmp;
  iss >> tmp;
  return tmp;
}
