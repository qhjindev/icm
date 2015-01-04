//---------------------------------------------------------- -*- Mode: C++ -*-
// \brief Properties file similar to java.util.Properties
//
//----------------------------------------------------------------------------

#ifndef ICC_PROPERTIES_H
#define ICC_PROPERTIES_H

#include <istream>
#include <string>
#include <map>
#include <stdint.h>


class Properties
{

private:
  //Map that holds the (key,value) pairs
  std::map<std::string, std::string> * propmap;
  std::string removeLTSpaces(std::string&);

public:
  Properties (const char* fileName,
              char delimiter = '=' , 
              bool verbose = true, 
              bool multiline = false);

  Properties();

  Properties(const Properties &p);

  ~Properties();

  // load the properties from a file
  int loadProperties(const char* fileName, 
                     char delimiter = '=' , 
                     bool verbose = true, 
                     bool multiline = false);

  // load the properties from an in-core buffer
  int loadProperties(std::istream &ist, 
                     char delimiter = '=', 
                     bool verbose = true, 
                     bool multiline = false);

  int saveProperties(const char* fileName);

  std::string getStr(const std::string& key, const std::string& def) const;

  void set(const std::string& key, const std::string& value);

  //const char* get(const char* key, const char* def) const;

  int getInt(const std::string& key, int def);
  void setInt(const std::string& key, int value);

  long getLong(const std::string& key, long def);
  void setLong(const std::string& key, long value);

  long long getLongLong(const std::string& key, long long def);

  uint64_t getInt64(const std::string& key, uint64_t def);

  double getDouble(const std::string& key, double def);

  float getFloat(const std::string& key, float def);

  bool getBool(const std::string& key, bool def);
  void setBool(const std::string& key, bool value);

  void getList(std::string &outBuf, const std::string& linePrefix) const;

};


#endif // ICC_PROPERTIES_H
