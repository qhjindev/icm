//---------------------------------------------------------- -*- Mode: C++ -*-
// \brief Properties implementation.
//----------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "properties.h"


Properties::Properties()
{
  propmap = new std::map<std::string, std::string>;
}

Properties::Properties(const Properties &p)
{
  propmap = new std::map<std::string, std::string>(*(p.propmap));
}

Properties::~Properties()
{
  delete propmap;
}

Properties::Properties(const char* fileName, char delimiter, bool verbose, bool multiline /*=false*/)
{
  propmap = new std::map<std::string, std::string>;
  loadProperties (fileName, delimiter, verbose, multiline);
}

int Properties::loadProperties(const char* fileName, char delimiter, bool verbose, bool multiline /*=false*/)
{
  std::ifstream input(fileName);
  std::string line;

  if (!input.is_open()) {
    std::cerr << "Properties::loadProperties() Could not open the file:" << fileName << std::endl;
    return (-1);
  }
  loadProperties(input, delimiter, verbose, multiline);
  input.close();
  return 0;
}

int Properties::saveProperties(const char *fileName)
{
  std::ofstream ofs(fileName);
  if (!ofs.is_open())
    return -1;

  for(std::map<std::string, std::string>::iterator it = this->propmap->begin(); it != propmap->end(); it++) {
    ofs << it->first << "=" << it->second << std::endl;
  }

  ofs.close();

  return 0;
}

int Properties::loadProperties(std::istream &ist, char delimiter, bool verbose, bool multiline /*=false*/)
{
  std::string line;

  while (ist) {
    getline(ist, line); //read one line at a time
    if (line.find('#') == 0)
      continue; //ignore comments
    std::string::size_type pos = line.find(delimiter); //find the delimiter

    if (pos == line.npos)
      continue; //ignore if no delimiter is found
    std::string key = line.substr(0, pos); // get the key
    key = removeLTSpaces(key);
    std::string value = line.substr(pos + 1); //get the value
    value = removeLTSpaces(value);

    if (multiline)
      (*propmap)[key] += value; // allow properties to be spread across multiple lines
    else
      (*propmap)[key] = value;

    if (verbose)
      std::cout << "Loading key " << key << " with value " << (*propmap)[key] << std::endl;
  }
  return 0;
}

void Properties::set(const std::string& key, const std::string& value)
{
  (*propmap)[key] = value;
  return;
}

std::string Properties::removeLTSpaces(std::string& str)
{

  char const* delims = " \t\r\n";

  // trim leading whitespace
  std::string::size_type notwhite = str.find_first_not_of(delims);
  str.erase(0, notwhite);

  // trim trailing whitespace
  notwhite = str.find_last_not_of(delims);
  str.erase(notwhite + 1);
  return (str);
}

std::string Properties::getStr(const std::string& key, const std::string& def) const
{
  if (propmap->find(key) == propmap->end())
    return def;
  return (*propmap)[key];
}
/*
const char* Properties::get(const char* key, const char* def) const
{
  if (propmap->find(key) == propmap->end())
    return def;
  return (((*propmap)[key]).c_str());
}
*/

int Properties::getInt(const std::string& key, int def)
{
  if (propmap->find(key) == propmap->end())
    return def;
  return (atoi(((*propmap)[key]).c_str()));
}

void Properties::setInt(const std::string& key, int value)
{
  std::ostringstream os;
  os << value;
  set(key, os.str());
}

long Properties::getLong(const std::string& key, long def)
{
  if (propmap->find(key) == propmap->end())
    return def;
  return (atol(((*propmap)[key]).c_str()));
}

void Properties::setLong(const std::string& key, long value)
{
  std::ostringstream os;
  os << value;
  set(key, os.str());
}

long long Properties::getLongLong(const std::string& key, long long def)
{
  if (propmap->find(key) == propmap->end())
    return def;
  return (atol(((*propmap)[key]).c_str()));
}

uint64_t Properties::getInt64(const std::string& key, uint64_t def)
{
  if (propmap->find(key) == propmap->end())
    return def;
  return (atol(((*propmap)[key]).c_str()));
}

double Properties::getDouble(const std::string& key, double def)
{
  if (propmap->find(key) == propmap->end())
    return def;
  return (atof(((*propmap)[key]).c_str()));
}

float Properties::getFloat(const std::string& key, float def)
{
  if (propmap->find(key) == propmap->end())
    return def;
  return ((float)atof(((*propmap)[key]).c_str()));
}

bool Properties::getBool(const std::string& key, bool def)
{
  std::map<std::string, std::string>::iterator it = propmap->find(key);
  if (it == propmap->end())
    return def;

  std::string value = it->second;
  if (value == "true")
    return true;
  else if(value == "false")
    return false;
  else 
    return def;
}

void Properties::setBool(const std::string& key, bool value)
{
  std::string v = value ? "true" : "false";
  set(key, v);
}

void Properties::getList(std::string &outBuf, const std::string& linePrefix) const
{
  std::map<std::string, std::string>::iterator iter;

  for (iter = propmap->begin(); iter != propmap->end(); iter++) {
    if ((*iter).first.size() > 0) {
      outBuf += linePrefix;
      outBuf += (*iter).first;
      outBuf += '=';
      outBuf += (*iter).second;
      outBuf += '\n';
    }
  }

  return;
}
