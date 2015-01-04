// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Slice/CPlusPlusUtil.h>

using namespace std;
using namespace Slice;
using namespace IceUtil;

char Slice::ToIfdef::operator()(char c)
{
  if(!isalnum(c))
  {
    return '_';
  }
  else
  {
    return c;
  }
}

string Slice::normalizePath(const string& path)
{
  string result = path;
  replace(result.begin(), result.end(), '\\', '/');
  string::size_type pos;
  while ((pos = result.find("//")) != string::npos) {
    result.replace(pos, 2, "/");
  }
  if (result.size() > 1 && isalpha(result[0]) && result[1] == ':') {
    result = result.substr(2);
  }
  return result;
}

string Slice::changeInclude(const string& orig, const vector<string>& includePaths)
{
  string file = normalizePath(orig);
  string::size_type pos;

  for (vector<string>::const_iterator p = includePaths.begin(); p != includePaths.end(); ++p) {
    string includePath = normalizePath(*p);

    if (file.compare(0, includePath.length(), includePath) == 0) {
      string s = file.substr(includePath.length());
      if (s.size() < file.size()) {
        file = s;
      }
    }
  }

  if ((pos = file.rfind('.')) != string::npos) {
    file.erase(pos);
  }

  return file;
}

void Slice::printHeader(Output& out)
{
  static const char* header =
      "// **********************************************************************\n"
        "//\n"
        "// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.\n"
        "//\n"
        "// This copy of Ice is licensed to you under the terms described in the\n"
        "// ICE_LICENSE file included in this distribution.\n"
        "//\n"
        "// **********************************************************************\n";

  out << header;
  out << "\n// Ice version " << ICE_STRING_VERSION;
}

void Slice::printVersionCheck(Output& out)
{
  out << "\n";
  out << "\n#ifndef ICE_IGNORE_VERSION";
  out << "\n#   if ICE_INT_VERSION / 100 != " << ICE_INT_VERSION / 100;
  out << "\n#       error Ice version mismatch!";
  out << "\n#   endif";
  out << "\n#   if ICE_INT_VERSION % 100 < " << ICE_INT_VERSION % 100;
  out << "\n#       error Ice patch level mismatch!";
  out << "\n#   endif";
  out << "\n#endif";
}

void Slice::printDllExportStuff(Output& out, const string& dllExport)
{
  if (dllExport.size()) {
    out << sp;
    out << "\n#ifndef " << dllExport;
    out << "\n#   ifdef " << dllExport << "_EXPORTS";
    out << "\n#       define " << dllExport << " ICE_DECLSPEC_EXPORT";
    out << "\n#   else";
    out << "\n#       define " << dllExport << " ICE_DECLSPEC_IMPORT";
    out << "\n#   endif";
    out << "\n#endif";
  }
}

string Slice::typeToString(const TypePtr& type, const StringList& metaData, bool inParam)
{
  static const char* builtinTable[] = { "Byte", "bool", "Short", "Int",
      "Long", "Float", "Double", "::std::string", "::Ice::ObjectPtr",
      "::IcmProxy::Object*", "::Ice::LocalObjectPtr" };

  BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
  if (builtin) {
    return builtinTable[builtin->kind()];
  }

  ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
  if (cl) {
    //return fixKwd(cl->scoped() + "Ptr"); by qinghua
    return fixKwd(cl->scoped());
  }

  ProxyPtr proxy = ProxyPtr::dynamicCast(type);
  if (proxy) {
    //return fixKwd(proxy->_class()->scoped() + "Prx");
    return "::IcmProxy" + fixKwd(proxy->_class()->scoped() + "*");
  }

  SequencePtr seq = SequencePtr::dynamicCast(type);
  if (seq) {
    string seqType = findMetaData(metaData, true);
    if (!seqType.empty()) {
      if (seqType == "array" || seqType == "range:array") {
        if (inParam) {
          TypePtr elemType = seq->type();
          string s = typeToString(elemType);
          return "::std::pair<const " + s + "*, const " + s + "*>";
        } else {
          return fixKwd(seq->scoped());
        }
      } else if (seqType.find("range") == 0) {
        if (inParam) {
          string s;
          if (seqType.find("range:") == 0) {
            s = seqType.substr(strlen("range:"));
          } else {
            s = fixKwd(seq->scoped());
          }
          if (s[0] == ':') {
            s = " " + s;
          }
          return "::std::pair<" + s + "::const_iterator, " + s + "::const_iterator>";
        } else {
          return fixKwd(seq->scoped());
        }
      } else {
        return seqType;
      }
    } else {
      return fixKwd(seq->scoped());
    }
  }

  ContainedPtr contained = ContainedPtr::dynamicCast(type);
  if (contained) {
    return fixKwd(contained->scoped());
  }

  EnumPtr en = EnumPtr::dynamicCast(type);
  if (en) {
    return fixKwd(en->scoped());
  }

  return "???";
}

string Slice::returnTypeToString(const TypePtr& type, const StringList& metaData)
{
  if (!type) {
    return "void";
  }

  return typeToString(type, metaData, false);
}

string Slice::inputTypeToString(const TypePtr& type, bool useWstring, const StringList& metaData)
{
  static const char* inputBuiltinTable[] = { "Byte", "bool", "Short", "Int",
      "Long", "Float", "Double", "const ::std::string&",
      "const ObjectPtr&", "IcmProxy::Object*", "const LocalObjectPtr&" };

  BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
  if(builtin)
  {
    if(builtin->kind() == Builtin::KindString)
    {
      string strType = findMetaData(metaData, true);
      if(strType != "string" && (useWstring || strType == "wstring"))
      {
          return "const ::std::wstring&";
      }
    }
    return inputBuiltinTable[builtin->kind()];
  }

  ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
  if (cl) {
    //return "const " + fixKwd(cl->scoped() + "Ptr") + "&"; by qinghua
    //return "const " + fixKwd(cl->scoped()) + "&"; by qinghua
    return fixKwd(cl->scoped()) + "&";
  }

  ProxyPtr proxy = ProxyPtr::dynamicCast(type);
  if (proxy) {
    //return "const " + fixKwd(proxy->_class()->scoped() + "Prx") + "&";
    return "IcmProxy" + proxy->_class()->scoped() + " *";
  }

  EnumPtr en = EnumPtr::dynamicCast(type);
  if (en) {
    return fixKwd(en->scoped());
  }

  SequencePtr seq = SequencePtr::dynamicCast(type);
  if (seq) {
    string seqType = findMetaData(metaData, true);
    if (!seqType.empty()) {
      if (seqType == "array" || seqType == "range:array") {
        TypePtr elemType = seq->type();
        string s = typeToString(elemType);
        return "const ::std::pair<const " + s + "*, const " + s + "*>&";
      } else if (seqType.find("range") == 0) {
        string s;
        if (seqType.find("range:") == 0) {
          s = seqType.substr(strlen("range:"));
        } else {
          s = fixKwd(seq->scoped());
        }
        if (s[0] == ':') {
          s = " " + s;
        }
        return "const ::std::pair<" + s + "::const_iterator, " + s + "::const_iterator>&";
      } else {
        return "const " + seqType + "&";
      }
    } else {
      return "const " + fixKwd(seq->scoped()) + "&";
    }
  }

  ContainedPtr contained = ContainedPtr::dynamicCast(type);
  if (contained) {
    return "const " + fixKwd(contained->scoped()) + "&";
  }

  return "???";
}

string Slice::outputTypeToString(const TypePtr& type, bool useWstring,const StringList& metaData)
{
  static const char* outputBuiltinTable[] = { "Byte&", "bool&", "Short&",
      "Int&", "Long&", "Float&", "Double&", "::std::string&",
      "ObjectPtr&", "Object*", "LocalObjectPtr&" };

  BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
  if(builtin)
  {
    if(builtin->kind() == Builtin::KindString)
    {
      string strType = findMetaData(metaData, true);
      if(strType != "string" && (useWstring || strType == "wstring"))
      {
        return "::std::wstring&";
      }
    }
    return outputBuiltinTable[builtin->kind()];
  }

  ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
  if (cl) {
    //return fixKwd(cl->scoped() + "Ptr") + "&"; by qinghua
    return fixKwd(cl->scoped()) + "&";
  }

  ProxyPtr proxy = ProxyPtr::dynamicCast(type);
  if (proxy) {
    return fixKwd(proxy->_class()->scoped() + "Prx") + "&";
  }

  SequencePtr seq = SequencePtr::dynamicCast(type);
  if (seq) {
    string seqType = findMetaData(metaData, false);
    if (!seqType.empty()) {
      return seqType + "&";
    } else {
      return fixKwd(seq->scoped()) + "&";
    }
  }
  ContainedPtr contained = ContainedPtr::dynamicCast(type);
  if (contained) {
    return fixKwd(contained->scoped()) + "&";
  }

  return "???";
}

string Slice::operationModeToString(Operation::Mode mode)
{
  switch (mode)
  {
  case Operation::Normal: {
    return "::Ice::Normal";
  }

  case Operation::Nonmutating: {
    return "::Ice::Nonmutating";
  }

  case Operation::Idempotent: {
    return "::Ice::Idempotent";
  }

  default: {
    assert(false);
  }
  }

  return "???";
}

//
// If the passed name is a keyword, return the name with a "_cxx_" prefix;
// otherwise, return the name unchanged.
//

static string lookupKwd(const string& name)
{
  //
  // Keyword list. *Must* be kept in alphabetical order.
  //
  static const string keywordList[] = { "and", "and_eq", "asm", "auto", "bit_and", "bit_or",
      "bool", "break", "case", "catch", "char", "class", "compl", "const", "const_cast",
      "continue", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit",
      "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", "long",
      "mutable", "namespace", "new", "not", "not_eq", "operator", "or", "or_eq", "private",
      "protected", "public", "register", "reinterpret_cast", "return", "short", "signed", "sizeof",
      "static", "static_cast", "struct", "switch", "template", "this", "throw", "true", "try",
      "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile",
      "wchar_t", "while", "xor", "xor_eq" };
  bool found = binary_search(&keywordList[0], &keywordList[sizeof(keywordList)
      / sizeof(*keywordList)], name);
  return found ? "_cpp_" + name : name;
}

//
// Split a scoped name into its components and return the components as a list of (unscoped) identifiers.
//
static StringList splitScopedName(const string& scoped)
{
  assert(scoped[0] == ':');
  StringList ids;
  string::size_type next = 0;
  string::size_type pos;
  while ((pos = scoped.find("::", next)) != string::npos) {
    pos += 2;
    if (pos != scoped.size()) {
      string::size_type endpos = scoped.find("::", pos);
      if (endpos != string::npos) {
        ids.push_back(scoped.substr(pos, endpos - pos));
      }
    }
    next = pos;
  }
  if (next != scoped.size()) {
    ids.push_back(scoped.substr(next));
  } else {
    ids.push_back("");
  }

  return ids;
}

//
// If the passed name is a scoped name, return the identical scoped name,
// but with all components that are C++ keywords replaced by
// their "_cxx_"-prefixed version; otherwise, if the passed name is
// not scoped, but a C++ keyword, return the "_cxx_"-prefixed name;
// otherwise, return the name unchanged.
//
string Slice::fixKwd(const string& name)
{
  if (name[0] != ':') {
    return lookupKwd(name);
  }
  StringList ids = splitScopedName(name);
  transform(ids.begin(), ids.end(), ids.begin(), ptr_fun(lookupKwd));
  stringstream result;
  for (StringList::const_iterator i = ids.begin(); i != ids.end(); ++i) {
    result << "::" + *i;
  }
  return result.str();
}

void Slice::writeMarshalUnmarshalCode(Output& out,
                                      const TypePtr& type,
                                      const string& param,
                                      bool marshal,
                                      const string& str,
                                      bool pointer,
                                      const StringList& metaData,
                                      bool inParam)
{
  string fixedParam = fixKwd(param);

  string stream;
  if (str.empty()) {
    stream = marshal ? "__os" : "__is";
  } else {
    stream = str;
  }

  string deref;
  if (pointer) {
    deref = "->";
  } else {
    deref = '.';
  }

  string func = marshal ? "write(" : "read(";

  BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
  if (builtin) {
    if (builtin->kind() == Builtin::KindObject) {
      if (marshal) {
        out << nl << stream << deref << func << fixedParam << ");";
      } else {
        out << nl << stream << deref << func << "::Ice::__patch__ObjectPtr, &" << fixedParam
            << ");";
      }
      return;
    } else {
      out << nl << stream << deref << func << fixedParam << ");";
      return;
    }
  }

  ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
  if (cl) {
    string scope = fixKwd(cl->scope());
    /* by qinghua
    if (marshal) {
      out << nl << scope << "__write(" << (pointer ? "" : "&") << stream << ", " << fixedParam
          << ");";
    } else {
      out << nl << stream << deref << "read(" << scope << "__patch__" << cl->name() << "Ptr, &"
          << fixedParam << ");";
    }
*/
    out << nl << fixedParam << ".__" << func << (pointer ? "" : "&") << stream << ");";
    return;
  }

  StructPtr st = StructPtr::dynamicCast(type);
  if (st) {
    out << nl << fixedParam << ".__" << func << (pointer ? "" : "&") << stream << ");";
    return;
  }

  ListPtr list = ListPtr::dynamicCast(type);
  if (list) {
    assert(false);
    string listType = findMetaData(metaData, inParam);
    builtin = BuiltinPtr::dynamicCast(list->type());
    if (marshal) {
      string scope = fixKwd(list->scope());
      if (listType == "array" || listType == "range:array") {
        //
        // Use array (pair<const TYPE*, const TYPE*>). In paramters only.
        //
        if (!builtin || builtin->kind() == Builtin::KindObject || builtin->kind()
            == Builtin::KindObjectProxy) {
          //
          // Sequence type in not handled by BasicStream functions. If the sequence is the
          // default vector than we can use the sequences generated write function. Otherwise
          // we need to generate marshal code to write each element.
          //
          StringList l = list->getMetaData();
          listType = findMetaData(l, false);
          if (listType.empty()) {
            out << nl << scope << "__" << func << (pointer ? "" : "&") << stream << ", "
                << fixedParam << ".first, " << fixedParam << ".second, " << scope << "__U__"
                << fixKwd(list->name()) << "());";
          } else {
            out << nl << "::Ice::Int __sz_" << fixedParam << " = static_cast< ::Ice::Int>("
                << fixedParam << ".second - " << fixedParam << ".first);";
            out << nl << stream << deref << "writeSize(__sz_" << fixedParam << ");";
            out << nl << "for(int __i_" << fixedParam << " = 0; __i_" << fixedParam << " < __sz_"
                << fixedParam << "; ++__i_" << fixedParam << ")";
            out << sb;
            writeMarshalUnmarshalCode(out, list->type(), fixedParam + ".first[__i_" + fixedParam
                + "]", true);
            out << eb;
          }
        } else {
          //
          // Use BasicStream write functions.
          //
          out << nl << stream << deref << func << fixedParam << ".first, " << fixedParam
              << ".second);";
        }
      } else if (listType.find("range") == 0) {
        //
        // Use range (pair<TYPE::const_iterator, TYPE::const_iterator). Only for in paramaters.
        // Need to check if the range defines an iterator type other than the actual sequence
        // type.
        //
        StringList l;
        if (listType.find("range:") == 0) {
          listType = listType.substr(strlen("range:"));
          l.push_back("cpp:" + listType);
        } else {
          listType = fixKwd(list->scoped());
        }
        out << nl << stream << deref << "writeSize(static_cast< ::Ice::Int>(ice_distance("
            << fixedParam << ".first, " << fixedParam << ".second)));";
        out << nl << "for(" << listType << "::const_iterator ___" << fixedParam << " = "
            << fixedParam << ".first; ___" << fixedParam << " != " << fixedParam
            << ".second; ++___" << fixedParam << ")";
        out << sb;
        writeMarshalUnmarshalCode(out, list->type(), "(*___" + fixedParam + ")", true, "", true, l, false);
        out << eb;
      } else if (!listType.empty()) {
        out << nl << stream << deref << "writeSize(static_cast< ::Ice::Int>(" << fixedParam
            << ".size()));";
        out << nl << listType << "::const_iterator __p_" << fixedParam << ";";
        out << nl << "for(__p_" << fixedParam << " = " << fixedParam << ".begin(); __p_"
            << fixedParam << " != " << fixedParam << ".end(); ++__p_" << fixedParam << ")";
        out << sb;
        writeMarshalUnmarshalCode(out, list->type(), "(*__p_" + fixedParam + ")", true);
        out << eb;
      } else {
        //
        // No modifying metadata specified. Use appropriate write methods for type.
        //
        StringList l = list->getMetaData();
        listType = findMetaData(l, false);
        if (!listType.empty()) {
          out << nl << scope << "__" << func << (pointer ? "" : "&") << stream << ", "
              << fixedParam << ", " << scope << "__U__" << fixKwd(list->name()) << "());";
        } else if (!builtin || builtin->kind() == Builtin::KindObject || builtin->kind()
            == Builtin::KindObjectProxy) {
          out << nl << "if(" << fixedParam << ".size() == 0)";
          out << sb;
          out << nl << stream << deref << "writeSize(0);";
          out << eb;
          out << nl << "else";
          out << sb;
          out << nl << scope << "__" << func << (pointer ? "" : "&") << stream << ", &"
              << fixedParam << "[0], &" << fixedParam << "[0] + " << fixedParam << ".size(), "
              << scope << "__U__" << fixKwd(list->name()) << "());";
          out << eb;
        } else if (builtin->kind() == Builtin::KindBool) {
          out << nl << stream << deref << func << fixedParam << ");";
        } else {
          out << nl << "if(" << fixedParam << ".size() == 0)";
          out << sb;
          out << nl << stream << deref << "writeSize(0);";
          out << eb;
          out << nl << "else";
          out << sb;
          out << nl << stream << deref << func << "&" << fixedParam << "[0], &" << fixedParam
              << "[0] + " << fixedParam << ".size());";
          out << eb;
        }
      }
    } else {
      string scope = fixKwd(list->scope());
      if (listType == "array" || listType == "range:array") {
        //
        // Use array (pair<const TYPE*, const TYPE*>). In paramters only.
        //
        if (!builtin || builtin->kind() == Builtin::KindObject || builtin->kind()
            == Builtin::KindObjectProxy) {
          StringList l = list->getMetaData();
          listType = findMetaData(l, false);
          if (listType.empty()) {
            out << nl << typeToString(type) << " ___" << fixedParam << ";";
            out << nl << scope << "___" << func << (pointer ? "" : "&") << stream << ", ___"
                << fixedParam << ", " << scope << "__U__" << fixKwd(list->name()) << "());";
          } else {
            listType = "::std::vector< " + typeToString(list->type()) + ">";
            StringList l;
            l.push_back("cpp:type:" + listType);
            out << nl << listType << " ___" << fixedParam << ";";
            writeMarshalUnmarshalCode(out, list, "___" + fixedParam, false, "", true, l, false);
          }
        } else if (builtin->kind() == Builtin::KindByte) {
          out << nl << stream << deref << func << fixedParam << ");";
        } else if (builtin->kind() != Builtin::KindString && builtin->kind() != Builtin::KindObject
            && builtin->kind() != Builtin::KindObjectProxy) {
          string s = typeToString(builtin);
          if (s[0] == ':') {
            s = " " + s;
          }
          out << nl << "::IceUtil::auto_array<" << s << "> ___" << fixedParam << ";";
          out << nl << stream << deref << func << fixedParam << ", ___" << fixedParam << ");";
        } else {
          out << nl << "::std::vector< " << typeToString(list->type()) << "> ___" << fixedParam
              << ";";
          out << nl << stream << deref << func << "___" << fixedParam << ");";
        }

        if (!builtin || builtin->kind() == Builtin::KindString || builtin->kind()
            == Builtin::KindObject || builtin->kind() == Builtin::KindObjectProxy) {
          out << nl << fixedParam << ".first" << " = &___" << fixedParam << "[0];";
          out << nl << fixedParam << ".second" << " = " << fixedParam << ".first + " << "___"
              << fixedParam << ".size();";
        }
      } else if (listType.find("range") == 0) {
        //
        // Use range (pair<TYPE::const_iterator, TYPE::const_iterator>). In paramters only.
        // Need to check if iterator type other than default is specified.
        //
        StringList l;
        if (listType.find("range:") == 0) {
          l.push_back("cpp:type:" + listType.substr(strlen("range:")));
        }
        out << nl << typeToString(list, l, false) << " ___" << fixedParam << ";";
        writeMarshalUnmarshalCode(out, list, "___" + fixedParam, false, "", true, l, false);
        out << nl << fixedParam << ".first = ___" << fixedParam << ".begin();";
        out << nl << fixedParam << ".second = ___" << fixedParam << ".end();";
      } else if (!listType.empty()) {
        //
        // Using alternate sequence type.
        //
        out << nl << "::Ice::Int __sz_" << fixedParam << ";";
        out << nl << stream << deref << "readSize(__sz_" << fixedParam << ");";
        out << nl << listType << "(__sz_" << fixedParam << ").swap(" << fixedParam << ");";
        if (list->type()->isVariableLength()) {
          out << nl << stream << deref << "startSeq(__sz_" << fixedParam << ", "
              << list->type()->minWireSize() << ");";
        } else {
          out << nl << stream << deref << "checkFixedSeq(__sz_" << fixedParam << ", "
              << list->type()->minWireSize() << ");";
        }
        out << nl << listType << "::iterator __p_" << fixedParam << ";";
        out << nl << "for(__p_" << fixedParam << " = " << fixedParam << ".begin(); __p_"
            << fixedParam << " != " << fixedParam << ".end(); ++__p_" << fixedParam << ")";
        out << sb;
        writeMarshalUnmarshalCode(out, list->type(), "(*__p_" + fixedParam + ")", false);
        if (list->type()->isVariableLength()) {
          if (!SequencePtr::dynamicCast(list->type())) {
            out << nl << stream << deref << "checkSeq();";
          }
          out << nl << stream << deref << "endElement();";
        }
        out << eb;
        if (list->type()->isVariableLength()) {
          out << nl << stream << deref << "endSeq(__sz_" << fixedParam << ");";
        }
      } else {
        //
        // No modifying metadata supplied. Just use appropriate read function.
        //
        StringList l = list->getMetaData();
        listType = findMetaData(l, false);
        if (!listType.empty() || !builtin || builtin->kind() == Builtin::KindObject
            || builtin->kind() == Builtin::KindObjectProxy) {
          out << nl << scope << "__" << func << (pointer ? "" : "&") << stream << ", "
              << fixedParam << ", " << scope << "__U__" << fixKwd(list->name()) << "());";
        } else if (builtin->kind() == Builtin::KindByte) {
          StringList md;
          md.push_back("cpp:array");
          string tmpParam = "___";
          if (fixedParam.find("(*") == 0) {
            tmpParam += fixedParam.substr(2, fixedParam.length() - 3);
          } else if (fixedParam.find("[i]") != string::npos) {
            tmpParam += fixedParam.substr(0, fixedParam.length() - 3);
          } else {
            tmpParam += fixedParam;
          }
          out << nl << typeToString(type, md) << " " << tmpParam << ";";
          out << nl << stream << deref << func << tmpParam << ");";
          out << nl << "::std::vector< ::Ice::Byte>(" << tmpParam << ".first, " << tmpParam
              << ".second).swap(" << fixedParam << ");";
        } else {
          out << nl << stream << deref << func << fixedParam << ");";
        }
      }
    }
    return;
  }

  SequencePtr seq = SequencePtr::dynamicCast(type);
  if (seq) {
    string seqType = findMetaData(metaData, inParam);
    builtin = BuiltinPtr::dynamicCast(seq->type());
    if (marshal) {
      string scope = fixKwd(seq->scope());
      if (seqType == "array" || seqType == "range:array") {
        //
        // Use array (pair<const TYPE*, const TYPE*>). In paramters only.
        //
        if (!builtin || builtin->kind() == Builtin::KindObject || builtin->kind()
            == Builtin::KindObjectProxy) {
          //
          // Sequence type in not handled by BasicStream functions. If the sequence is the
          // default vector than we can use the sequences generated write function. Otherwise
          // we need to generate marshal code to write each element.
          //
          StringList l = seq->getMetaData();
          seqType = findMetaData(l, false);
          if (seqType.empty()) {
            out << nl << scope << "__" << func << (pointer ? "" : "&") << stream << ", "
                << fixedParam << ".first, " << fixedParam << ".second, " << scope << "__U__"
                << fixKwd(seq->name()) << "());";
          } else {
            out << nl << "::Ice::Int __sz_" << fixedParam << " = static_cast< ::Ice::Int>("
                << fixedParam << ".second - " << fixedParam << ".first);";
            out << nl << stream << deref << "writeSize(__sz_" << fixedParam << ");";
            out << nl << "for(int __i_" << fixedParam << " = 0; __i_" << fixedParam << " < __sz_"
                << fixedParam << "; ++__i_" << fixedParam << ")";
            out << sb;
            writeMarshalUnmarshalCode(out, seq->type(), fixedParam + ".first[__i_" + fixedParam
                + "]", true);
            out << eb;
          }
        } else {
          //
          // Use BasicStream write functions.
          //
          out << nl << stream << deref << func << fixedParam << ".first, " << fixedParam
              << ".second);";
        }
      } else if (seqType.find("range") == 0) {
        //
        // Use range (pair<TYPE::const_iterator, TYPE::const_iterator). Only for in paramaters.
        // Need to check if the range defines an iterator type other than the actual sequence
        // type.
        //
        StringList l;
        if (seqType.find("range:") == 0) {
          seqType = seqType.substr(strlen("range:"));
          l.push_back("cpp:" + seqType);
        } else {
          seqType = fixKwd(seq->scoped());
        }
        out << nl << stream << deref << "writeSize(static_cast< ::Ice::Int>(ice_distance("
            << fixedParam << ".first, " << fixedParam << ".second)));";
        out << nl << "for(" << seqType << "::const_iterator ___" << fixedParam << " = "
            << fixedParam << ".first; ___" << fixedParam << " != " << fixedParam
            << ".second; ++___" << fixedParam << ")";
        out << sb;
        writeMarshalUnmarshalCode(out, seq->type(), "(*___" + fixedParam + ")", true, "", true, l, false);
        out << eb;
      } else if (!seqType.empty()) {
        out << nl << stream << deref << "writeSize(static_cast< ::Ice::Int>(" << fixedParam
            << ".size()));";
        out << nl << seqType << "::const_iterator __p_" << fixedParam << ";";
        out << nl << "for(__p_" << fixedParam << " = " << fixedParam << ".begin(); __p_"
            << fixedParam << " != " << fixedParam << ".end(); ++__p_" << fixedParam << ")";
        out << sb;
        writeMarshalUnmarshalCode(out, seq->type(), "(*__p_" + fixedParam + ")", true);
        out << eb;
      } else {
        //
        // No modifying metadata specified. Use appropriate write methods for type.
        //
        StringList l = seq->getMetaData();
        seqType = findMetaData(l, false);
        if (!seqType.empty()) {
          out << nl << scope << "__" << func << (pointer ? "" : "&") << stream << ", "
              << fixedParam << ", " << scope << "__U__" << fixKwd(seq->name()) << "());";
        } else if (!builtin || builtin->kind() == Builtin::KindObject || builtin->kind()
            == Builtin::KindObjectProxy) {
          out << nl << "if(" << fixedParam << ".size() == 0)";
          out << sb;
          out << nl << stream << deref << "writeSize(0);";
          out << eb;
          out << nl << "else";
          out << sb;
          out << nl << scope << "__" << func << (pointer ? "" : "&") << stream << ", &"
              << fixedParam << "[0], &" << fixedParam << "[0] + " << fixedParam << ".size(), "
              << scope << "__U__" << fixKwd(seq->name()) << "());";
          out << eb;
        } else if (builtin->kind() == Builtin::KindBool) {
          out << nl << stream << deref << func << fixedParam << ");";
        } else {
          out << nl << "if(" << fixedParam << ".size() == 0)";
          out << sb;
          out << nl << stream << deref << "writeSize(0);";
          out << eb;
          out << nl << "else";
          out << sb;
          out << nl << stream << deref << func << "&" << fixedParam << "[0], &" << fixedParam
              << "[0] + " << fixedParam << ".size());";
          out << eb;
        }
      }
    } else {
      string scope = fixKwd(seq->scope());
      if (seqType == "array" || seqType == "range:array") {
        //
        // Use array (pair<const TYPE*, const TYPE*>). In paramters only.
        //
        if (!builtin || builtin->kind() == Builtin::KindObject || builtin->kind()
            == Builtin::KindObjectProxy) {
          StringList l = seq->getMetaData();
          seqType = findMetaData(l, false);
          if (seqType.empty()) {
            out << nl << typeToString(type) << " ___" << fixedParam << ";";
            out << nl << scope << "___" << func << (pointer ? "" : "&") << stream << ", ___"
                << fixedParam << ", " << scope << "__U__" << fixKwd(seq->name()) << "());";
          } else {
            seqType = "::std::vector< " + typeToString(seq->type()) + ">";
            StringList l;
            l.push_back("cpp:type:" + seqType);
            out << nl << seqType << " ___" << fixedParam << ";";
            writeMarshalUnmarshalCode(out, seq, "___" + fixedParam, false, "", true, l, false);
          }
        } else if (builtin->kind() == Builtin::KindByte) {
          out << nl << stream << deref << func << fixedParam << ");";
        } else if (builtin->kind() != Builtin::KindString && builtin->kind() != Builtin::KindObject
            && builtin->kind() != Builtin::KindObjectProxy) {
          string s = typeToString(builtin);
          if (s[0] == ':') {
            s = " " + s;
          }
          out << nl << "::IceUtil::auto_array<" << s << "> ___" << fixedParam << ";";
          out << nl << stream << deref << func << fixedParam << ", ___" << fixedParam << ");";
        } else {
          out << nl << "::std::vector< " << typeToString(seq->type()) << "> ___" << fixedParam
              << ";";
          out << nl << stream << deref << func << "___" << fixedParam << ");";
        }

        if (!builtin || builtin->kind() == Builtin::KindString || builtin->kind()
            == Builtin::KindObject || builtin->kind() == Builtin::KindObjectProxy) {
          out << nl << fixedParam << ".first" << " = &___" << fixedParam << "[0];";
          out << nl << fixedParam << ".second" << " = " << fixedParam << ".first + " << "___"
              << fixedParam << ".size();";
        }
      } else if (seqType.find("range") == 0) {
        //
        // Use range (pair<TYPE::const_iterator, TYPE::const_iterator>). In paramters only.
        // Need to check if iterator type other than default is specified.
        //
        StringList l;
        if (seqType.find("range:") == 0) {
          l.push_back("cpp:type:" + seqType.substr(strlen("range:")));
        }
        out << nl << typeToString(seq, l, false) << " ___" << fixedParam << ";";
        writeMarshalUnmarshalCode(out, seq, "___" + fixedParam, false, "", true, l, false);
        out << nl << fixedParam << ".first = ___" << fixedParam << ".begin();";
        out << nl << fixedParam << ".second = ___" << fixedParam << ".end();";
      } else if (!seqType.empty()) {
        //
        // Using alternate sequence type.
        //
        out << nl << "::Ice::Int __sz_" << fixedParam << ";";
        out << nl << stream << deref << "readSize(__sz_" << fixedParam << ");";
        out << nl << seqType << "(__sz_" << fixedParam << ").swap(" << fixedParam << ");";
        if (seq->type()->isVariableLength()) {
          out << nl << stream << deref << "startSeq(__sz_" << fixedParam << ", "
              << seq->type()->minWireSize() << ");";
        } else {
          out << nl << stream << deref << "checkFixedSeq(__sz_" << fixedParam << ", "
              << seq->type()->minWireSize() << ");";
        }
        out << nl << seqType << "::iterator __p_" << fixedParam << ";";
        out << nl << "for(__p_" << fixedParam << " = " << fixedParam << ".begin(); __p_"
            << fixedParam << " != " << fixedParam << ".end(); ++__p_" << fixedParam << ")";
        out << sb;
        writeMarshalUnmarshalCode(out, seq->type(), "(*__p_" + fixedParam + ")", false);
        if (seq->type()->isVariableLength()) {
          if (!SequencePtr::dynamicCast(seq->type())) {
            out << nl << stream << deref << "checkSeq();";
          }
          out << nl << stream << deref << "endElement();";
        }
        out << eb;
        if (seq->type()->isVariableLength()) {
          out << nl << stream << deref << "endSeq(__sz_" << fixedParam << ");";
        }
      } else {
        //
        // No modifying metadata supplied. Just use appropriate read function.
        //
        StringList l = seq->getMetaData();
        seqType = findMetaData(l, false);
        if (!seqType.empty() || !builtin || builtin->kind() == Builtin::KindObject
            || builtin->kind() == Builtin::KindObjectProxy) {
          out << nl << scope << "__" << func << (pointer ? "" : "&") << stream << ", "
              << fixedParam << ", " << scope << "__U__" << fixKwd(seq->name()) << "());";
        } else if (builtin->kind() == Builtin::KindByte) {
          StringList md;
          md.push_back("cpp:array");
          string tmpParam = "___";
          if (fixedParam.find("(*") == 0) {
            tmpParam += fixedParam.substr(2, fixedParam.length() - 3);
          } else if (fixedParam.find("[i]") != string::npos) {
            tmpParam += fixedParam.substr(0, fixedParam.length() - 3);
          } else {
            tmpParam += fixedParam;
          }
          out << nl << typeToString(type, md) << " " << tmpParam << ";";
          out << nl << stream << deref << func << tmpParam << ");";
          out << nl << "::std::vector< ::Ice::Byte>(" << tmpParam << ".first, " << tmpParam
              << ".second).swap(" << fixedParam << ");";
        } else {
          out << nl << stream << deref << func << fixedParam << ");";
        }
      }
    }
    return;
  }

  DictionaryPtr dict = DictionaryPtr::dynamicCast(type);
  if (dict) {
    string scope = fixKwd(dict->scope());
    out << nl << scope << "__" << func << (pointer ? "" : "&") << stream << ", " << fixedParam
        << ", " << scope << "__U__" << fixKwd(dict->name()) << "());";
    return;
  }

  ConstructedPtr constructed = ConstructedPtr::dynamicCast(type);
  if (!constructed) {
    ProxyPtr proxy = ProxyPtr::dynamicCast(type);
    assert(proxy);
    constructed = proxy->_class();
  }

  out << nl << fixKwd(constructed->scope()) << "__" << func << (pointer ? "" : "&") << stream
      << ", " << fixedParam << ");";
}

void Slice::writeMarshalCode(Output& out,
                             const ParamDeclList& params,
                             const TypePtr& ret,
                             const StringList& metaData,
                             const string& str,
                             bool inParam)
{
  for (ParamDeclList::const_iterator p = params.begin(); p != params.end(); ++p) {
    writeStreamMarshalUnmarshalCode(out, (*p)->type(), fixKwd((*p)->name()), true, str, (*p)->getMetaData() );
  }
  if (ret) {
    writeStreamMarshalUnmarshalCode(out, ret, "__ret", true, str, metaData);
  }
}

void Slice::writeUnmarshalCode(Output& out,
                               const ParamDeclList& params,
                               const TypePtr& ret,
                               const StringList& metaData,
                               const string& str,
                               bool inParam)
{
  for (ParamDeclList::const_iterator p = params.begin(); p != params.end(); ++p) {
    writeStreamMarshalUnmarshalCode(out, (*p)->type(), fixKwd((*p)->name()), false, str, (*p)->getMetaData());
  }
  if (ret) {
    writeStreamMarshalUnmarshalCode(out, ret, "__ret", false, str, metaData);
  }
}

void Slice::writeAllocateCode(Output& out,
                              const ParamDeclList& params,
                              const TypePtr& ret,
                              const StringList& metaData,
                              bool inParam)
{
  for (ParamDeclList::const_iterator p = params.begin(); p != params.end(); ++p) {
    out << nl << typeToString((*p)->type(), (*p)->getMetaData(), inParam) << ' '
        << fixKwd((*p)->name()) << ';';
  }
  if (ret) {
    out << nl << typeToString(ret, metaData, inParam) << " __ret;";
  }
}

void Slice::writeStreamMarshalUnmarshalCode(Output& out,
                                            const TypePtr& type,
                                            const string& param,
                                            bool marshal,
                                            const string& str,
                                            const StringList& metaData)
{
  string fixedParam = fixKwd(param);

  string stream;
  if (str.empty()) {
    stream = marshal ? "__os" : "__is";
  } else {
    stream = str;
  }

  BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
  if (builtin) {
    switch (builtin->kind())
    {
    case Builtin::KindByte: {
      if (marshal) {
        out << nl << stream << "->write_octet(" << fixedParam << ");";
      } else {
        //out << nl << fixedParam << " = " << stream << "->readByte();";
        out << nl << stream << "->read_octet(" << fixedParam << ");";
      }
      break;
    }
    case Builtin::KindBool: {
      if (marshal) {
        out << nl << stream << "->write_boolean((ACE_CDR::Boolean&)" << fixedParam << ");";
      } else {
        //out << nl << fixedParam << " = " << stream << "->readBool();";
        out << nl << stream << "->read_boolean((ACE_CDR::Boolean&)" << fixedParam << ");";
      }
      break;
    }
    case Builtin::KindShort: {
      if (marshal) {
        out << nl << stream << "->write_short(" << fixedParam << ");";
      } else {
        //out << nl << fixedParam << " = " << stream << "->readShort();";
        out << nl << stream << "->read_short(" << fixedParam << ");";
      }
      break;
    }
    case Builtin::KindInt: {
      if (marshal) {
        out << nl << stream << "->write_int(" << fixedParam << ");";
      } else {
        //out << nl << fixedParam << " = " << stream << "->readInt();";
        out << nl << stream << "->read_int(" << fixedParam << ");";
      }
      break;
    }
    case Builtin::KindLong: {
      if (marshal) {
        out << nl << stream << "->write_long(" << fixedParam << ");";
      } else {
        //out << nl << fixedParam << " = " << stream << "->readLong();";
        out << nl << stream << "->read_long(" << fixedParam << ");";
      }
      break;
    }
    case Builtin::KindFloat: {
      if (marshal) {
        out << nl << stream << "->write_float(" << fixedParam << ");";
      } else {
        //out << nl << fixedParam << " = " << stream << "->readFloat();";
        out << nl << stream << "->read_float(" << fixedParam << ");";
      }
      break;
    }
    case Builtin::KindDouble: {
      if (marshal) {
        out << nl << stream << "->write_double(" << fixedParam << ");";
      } else {
        //out << nl << fixedParam << " = " << stream << "->readDouble();";
        out << nl << stream << "->read_double(" << fixedParam << ");";
      }
      break;
    }
    case Builtin::KindString: {
      if (marshal) {
        out << nl << stream << "->write_string(" << fixedParam << ");";
      } else {
        //out << nl << fixedParam << " = " << stream << "->readString();";
        out << nl << stream << "->read_string(" << fixedParam << ");";
      }
      break;
    }
    case Builtin::KindObject: {
      if (marshal) {
        out << nl << "::Ice::ice_writeObject(" << stream << ", " << fixedParam << ");";
      } else {
        //out << nl << "::Ice::ice_readObject(" << stream << ", " << fixedParam << ");";
        out << nl << "::Ice::ice_readObject(" << stream << ", " << fixedParam << ");";
      }
      break;
    }
    case Builtin::KindObjectProxy: {
      // TODO
      if (marshal) {
        out << nl << stream << "->write_obj(" << fixedParam << ");";
      } else {
        //out << nl << fixedParam << " = " << stream << "->readProxy();";
        out << nl << stream << "->read_obj(" << fixedParam << ");";
      }
      break;
    }
    case Builtin::KindLocalObject: {
      assert(false);
      break;
    }
    }

    return;
  }

  ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
  if (cl) {
    string scope = fixKwd(cl->scope());
    if (marshal) {
      //out << nl << scope << "ice_write" << cl->name() << "(" << stream << ", " << fixedParam << ");";
      out << nl << fixedParam << "->__" << "write(" << stream << ");";
    } else {
      //out << nl << scope << "ice_read" << cl->name() << "(" << stream << ", " << fixedParam << ");";
      out << nl << fixedParam << "->__" << "read(" << stream << ");";
    }

    return;
  }

  StructPtr st = StructPtr::dynamicCast(type);
  if (st) {
    string scope = fixKwd(st->scope());
    if (marshal) {
      /*
      out << nl << scope << "ice_write" << st->name() << "(" << stream << ", " << fixedParam
          << ");";
      */
      out << nl << fixedParam << ".__" << "write(" << stream << ");";
    } else {
      //out << nl << scope << "ice_read" << st->name() << "(" << stream << ", " << fixedParam << ");";
      out << nl << fixedParam << ".__" << "read(" << stream << ");";
    }

    return;
  }

  ListPtr list = ListPtr::dynamicCast(type);
  if (list) {
    string listType = findMetaData(metaData, false);
    if (!listType.empty()) {
      if (marshal) {
        out << nl << stream << "->writeSize(static_cast< ::Ice::Int>(" << fixedParam
            << ".size()));";
        out << nl << listType << "::const_iterator ___" << fixedParam << ";";
        out << nl << "for(___" << fixedParam << " = " << fixedParam << ".begin(); ___"
            << fixedParam << " != " << fixedParam << ".end(); ++___" << fixedParam << ")";
        out << sb;
        writeStreamMarshalUnmarshalCode(out, list->type(), "(*___" + fixedParam + ")", true);
        out << eb;
      } else {
        out << nl << listType << "(static_cast< ::Ice::Int>(" << stream << "->readSize())).swap("
            << fixedParam << ");";
        out << nl << listType << "::iterator ___" << fixedParam << ";";
        out << nl << "for(___" << fixedParam << " = " << fixedParam << ".begin(); ___"
            << fixedParam << " != " << fixedParam << ".end(); ++___" << fixedParam << ")";
        out << sb;
        writeStreamMarshalUnmarshalCode(out, list->type(), "(*___" + fixedParam + ")", false);
        out << eb;
      }
    } else {
      listType = findMetaData(list->getMetaData(), false);
      builtin = BuiltinPtr::dynamicCast(list->type());
      if (!listType.empty() || !builtin || (builtin->kind() == Builtin::KindObject
          || builtin->kind() == Builtin::KindObjectProxy))
      {
        string scope = fixKwd(list->scope());
        if(marshal)
        {
          out << nl << "if(" << fixedParam << ".size() == 0)";
          out << sb;
          out << nl << stream << "->" << "write_uint(0);";
          out << eb;
          out << nl << "else";
          out << sb;
//          out << nl << scope << "__" << "write(" << stream << ", &"
//              << fixedParam << "[0], &" << fixedParam << "[0] + " << fixedParam << ".size(), "
//              << scope << "__U__" << fixKwd(list->name()) << "());";
          out << nl << scope << "__" << "write(" << stream << ", "
              << fixedParam << ", " << scope << "__U__" << fixKwd(list->name()) << "());";
          out << eb;
        }
        else
        {
          out << nl << scope << "__" << "read(" << stream << ", "
              << fixedParam << ", " << scope << "__U__" << fixKwd(list->name()) << "());";
        }
      } else {
        switch (builtin->kind())
        {
        case Builtin::KindByte: {
          if (marshal) {
            out << nl << stream << "->write_byte_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_byte_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindBool: {
          if (marshal) {
            out << nl << stream << "->write_bool_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_bool_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindShort: {
          if (marshal) {
            out << nl << stream << "->write_short_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_short_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindInt: {
          if (marshal) {
            out << nl << stream << "->write_int_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_int_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindLong: {
          if (marshal) {
            out << nl << stream << "->write_long_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_long_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindFloat: {
          if (marshal) {
            out << nl << stream << "->write_float_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_float_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindDouble: {
          if (marshal) {
            out << nl << stream << "->write_double_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_double_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindString: {
          if (marshal) {
            out << nl << stream << "->write_string_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_string_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindObject:
        case Builtin::KindObjectProxy:
        case Builtin::KindLocalObject: {
          assert(false);
          break;
        }
        }
      }
    }

    return;
  }

  SequencePtr seq = SequencePtr::dynamicCast(type);
  if (seq) {
    string seqType = findMetaData(metaData, false);
    if (!seqType.empty()) {
      if (marshal) {
        out << nl << stream << "->writeSize(static_cast< ::Ice::Int>(" << fixedParam
            << ".size()));";
        out << nl << seqType << "::const_iterator ___" << fixedParam << ";";
        out << nl << "for(___" << fixedParam << " = " << fixedParam << ".begin(); ___"
            << fixedParam << " != " << fixedParam << ".end(); ++___" << fixedParam << ")";
        out << sb;
        writeStreamMarshalUnmarshalCode(out, seq->type(), "(*___" + fixedParam + ")", true);
        out << eb;
      } else {
        out << nl << seqType << "(static_cast< ::Ice::Int>(" << stream << "->readSize())).swap("
            << fixedParam << ");";
        out << nl << seqType << "::iterator ___" << fixedParam << ";";
        out << nl << "for(___" << fixedParam << " = " << fixedParam << ".begin(); ___"
            << fixedParam << " != " << fixedParam << ".end(); ++___" << fixedParam << ")";
        out << sb;
        writeStreamMarshalUnmarshalCode(out, seq->type(), "(*___" + fixedParam + ")", false);
        out << eb;
      }
    } else {
      seqType = findMetaData(seq->getMetaData(), false);
      builtin = BuiltinPtr::dynamicCast(seq->type());
      if (!seqType.empty() || !builtin || (builtin->kind() == Builtin::KindObject
          || builtin->kind() == Builtin::KindObjectProxy))
      {
        string scope = fixKwd(seq->scope());
        if(marshal)
        {
          out << nl << "if(" << fixedParam << ".size() == 0)";
          out << sb;
          out << nl << stream << "->" << "write_uint(0);";
          out << eb;
          out << nl << "else";
          out << sb;
          out << nl << scope << "__" << "write(" << stream << ", &"
              << fixedParam << "[0], &" << fixedParam << "[0] + " << fixedParam << ".size(), "
              << scope << "__U__" << fixKwd(seq->name()) << "());";
          out << eb;
        }
        else
        {
          out << nl << scope << "__" << "read(" << stream << ", "
              << fixedParam << ", " << scope << "__U__" << fixKwd(seq->name()) << "());";
        }
        /*
        string scope = fixKwd(seq->scope());
        if (marshal) {
          out << nl << scope << "ice_write" << seq->name() << '(' << stream << ", " << fixedParam
              << ");";
        } else {
          out << nl << scope << "ice_read" << seq->name() << '(' << stream << ", " << fixedParam
              << ");";
        }
        */
      } else {
        switch (builtin->kind())
        {
        case Builtin::KindByte: {
          if (marshal) {
            out << nl << stream << "->write_byte_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_byte_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindBool: {
          if (marshal) {
            out << nl << stream << "->write_bool_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_bool_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindShort: {
          if (marshal) {
            out << nl << stream << "->write_short_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_short_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindInt: {
          if (marshal) {
            out << nl << stream << "->write_int_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_int_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindLong: {
          if (marshal) {
            out << nl << stream << "->write_long_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_long_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindFloat: {
          if (marshal) {
            out << nl << stream << "->write_float_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_float_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindDouble: {
          if (marshal) {
            out << nl << stream << "->write_double_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_double_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindString: {
          if (marshal) {
            out << nl << stream << "->write_string_seq(" << fixedParam << ");";
          } else {
            out << nl << stream << "->read_string_seq(" << fixedParam << ");";
          }
          break;
        }
        case Builtin::KindObject:
        case Builtin::KindObjectProxy:
        case Builtin::KindLocalObject: {
          assert(false);
          break;
        }
        }
      }
    }

    return;
  }

  DictionaryPtr dict = DictionaryPtr::dynamicCast(type);
  if (dict) {
    string scope = fixKwd(dict->scope());

    if (marshal) {
//      out << nl << scope << "__write(" << stream << ", " << fixedParam << ");";
      out << nl << scope << "__write(" << stream << ", " << fixedParam << ", " << scope << "__U__" << fixKwd(dict->name()) << "());";
    } else {
//      out << nl << scope << "__read(" << stream << ", " << fixedParam << ");";
      out << nl << scope << "__read(" << stream << ", " << fixedParam << ", " << scope << "__U__" << fixKwd(dict->name()) << "());";
    }

    return;
  }

  EnumPtr en = EnumPtr::dynamicCast(type);
  if (en) {
    string scope = fixKwd(en->scope());
    if (marshal) {
      out << nl << scope << "ice_write" << en->name() << "(" << stream << ", " << fixedParam
          << ");";
    } else {
      out << nl << scope << "ice_read" << en->name() << "(" << stream << ", " << fixedParam << ");";
    }

    return;
  }

  ProxyPtr prx = ProxyPtr::dynamicCast(type);
  if (prx) {
    ClassDeclPtr cls = prx->_class();
    string scope = fixKwd(cls->scope());
    if (marshal) {
      //out << nl << scope << "__write" << cls->name() << "Prx(" << stream << ", " << fixedParam
      //    << ");";
      out << nl << scope << "__write(" << stream << ", " << fixedParam << ");";
    } else {
      //out << nl << scope << "__read" << cls->name() << "Prx(" << stream << ", " << fixedParam
      //    << ");";
      out << nl << scope << "__read(" << stream << ", " << fixedParam << ");";
    }

    return;
  }

  assert(false);
}

void Slice::writeToStringCode(Output& out,
                              const TypePtr& type,
                              const string& param,
                              const string& typeString,
                              const StringList& metaData)
{
  string fixedParam = fixKwd(param);

  BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
  if (builtin) {
    out << nl << "oss << \"" << param << ":\" << " << param << " << \" \";";
    return;
  }

  //TODO
//  ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
//  if (cl) {
//    string scope = fixKwd(cl->scope());
//    if (marshal) {
//      //out << nl << scope << "ice_write" << cl->name() << "(" << stream << ", " << fixedParam << ");";
//      out << nl << fixedParam << "->__" << "write(" << stream << ");";
//    } else {
//      //out << nl << scope << "ice_read" << cl->name() << "(" << stream << ", " << fixedParam << ");";
//      out << nl << fixedParam << "->__" << "read(" << stream << ");";
//    }
//
//    return;
//  }

  StructPtr st = StructPtr::dynamicCast(type);
  if (st) {
    out << nl << "oss << \"" << param << ":[\" << " << param << ".toString() << \"] \";";
    return;
  }

  SequencePtr seq = SequencePtr::dynamicCast(type);
  if (seq) {
    string seqType = findMetaData(metaData, false);
    if (!seqType.empty()) {

    } else {
      out << nl << "oss << \"" << param << ":{\";";
      out << nl << "for(" << typeString << "::const_iterator it = " << param << ".begin(); it != " << param << ".end(); ++it)";
      out << sb;

      seqType = findMetaData(seq->getMetaData(), false);
      builtin = BuiltinPtr::dynamicCast(seq->type());
      if (!seqType.empty() || !builtin || (builtin->kind() == Builtin::KindObject
          || builtin->kind() == Builtin::KindObjectProxy))
      {
        out << nl << "oss << \"[\" << it->toString() << \"]\";";
      } else {
        out << nl << "oss << \"\" << *it << \" \";";
      }

      out << eb;
      out << nl << "oss << \"} \";";
    }

    return;
  }


  DictionaryPtr dict = DictionaryPtr::dynamicCast(type);
  if (dict) {
    string firstStr = "it->first";
    if(!BuiltinPtr::dynamicCast(dict->keyType())) {
      firstStr = "\"[\" << it->first.toString() << \"]\"";
    }
    string secondStr = "it->second";
    if(!BuiltinPtr::dynamicCast(dict->valueType())) {
      secondStr = "\"[\" << it->second.toString() << \"]\"";
    }

    string scope = fixKwd(dict->scope());

    out << nl << "oss << \"" << param << ":{\";";
    out << nl << "for(" << typeString << "::const_iterator it = " << param << ".begin(); it != " << param << ".end(); ++it)";
    out << sb;

//    seqType = findMetaData(seq->getMetaData(), false);
//    builtin = BuiltinPtr::dynamicCast(seq->type());
//    if (!seqType.empty() || !builtin || (builtin->kind() == Builtin::KindObject
//        || builtin->kind() == Builtin::KindObjectProxy))
//    {
      out << nl << "oss << \"<\" << " << firstStr << " << \",\" << " << secondStr << " << \">\";";
//    }

    out << eb;
    out << nl << "oss << \"} \";";

    return;
  }

  // TODO
//  EnumPtr en = EnumPtr::dynamicCast(type);
//  if (en) {
//    string scope = fixKwd(en->scope());
//    if (marshal) {
//      out << nl << scope << "ice_write" << en->name() << "(" << stream << ", " << fixedParam
//          << ");";
//    } else {
//      out << nl << scope << "ice_read" << en->name() << "(" << stream << ", " << fixedParam << ");";
//    }
//
//    return;
//  }
//
//  ProxyPtr prx = ProxyPtr::dynamicCast(type);
//  if (prx) {
//    ClassDeclPtr cls = prx->_class();
//    string scope = fixKwd(cls->scope());
//    if (marshal) {
//      //out << nl << scope << "__write" << cls->name() << "Prx(" << stream << ", " << fixedParam
//      //    << ");";
//      out << nl << scope << "__write(" << stream << ", " << fixedParam << ");";
//    } else {
//      //out << nl << scope << "__read" << cls->name() << "Prx(" << stream << ", " << fixedParam
//      //    << ");";
//      out << nl << scope << "__read(" << stream << ", " << fixedParam << ");";
//    }
//
//    return;
//  }
//
  assert(false);
}

string Slice::findMetaData(const StringList& metaData, bool inParam)
{
  static const string prefix = "cpp:";
  for (StringList::const_iterator q = metaData.begin(); q != metaData.end(); ++q) {
    string str = *q;
    if (str.find(prefix) == 0) {
      string::size_type pos = str.find(':', prefix.size());
      if (pos != string::npos) {
        string ss = str.substr(prefix.size(), pos - prefix.size());
        if (ss == "type") {
          return str.substr(pos + 1);
        } else if (inParam && ss == "range") {
          return str.substr(prefix.size());
        }
      } else if (inParam) {
        string ss = str.substr(prefix.size());
        if (ss == "array" || ss == "range") {
          return ss;
        }

      }
    }
  }

  return "";
}
