// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Gen.h>
#include <Slice/CPlusPlusUtil.h>
#include <IceUtil/Functional.h>
#include <IceUtil/Iterator.h>

#include <limits>
#include <sys/stat.h>

using namespace std;
using namespace Slice;
using namespace IceUtil;

static void getIds(const ClassDefPtr& p, StringList& ids)
{
  ClassList allBases = p->allBases();
#if defined(__IBMCPP__) && defined(NDEBUG)
  //
  // VisualAge C++ 6.0 does not see that ClassDef is a Contained,
  // when inlining is on. The code below issues a warning: better
  // than an error!
  //
  transform(allBases.begin(), allBases.end(), back_inserter(ids), ::IceUtil::constMemFun<string,ClassDef>(&Contained::scoped));
#else
  transform(allBases.begin(), allBases.end(), back_inserter(ids), ::IceUtil::constMemFun(&Contained::scoped));
#endif
  StringList other;
  other.push_back(p->scoped());
  other.push_back("::Ice::Object");
  other.sort();
  ids.merge(other);
  ids.unique();
}

Slice::Gen::Gen(const string& name,
                const string& base,
                const string& headerExtension,
                const string& sourceExtension,
                const vector<string>& extraHeaders,
                const string& include,
                const vector<string>& includePaths,
                const string& dllExport,
                const string& dir,
                bool imp) :
  _base(base), _headerExtension(headerExtension), _sourceExtension(sourceExtension),
      _extraHeaders(extraHeaders), _include(include), _includePaths(includePaths),
      _dllExport(dllExport), _impl(imp)
{
  for (vector<string>::iterator p = _includePaths.begin(); p != _includePaths.end(); ++p) {
    if (p->length() && (*p)[p->length() - 1] != '/') {
      *p += '/';
    }
  }

  string::size_type pos = _base.find_last_of("/\\");
  if (pos != string::npos) {
    _base.erase(0, pos + 1);
  }

  if (_impl) {
    string fileImplH = _base + "I." + _headerExtension;
    string fileImplC = _base + "I." + _sourceExtension;
    if (!dir.empty()) {
      fileImplH = dir + '/' + fileImplH;
      fileImplC = dir + '/' + fileImplC;
    }

    struct stat st;
    if (stat(fileImplH.c_str(), &st) == 0) {
      cerr << name << ": `" << fileImplH << "' already exists - will not overwrite" << endl;
      return;
    }
    if (stat(fileImplC.c_str(), &st) == 0) {
      cerr << name << ": `" << fileImplC << "' already exists - will not overwrite" << endl;
      return;
    }

    implH.open(fileImplH.c_str());
    if (!implH) {
      cerr << name << ": can't open `" << fileImplH << "' for writing" << endl;
      return;
    }

    implC.open(fileImplC.c_str());
    if (!implC) {
      cerr << name << ": can't open `" << fileImplC << "' for writing" << endl;
      return;
    }

    string s = fileImplH;
    if (_include.size()) {
      s = _include + '/' + s;
    }
    transform(s.begin(), s.end(), s.begin(), ToIfdef());
    implH << "#ifndef __" << s << "__";
    implH << "\n#define __" << s << "__";
    implH << '\n';
  }

  string fileH = _base + "." + _headerExtension;
  string fileC = _base + "." + _sourceExtension;
  if (!dir.empty()) {
    fileH = dir + '/' + fileH;
    fileC = dir + '/' + fileC;
  }

  H.open(fileH.c_str());
  if (!H) {
    cerr << name << ": can't open `" << fileH << "' for writing" << endl;
    return;
  }

  C.open(fileC.c_str());
  if (!C) {
    cerr << name << ": can't open `" << fileC << "' for writing" << endl;
    return;
  }

/*
  printHeader(H);
  printHeader(C);
  H << "\n// Generated from file `" << changeInclude(_base, _includePaths) << ".ice'\n";
  C << "\n// Generated from file `" << changeInclude(_base, _includePaths) << ".ice'\n";
  */

  string s = fileH;
  if (_include.size()) {
    s = _include + '/' + s;
  }
  transform(s.begin(), s.end(), s.begin(), ToIfdef());
  H << "\n#ifndef __" << s << "__";
  H << "\n#define __" << s << "__";
  H << '\n';
}

Slice::Gen::Gen(const string& name,
                const string& base,
                const string& headerExtension,
                const string& sourceExtension,
                const vector<string>& extraHeaders,
                const string& include,
                const vector<string>& includePaths,
                const string& dllExport,
                const string& dir,
                bool imp,
                bool outputToConsole) :
      H(outputToConsole), C(outputToConsole), _base(base), _headerExtension(headerExtension), _sourceExtension(sourceExtension),
      _extraHeaders(extraHeaders), _include(include), _includePaths(includePaths),
      _dllExport(dllExport), _impl(imp)
{
  for (vector<string>::iterator p = _includePaths.begin(); p != _includePaths.end(); ++p) {
    if (p->length() && (*p)[p->length() - 1] != '/') {
      *p += '/';
    }
  }

  string::size_type pos = _base.find_last_of("/\\");
  if (pos != string::npos) {
    _base.erase(0, pos + 1);
  }

  if (_impl) {
    string fileImplH = _base + "I." + _headerExtension;
    string fileImplC = _base + "I." + _sourceExtension;
    if (!dir.empty()) {
      fileImplH = dir + '/' + fileImplH;
      fileImplC = dir + '/' + fileImplC;
    }

    struct stat st;
    if (stat(fileImplH.c_str(), &st) == 0) {
      cerr << name << ": `" << fileImplH << "' already exists - will not overwrite" << endl;
      return;
    }
    if (stat(fileImplC.c_str(), &st) == 0) {
      cerr << name << ": `" << fileImplC << "' already exists - will not overwrite" << endl;
      return;
    }

    implH.open(fileImplH.c_str());
    if (!implH) {
      cerr << name << ": can't open `" << fileImplH << "' for writing" << endl;
      return;
    }

    implC.open(fileImplC.c_str());
    if (!implC) {
      cerr << name << ": can't open `" << fileImplC << "' for writing" << endl;
      return;
    }

    string s = fileImplH;
    if (_include.size()) {
      s = _include + '/' + s;
    }
    transform(s.begin(), s.end(), s.begin(), ToIfdef());
    implH << "#ifndef __" << s << "__";
    implH << "\n#define __" << s << "__";
    implH << '\n';
  }

  string fileH = _base + "." + _headerExtension;
  string fileC = _base + "." + _sourceExtension;
  if (!dir.empty()) {
    fileH = dir + '/' + fileH;
    fileC = dir + '/' + fileC;
  }

  H.open(fileH.c_str());
  if (!H) {
    cerr << name << ": can't open `" << fileH << "' for writing" << endl;
    return;
  }

  C.open(fileC.c_str());
  if (!C) {
    cerr << name << ": can't open `" << fileC << "' for writing" << endl;
    return;
  }

/*
  printHeader(H);
  printHeader(C);
  H << "\n// Generated from file `" << changeInclude(_base, _includePaths) << ".ice'\n";
  C << "\n// Generated from file `" << changeInclude(_base, _includePaths) << ".ice'\n";
  */

  string s = fileH;
  if (_include.size()) {
    s = _include + '/' + s;
  }
  transform(s.begin(), s.end(), s.begin(), ToIfdef());
  H << "\n#ifndef __" << s << "__";
  H << "\n#define __" << s << "__";
  H << '\n';
}

Slice::Gen::~Gen()
{
  H << "\n\n#endif\n";
  C << '\n';

  if (_impl) {
    implH << "\n\n#endif\n";
    implC << '\n';
  }
}

bool Slice::Gen::operator!() const
{
  if(!H || !C)
  {
    return true;
  }
  if(_impl && (!implH || !implC))
  {
    return true;
  }
  return false;
}

void Slice::Gen::generate(const UnitPtr& p)
{
  validateMetaData(p);

  H << "\n#include <string>";
  H << "\n#include <vector>";
  H << "\n#include <list>";
  H << "\n#include <os/OS.h>";
  H << "\n#include <icm/Proxy.h>";
  H << "\n#include <icm/Object.h>";
  H << "\n#include <icm/ReplyDispatcher.h>";
  H << "\n#include <icm/AmhResponseHandler.h>";
  H << "\n#include <icm/IcmError.h>";

  StringList includes = p->includeFiles();

  for (StringList::const_iterator q = includes.begin(); q != includes.end(); ++q) {
    //H << "\n#include <" << changeInclude(*q, _includePaths) << "." << _headerExtension << ">";
    H << "\n#include \"" << changeInclude(*q, _includePaths) << "." << _headerExtension << "\"";
  }

  H << "\r\nclass ServerRequest;";

  C << "\n#include <algorithm>";
  C << "\n#include <sstream>";

  C << "\n#include \"";
  if (_include.size()) {
    C << _include << '/';
  }
  C << _base << "." << _headerExtension << "\"";

  C << "\n#include <icm/ServerRequest.h>";
  C << "\n#include <icm/IcmStream.h>";
  C << "\n#include <icm/Invocation.h>";
  C << "\n#include <icm/Communicator.h>";
  C << "\n#include <icc/Log.h>";
  C << "\n#include <icc/Guard.h>";

/*
  writeExtraHeaders(C);

  H << "\n#include <IceE/LocalObjectF.h>";
  H << "\n#include <IceE/ProxyF.h>";
  H << "\n#ifndef ICEE_PURE_CLIENT";
  H << "\n#  include <IceE/ObjectF.h>";
  H << "\n#endif";
  H << "\n#include <IceE/Exception.h>";
  H << "\n#include <IceE/LocalObject.h>";

  if (p->usesProxies()) {
    H << "\n#include <IceE/Proxy.h>";
  }

  if (p->hasNonLocalClassDefs()) {
    H << "\n#ifndef ICEE_PURE_CLIENT";
    H << "\n#  include <IceE/Object.h>";
    H << "\n#  include <IceE/Incoming.h>";
    H << "\n#endif";
    H << "\n#include <IceE/Outgoing.h>";
    C << "\n#include <IceE/Connection.h>";
    C << "\n#include <IceE/LocalException.h>";
  }

  if (p->hasNonLocalExceptions()) {
    H << "\n#include <IceE/UserExceptionFactory.h>";
  }

  if (p->hasDataOnlyClasses() || p->hasNonLocalExceptions()) {
    H << "\n#include <IceE/FactoryTable.h>";
  }

  if (p->usesNonLocals()) {
    C << "\n#include <IceE/BasicStream.h>";
    C << "\n#ifndef ICEE_PURE_CLIENT";
    C << "\n#  include <IceE/Object.h>";
    C << "\n#endif";
  }

  if (p->hasNonLocalExceptions()) {
    C << "\n#include <IceE/LocalException.h>";
  }

  C << "\n#include <IceE/Iterator.h>";


  H << "\n#include <IceE/UndefSysMacros.h>";

  GlobalIncludeVisitor globalIncludeVisitor(H);
  p->visit(&globalIncludeVisitor, false);

  printVersionCheck(H);
  printVersionCheck(C);

  printDllExportStuff(H, _dllExport);
  if (_dllExport.size()) {
    _dllExport += " ";
  }

  ProxyDeclVisitor proxyDeclVisitor(H, C, _dllExport);
  p->visit(&proxyDeclVisitor, false);

  ObjectDeclVisitor objectDeclVisitor(H, C, _dllExport);
  p->visit(&objectDeclVisitor, false);

  IceInternalVisitor iceInternalVisitor(H, C, _dllExport);
  p->visit(&iceInternalVisitor, false);

  HandleVisitor handleVisitor(H, C, _dllExport);
  p->visit(&handleVisitor, false);
*/
  ProxyDeclVisitor proxyDeclVisitor(H, C, _dllExport);
  p->visit(&proxyDeclVisitor, false);

  HandleVisitor handleVisitor(H, C, _dllExport);
  p->visit(&handleVisitor, false);

  TypesVisitor typesVisitor(H, C, _dllExport);
  p->visit(&typesVisitor, false);

  AsyncVisitor asyncVisitor(H, C, _dllExport);
  p->visit(&asyncVisitor, false);

  ObjectVisitor objectVisitor(H, C, _dllExport);
  p->visit(&objectVisitor, false);

  ProxyVisitor proxyVisitor(H, C, _dllExport);
  p->visit(&proxyVisitor, false);

  AsyncImplVisitor asyncImplVisitor(H, C, _dllExport);
  p->visit(&asyncImplVisitor, false);

  if (_impl) {
    implH << "\n#include <";
    if (_include.size()) {
      implH << _include << '/';
    }
    implH << _base << ".h>";

    writeExtraHeaders(implC);

    implC << "\n#include <";
    if (_include.size()) {
      implC << _include << '/';
    }
    implC << _base << "I.h>";

    ImplVisitor implVisitor(implH, implC, _dllExport);
    p->visit(&implVisitor, false);
  }
}

void Slice::Gen::writeExtraHeaders(Output& out)
{
  for (vector<string>::const_iterator i = _extraHeaders.begin(); i != _extraHeaders.end(); ++i) {
    string hdr = *i;
    string guard;
    string::size_type pos = hdr.rfind(',');
    if (pos != string::npos) {
      hdr = i->substr(0, pos);
      guard = i->substr(pos + 1);
    }
    if (!guard.empty()) {
      out << "\n#ifndef " << guard;
      out << "\n#define " << guard;
    }
    out << "\n#include <";
    if (!_include.empty()) {
      out << _include << '/';
    }
    out << hdr << '>';
    if (!guard.empty()) {
      out << "\n#endif";
    }
  }
}

Slice::Gen::GlobalIncludeVisitor::GlobalIncludeVisitor(Output& h) :
  H(h), _finished(false)
{
}

bool Slice::Gen::GlobalIncludeVisitor::visitModuleStart(const ModulePtr& p)
{
  if (!_finished) {
    DefinitionContextPtr dc = p->definitionContext();
    assert(dc);
    StringList globalMetaData = dc->getMetaData();

    static const string includePrefix = "cpp:include:";

    for (StringList::const_iterator q = globalMetaData.begin(); q != globalMetaData.end(); ++q) {
      string s = *q;
      if (s.find(includePrefix) == 0) {
        H << nl << "#include <" << s.substr(includePrefix.size()) << ">";
      }
    }
    _finished = true;
  }

  return false;
}

Slice::Gen::TypesVisitor::TypesVisitor(Output& h, Output& c, const string& dllExport) :
  H(h), C(c), _useWstring(false), _dllExport(dllExport), _doneStaticSymbol(false)
{
}

bool Slice::Gen::TypesVisitor::visitModuleStart(const ModulePtr& p)
{
  string name = fixKwd(p->name());

  H << sp << nl << "namespace " << name << nl << '{';

  C << nl << nl << "namespace " << name;
  C << sb;

  C << nl << "template<typename T>";
  C << nl << "std::string __toString(T t)";
  C << sb;
  C << nl << "std::ostringstream oss;";
  C << nl << "oss << t;";
  C << nl << "return oss.str();";
  C << eb;

  C << nl << nl << "std::string& trimEnd(std::string& str)";
  C << sb;
  C << nl << "while(isspace(str[str.size() - 1]))";
  C << sb;
  C << nl << "str = str.substr(0, str.size()-1);";
  C << eb;
  C << nl << "return str;";
  C << eb;
//  std::string& trimEnd(std::string& str) {
//    while(isspace(str[str.size() - 1])) {
//      str = str.substr(0, str.size()-1);
//    }
//    return str;
//  }
  C << eb;

  return true;
}

void Slice::Gen::TypesVisitor::visitModuleEnd(const ModulePtr& p)
{
  H << sp << nl << '}';
}

bool Slice::Gen::TypesVisitor::visitClassDefStart(const ClassDefPtr&)
{
  return false;
}

bool Slice::Gen::TypesVisitor::visitExceptionStart(const ExceptionPtr& p)
{
  string name = fixKwd(p->name());
  string scoped = fixKwd(p->scoped());
  ExceptionPtr base = p->base();
  DataMemberList dataMembers = p->dataMembers();
  DataMemberList allDataMembers = p->allDataMembers();
  DataMemberList::const_iterator q;

  vector<string> params;
  vector<string> allTypes;
  vector<string> allParamDecls;
  vector<string> baseParams;
  vector<string>::const_iterator pi;

  for (q = dataMembers.begin(); q != dataMembers.end(); ++q) {
    params.push_back(fixKwd((*q)->name()));
  }

  for (q = allDataMembers.begin(); q != allDataMembers.end(); ++q) {
    string typeName = inputTypeToString((*q)->type(), _useWstring, (*q)->getMetaData());
    allTypes.push_back(typeName);
    allParamDecls.push_back(typeName + " __ice_" + (*q)->name());
  }

  if (base) {
    DataMemberList baseDataMembers = base->allDataMembers();
    for (q = baseDataMembers.begin(); q != baseDataMembers.end(); ++q) {
      baseParams.push_back("__ice_" + (*q)->name());
    }
  }

  H << sp << nl << "class " << _dllExport << name << " : ";
  H.useCurrentPosAsIndent();
  H << "public ";
  if (!base) {
    H << (p->isLocal() ? "::Ice::LocalException" : "::Ice::UserException");
  } else {
    H << fixKwd(base->scoped());
  }
  H.restoreIndent();
  H << sb;

  H.dec();
  H << nl << "public:";
  H.inc();

  H << sp << nl << name << spar;
  if (p->isLocal()) {
    H << "const char*" << "int";
  }
  H << epar;
  if (!p->isLocal()) {
    H << " {}";
  } else {
    H << ';';
  }
  if (!allTypes.empty()) {
    H << nl;
    if (!p->isLocal() && allTypes.size() == 1) {
      H << "explicit ";
    }
    H << name << spar;
    if (p->isLocal()) {
      H << "const char*" << "int";
    }
    H << allTypes << epar << ';';
  }
  H << sp;

  if (p->isLocal()) {
    C << sp << nl << scoped.substr(2) << "::" << name << spar << "const char* __file"
        << "int __line" << epar << " :";
    C.inc();
    emitUpcall(base, "(__file, __line)", true);
    C.dec();
    C << sb;
    C << eb;
  }

  if (!allTypes.empty()) {
    C << sp << nl;
    C << scoped.substr(2) << "::" << name << spar;
    if (p->isLocal()) {
      C << "const char* __file" << "int __line";
    }
    C << allParamDecls << epar;
    if (p->isLocal() || !baseParams.empty() || !params.empty()) {
      C << " :";
      C.inc();
      string upcall;
      if (!allParamDecls.empty()) {
        upcall = "(";
        if (p->isLocal()) {
          upcall += "__file, __line";
        }
        for (pi = baseParams.begin(); pi != baseParams.end(); ++pi) {
          if (p->isLocal() || pi != baseParams.begin()) {
            upcall += ", ";
          }
          upcall += *pi;
        }
        upcall += ")";
      }
      if (!params.empty()) {
        upcall += ",";
      }
      emitUpcall(base, upcall, p->isLocal());
    }
    for (pi = params.begin(); pi != params.end(); ++pi) {
      if (pi != params.begin()) {
        C << ",";
      }
      C << nl << *pi << "(__ice_" << *pi << ')';
    }
    if (p->isLocal() || !baseParams.empty() || !params.empty()) {
      C.dec();
    }
    C << sb;
    C << eb;
  }

  H << nl << "virtual const ::std::string ice_name() const;";

  string flatName = p->flattenedScope() + p->name() + "_name";

  C << sp << nl << "static const char* " << flatName << " = \"" << p->scoped().substr(2) << "\";";
  C << sp << nl << "const ::std::string" << nl << scoped.substr(2) << "::ice_name() const";
  C << sb;
  C << nl << "return " << flatName << ';';
  C << eb;

  if (p->isLocal()) {
    H << nl << "virtual ::std::string  toString() const;";
  }

  H << nl << "virtual ::Ice::Exception* ice_clone() const;";
  C << sp << nl << "::Ice::Exception*" << nl << scoped.substr(2) << "::ice_clone() const";
  C << sb;
  C << nl << "return new " << name << "(*this);";
  C << eb;

  H << nl << "virtual void ice_throw() const;";
  C << sp << nl << "void" << nl << scoped.substr(2) << "::ice_throw() const";
  C << sb;
  C << nl << "throw *this;";
  C << eb;

  if (!p->isLocal()) {
    H << sp << nl << "static const ::IceInternal::UserExceptionFactoryPtr& ice_factory();";
  }
  if (!dataMembers.empty()) {
    H << sp;
  }
  return true;
}

void Slice::Gen::TypesVisitor::visitExceptionEnd(const ExceptionPtr& p)
{
  string name = fixKwd(p->name());
  string scope = fixKwd(p->scope());
  string scoped = fixKwd(p->scoped());
  DataMemberList dataMembers = p->dataMembers();
  DataMemberList::const_iterator q;

  string factoryName;

  if (!p->isLocal()) {
    ExceptionPtr base = p->base();

    H << sp << nl << "virtual void __write(::IceInternal::BasicStream*) const;";
    H << nl << "virtual void __read(::IceInternal::BasicStream*, bool);";
    C << sp << nl << "void" << nl << scoped.substr(2)
        << "::__write(::IceInternal::BasicStream* __os) const";
    C << sb;
    C << nl << "__os->write(::std::string(\"" << p->scoped() << "\"));";
    C << nl << "__os->startWriteSlice();";
    for (q = dataMembers.begin(); q != dataMembers.end(); ++q) {
      writeMarshalUnmarshalCode(C, (*q)->type(), fixKwd((*q)->name()), true, "", true, (*q)->getMetaData());
    }
    C << nl << "__os->endWriteSlice();";
    if (base) {
      emitUpcall(base, "::__write(__os);");
    }
    C << eb;

    C << sp << nl << "void" << nl << scoped.substr(2)
        << "::__read(::IceInternal::BasicStream* __is, bool __rid)";
    C << sb;
    C << nl << "if(__rid)";
    C << sb;
    C << nl << "::std::string myId;";
    C << nl << "__is->read(myId);";
    C << eb;
    C << nl << "__is->startReadSlice();";
    for (q = dataMembers.begin(); q != dataMembers.end(); ++q) {
      writeMarshalUnmarshalCode(C, (*q)->type(), fixKwd((*q)->name()), false, "", true, (*q)->getMetaData());
    }
    C << nl << "__is->endReadSlice();";
    if (base) {
      emitUpcall(base, "::__read(__is, true);");
    }
    C << eb;

    factoryName = "__F" + p->flattenedScope() + p->name();

    C << sp << nl << "struct " << factoryName << " : public ::IceInternal::UserExceptionFactory";
    C << sb;
    C << sp << nl << "virtual void";
    C << nl << "createAndThrow()";
    C << sb;
    C << nl << "throw " << scoped << "();";
    C << eb;
    C << eb << ';';

    C << sp << nl << "static ::IceInternal::UserExceptionFactoryPtr " << factoryName
        << "__Ptr = new " << factoryName << ';';

    C << sp << nl << "const ::IceInternal::UserExceptionFactoryPtr&";
    C << nl << scoped.substr(2) << "::ice_factory()";
    C << sb;
    C << nl << "return " << factoryName << "__Ptr;";
    C << eb;

    C << sp << nl << "class " << factoryName << "__Init";
    C << sb;
    C.dec();
    C << nl << "public:";
    C.inc();
    C << sp << nl << factoryName << "__Init()";
    C << sb;
    C << nl << "::IceInternal::factoryTable->addExceptionFactory(\"" << p->scoped() << "\", "
        << scoped << "::ice_factory());";
    C << eb;
    C << sp << nl << "~" << factoryName << "__Init()";
    C << sb;
    C << nl << "::IceInternal::factoryTable->removeExceptionFactory(\"" << p->scoped() << "\");";
    C << eb;
    C << eb << ';';
    C << sp << nl << "static " << factoryName << "__Init " << factoryName << "__i;";
    C << sp << nl << "#ifdef __APPLE__";

    string initfuncname = "__F" + p->flattenedScope() + p->name() + "__initializer";
    C << nl << "extern \"C\" { void " << initfuncname << "() {} }";
    C << nl << "#endif";
  }
  H << eb << ';';

  if (!p->isLocal()) {
    //
    // We need an instance here to trigger initialization if the implementation is in a shared libarry.
    // But we do this only once per source file, because a single instance is sufficient to initialize
    // all of the globals in a shared library.
    //
    if (!_doneStaticSymbol) {
      _doneStaticSymbol = true;
      H << sp << nl << "static " << name << " __" << p->name() << "_init;";
    }
  }
}

bool Slice::Gen::TypesVisitor::visitStructStart(const StructPtr& p)
{
  string name = fixKwd(p->name());

  H << sp << nl << "struct " << name;
  H << sb;
  H << nl << name << "();";

  return true;
}

void Slice::Gen::TypesVisitor::visitStructEnd(const StructPtr& p)
{
  string name = fixKwd(p->name());
  string scoped = fixKwd(p->scoped());
  string scope = fixKwd(p->scope());

  DataMemberList dataMembers = p->dataMembers();
  DataMemberList::const_iterator q;

  vector<string> params;
  vector<string>::const_iterator pi;

  for (q = dataMembers.begin(); q != dataMembers.end(); ++q) {
    params.push_back(fixKwd((*q)->name()));
  }

  H << sp;
  H << nl << _dllExport << "bool operator==(const " << name << "&) const;";
  H << nl << _dllExport << "bool operator!=(const " << name << "&) const;";
  //H << nl << _dllExport << "bool operator<(const " << name << "&) const;";

  C << sp << nl << scoped.substr(2) << "::" << name << "()";
  C << sb;
  for (q = dataMembers.begin(); q != dataMembers.end(); ++q) {
    BuiltinPtr builtin = BuiltinPtr::dynamicCast((*q)->type());
    if (builtin) {
      switch (builtin->kind())
      {
      case Builtin::KindBool:
      {
        C << nl << fixKwd((*q)->name()) << " = false;";
        break;
      }
      case Builtin::KindByte:
      case Builtin::KindShort:
      case Builtin::KindInt:
      case Builtin::KindLong:
      case Builtin::KindFloat:
      case Builtin::KindDouble:
      {
        C << nl << fixKwd((*q)->name()) << " = 0;";
        break;
      }
      default:
      {
        break;
      }
      }
    }
  }
  C << eb;

  C << sp << nl << "bool" << nl << scoped.substr(2) << "::operator==(const " << name
      << "& __rhs) const";
  C << sb;
  C << nl << "return !operator!=(__rhs);";
  C << eb;
  C << sp << nl << "bool" << nl << scoped.substr(2) << "::operator!=(const " << name
      << "& __rhs) const";
  C << sb;
  C << nl << "if(this == &__rhs)";
  C << sb;
  C << nl << "return false;";
  C << eb;
  for (pi = params.begin(); pi != params.end(); ++pi) {
    C << nl << "if(" << *pi << " != __rhs." << *pi << ')';
    C << sb;
    C << nl << "return true;";
    C << eb;
  }
  C << nl << "return false;";
  C << eb;

  if (!p->isLocal()) {
    //
    // None of these member functions is virtual!
    //
    H << sp << nl << _dllExport << "void __write(OutputStream*) const;";
    H << nl << _dllExport << "void __read(InputStream*);";
    H << nl;
    H << nl << "std::string toString() const;";

    C << sp << nl << "void" << nl << scoped.substr(2)
        << "::__write(OutputStream* __os) const";
    C << sb;
    for (q = dataMembers.begin(); q != dataMembers.end(); ++q) {
      writeStreamMarshalUnmarshalCode(C, (*q)->type(), fixKwd((*q)->name()), true, "", (*q)->getMetaData());
    }
    C << eb;

    C << sp << nl << "void" << nl << scoped.substr(2)
        << "::__read(InputStream* __is)";
    C << sb;
    for (q = dataMembers.begin(); q != dataMembers.end(); ++q) {
      //writeMarshalUnmarshalCode(C, (*q)->type(), fixKwd((*q)->name()), false, "", true, (*q)->getMetaData());
      writeStreamMarshalUnmarshalCode(C, (*q)->type(), fixKwd((*q)->name()), false, "", (*q)->getMetaData());
    }
    C << eb;

    C << sp << nl << "std::string" << nl << scoped.substr(2) << "::toString() const";
    C << sb;
//    C << nl << "std::ostringstream oss;";
    C << nl << "std::string result(\"" << scoped.substr(2) << "-> \");";

    for (q = dataMembers.begin(); q != dataMembers.end(); ++q) {
//      string typeString = typeToString((*q)->type(), (*q)->getMetaData());
////      assert(typeString == name);
//      writeToStringCode(C, (*q)->type(), fixKwd((*q)->name()), typeString, (*q)->getMetaData());

//      C << nl << "oss << \"" << fixKwd((*q)->name()) << ":\" << __toString(" << fixKwd((*q)->name()) << ") << \" \";";
      C << nl << "result += (\"" << fixKwd((*q)->name()) << ":\" + __toString(this->" << fixKwd((*q)->name()) << ") + \" \");";
    }

//    C << nl << "return oss.str();";
    C << nl << "return trimEnd(result);";
    C << eb;
  }

  H << eb << ';';
  H << nl << _dllExport << "std::string __toString(const " << name << "&);";

  C << sp << nl << "std::string" << nl << scope.substr(2) << "__toString(const " << name << "& arg)";
  C << sb;
  C << nl << "return \"[\" + arg.toString() + \"]\";";
  C << eb;
}

void Slice::Gen::TypesVisitor::visitDataMember(const DataMemberPtr& p)
{
  string name = fixKwd(p->name());
  string s = typeToString(p->type(), p->getMetaData());
  H << nl << s << ' ' << name << ';';
}

void Slice::Gen::TypesVisitor::visitList(const ListPtr& p)
{
  string name = fixKwd(p->name());
  TypePtr type = p->type();
  string typeStr = typeToString(type);
  StringList metaData = p->getMetaData();
  string listType = findMetaData(metaData, true);
  if (!listType.empty() && listType != "array" && listType.find("range") != 0) {
    H << sp << nl << "typedef " << listType << ' ' << name << ';';
  } else {
    H << sp << nl << "typedef ::std::list<" << (typeStr[0] == ':' ? " " : "") << typeStr << "> " << name
        << ';';
  }

  BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
  if (!p->isLocal()) {
    string scoped = fixKwd(p->scoped());
    string scope = fixKwd(p->scope());

    H << sp << nl << "class __U__" << name << " { };";

    if(visitedContainerTypes.find(typeStr) == visitedContainerTypes.end()) {
      H << nl << _dllExport << "std::string __toString(const " << name << "&);";
      C << sp << nl << "std::string" << nl << scope.substr(2) << "__toString(const " << name << "& arg)";
      C << sb;
      C << nl << "std::string result = \"{\";";
      C << nl << "for(" << name << "::const_iterator it = arg.begin(); it != arg.end(); ++it)";
      C << sb;
      C << nl << "result += __toString(*it);";
      C << nl << "result += \" \";";
      C << eb;
      C << nl << "trimEnd(result);";
      C << nl << "result += \"}\";";
      C << nl << "return result;";
      C << eb;

      visitedContainerTypes.insert(typeStr);
    }

    if (!listType.empty()) {
      H << nl << _dllExport << "void __write(OutputStream*, const " << name
          << "&, __U__" << name << ");";
      H << nl << _dllExport << "void __read(InputStream*, " << name << "&, __U__"
          << name << ");";


      C << sp << nl << "void" << nl << scope.substr(2)
          << "__write(OutputStream* __os, const " << scoped << "& v, " << scope
          << "__U__" << name << ")";
      C << sb;
      C << nl << "UInt size = static_cast< UInt>(v.size());";
      C << nl << "__os->write_uint(size);";
      C << nl << "for(" << name << "::const_iterator p = v.begin(); p != v.end(); ++p)";
      C << sb;
      writeStreamMarshalUnmarshalCode(C, type, "(*p)", true);
      C << eb;
      C << eb;

      C << sp << nl << "void" << nl << scope.substr(2)
          << "__read(InputStream* __is, " << scoped << "& v, " << scope << "__U__"
          << name << ')';
      C << sb;
      C << nl << "UInt sz;";
      C << nl << "__is->read_uint(sz);";
      C << nl << name << "(sz).swap(v);";
      if (type->isVariableLength()) {
        // Protect against bogus sequence sizes.
        //C << nl << "__is->startSeq(sz, " << type->minWireSize() << ");";
      } else {
        //C << nl << "__is->checkFixedSeq(sz, " << type->minWireSize() << ");";
      }
      C << nl << "for(" << name << "::iterator p = v.begin(); p != v.end(); ++p)";
      C << sb;
      writeStreamMarshalUnmarshalCode(C, type, "(*p)", false);

      C << eb;
      C << eb;
    } else if (!builtin || builtin->kind() == Builtin::KindObject || builtin->kind()
        == Builtin::KindObjectProxy) {
//      H << nl << _dllExport << "void __write(OutputStream*, const " << typeStr
//          << "*, const " << typeStr << "*, __U__" << name << ");";
      H << nl << _dllExport << "void __write(OutputStream*, const " << name << "&, __U__" << name << ");";
      H << nl << _dllExport << "void __read(InputStream*, " << name << "&, __U__" << name << ");";

      C << sp << nl << "void" << nl << scope.substr(2)
          << "__write(OutputStream* __os, const " << name << "& l, " << scope << "__U__" << name << ")";
      C << sb;
      C << nl << "UInt size = static_cast< UInt>(l.size());";
      C << nl << "__os->write_uint(size);";
//      C << nl << "for(UInt i = 0; i < size; ++i)";
      C << nl << "for(" << name << "::const_iterator it = l.begin(); it != l.end(); ++it)";
      C << sb;
//      writeMarshalUnmarshalCode(C, type, "begin[i]", true);
      writeMarshalUnmarshalCode(C, type, "(*it)", true);
      C << eb;
      C << eb;

      C << sp << nl << "void" << nl << scope.substr(2)
          << "__read(InputStream* __is, " << scoped << "& v, " << scope << "__U__"
          << name << ')';
      C << sb;
      C << nl << "UInt sz;";
      C << nl << "__is->read_uint(sz);";
      C << nl << "v.clear();";
      C << nl << "for(UInt i = 0; i < sz; ++i)";
      C << sb;
      C << nl << typeStr << " tmpElement;";
      writeMarshalUnmarshalCode(C, type, "tmpElement", false);
      C << nl << "v.push_back(tmpElement);";
      C << eb;
      C << eb;
    }
  }
}

void Slice::Gen::TypesVisitor::visitSequence(const SequencePtr& p)
{
  string name = fixKwd(p->name());
  TypePtr type = p->type();
  string typeStr = typeToString(type);
  StringList metaData = p->getMetaData();
  string seqType = findMetaData(metaData, true);
  if (!seqType.empty() && seqType != "array" && seqType.find("range") != 0) {
    H << sp << nl << "typedef " << seqType << ' ' << name << ';';
  } else {
    H << sp << nl << "typedef ::std::vector<" << (typeStr[0] == ':' ? " " : "") << typeStr << "> " << name
        << ';';
  }

  BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
  if (!p->isLocal()) {
    string scoped = fixKwd(p->scoped());
    string scope = fixKwd(p->scope());

    H << sp << nl << "class __U__" << name << " { };";

    if(visitedContainerTypes.find(typeStr) == visitedContainerTypes.end()) {
      H << nl << _dllExport << "std::string __toString(const " << name << "&);";
      C << sp << nl << "std::string" << nl << scope.substr(2) << "__toString(const " << name << "& arg)";
      C << sb;
      C << nl << "std::string result = \"{\";";
      C << nl << "for(" << name << "::const_iterator it = arg.begin(); it != arg.end(); ++it)";
      C << sb;
      C << nl << "result += __toString(*it);";
      C << nl << "result += \" \";";
      C << eb;
      C << nl << "trimEnd(result);";
      C << nl << "result += \"}\";";
      C << nl << "return result;";
      C << eb;

      visitedContainerTypes.insert(typeStr);
    }

    if (!seqType.empty()) {
      H << nl << _dllExport << "void __write(OutputStream*, const " << name
          << "&, __U__" << name << ");";
      H << nl << _dllExport << "void __read(InputStream*, " << name << "&, __U__"
          << name << ");";


      C << sp << nl << "void" << nl << scope.substr(2)
          << "__write(OutputStream* __os, const " << scoped << "& v, " << scope
          << "__U__" << name << ")";
      C << sb;
      C << nl << "UInt size = static_cast< UInt>(v.size());";
      C << nl << "__os->write_uint(size);";
      C << nl << "for(" << name << "::const_iterator p = v.begin(); p != v.end(); ++p)";
      C << sb;
      writeStreamMarshalUnmarshalCode(C, type, "(*p)", true);
      C << eb;
      C << eb;

      C << sp << nl << "void" << nl << scope.substr(2)
          << "__read(InputStream* __is, " << scoped << "& v, " << scope << "__U__"
          << name << ')';
      C << sb;
      C << nl << "UInt sz;";
      C << nl << "__is->read_uint(sz);";
      C << nl << name << "(sz).swap(v);";
      if (type->isVariableLength()) {
        // Protect against bogus sequence sizes.
        //C << nl << "__is->startSeq(sz, " << type->minWireSize() << ");";
      } else {
        //C << nl << "__is->checkFixedSeq(sz, " << type->minWireSize() << ");";
      }
      C << nl << "for(" << name << "::iterator p = v.begin(); p != v.end(); ++p)";
      C << sb;
      writeStreamMarshalUnmarshalCode(C, type, "(*p)", false);

      //
      // After unmarshaling each element, check that there are still enough bytes left in the stream
      // to unmarshal the remainder of the sequence, and decrement the count of elements
      // yet to be unmarshaled for sequences with variable-length element type (that is, for sequences
      // of classes, structs, dictionaries, sequences, strings, or proxies). This allows us to
      // abort unmarshaling for bogus sequence sizes at the earliest possible moment.
      // (For fixed-length sequences, we don't need to do this because the prediction of how many
      // bytes will be taken up by the sequence is accurate.)
      //
      if (type->isVariableLength()) {
        if (!SequencePtr::dynamicCast(type)) {
          //
          // No need to check for directly nested sequences because, at the start of each
          // sequence, we check anyway.
          //
          //C << nl << "__is->checkSeq();";
        }
        //C << nl << "__is->endElement();";
      }
      C << eb;
      if (type->isVariableLength()) {
        //C << nl << "__is->endSeq(sz);";
      }
      C << eb;
    } else if (!builtin || builtin->kind() == Builtin::KindObject || builtin->kind()
        == Builtin::KindObjectProxy) {
      H << nl << _dllExport << "void __write(OutputStream*, const " << typeStr
          << "*, const " << typeStr << "*, __U__" << name << ");";
      H << nl << _dllExport << "void __read(InputStream*, " << name << "&, __U__"
          << name << ");";

      C << sp << nl << "void" << nl << scope.substr(2)
          << "__write(OutputStream* __os, const " << typeStr << "* begin, const " << typeStr
          << "* end, " << scope << "__U__" << name << ")";
      C << sb;
      C << nl << "UInt size = static_cast< UInt>(end - begin);";
      C << nl << "__os->write_uint(size);";
      C << nl << "for(UInt i = 0; i < size; ++i)";
      C << sb;
      writeMarshalUnmarshalCode(C, type, "begin[i]", true);
      C << eb;
      C << eb;

      C << sp << nl << "void" << nl << scope.substr(2)
          << "__read(InputStream* __is, " << scoped << "& v, " << scope << "__U__"
          << name << ')';
      C << sb;
      C << nl << "UInt sz;";
      C << nl << "__is->read_uint(sz);";
      if (type->isVariableLength()) {
        // Protect against bogus sequence sizes.
        //C << nl << "__is->startSeq(sz, " << type->minWireSize() << ");";
      } else {
        //C << nl << "__is->checkFixedSeq(sz, " << type->minWireSize() << ");";
      }
      C << nl << "v.resize(sz);";
      C << nl << "for(UInt i = 0; i < sz; ++i)";
      C << sb;
      writeMarshalUnmarshalCode(C, type, "v[i]", false);

      //
      // After unmarshaling each element, check that there are still enough bytes left in the stream
      // to unmarshal the remainder of the sequence, and decrement the count of elements
      // yet to be unmarshaled for sequences with variable-length element type (that is, for sequences
      // of classes, structs, dictionaries, sequences, strings, or proxies). This allows us to
      // abort unmarshaling for bogus sequence sizes at the earliest possible moment.
      // (For fixed-length sequences, we don't need to do this because the prediction of how many
      // bytes will be taken up by the sequence is accurate.)
      //
      if (type->isVariableLength()) {
        if (!SequencePtr::dynamicCast(type)) {
          //
          // No need to check for directly nested sequences because, at the start of each
          // sequence, we check anyway.
          //
          //C << nl << "__is->checkSeq();";
        }
        //C << nl << "__is->endElement();";
      }
      C << eb;
      if (type->isVariableLength()) {
        //C << nl << "__is->endSeq(sz);";
      }
      C << eb;
    }
  }
}

void Slice::Gen::TypesVisitor::visitDictionary(const DictionaryPtr& p)
{
  string name = fixKwd(p->name());
  TypePtr keyType = p->keyType();
  TypePtr valueType = p->valueType();
  string ks = typeToString(keyType);
  if (ks[0] == ':') {
    ks.insert(0, " ");
  }
  string vs = typeToString(valueType);
  H << sp << nl << "typedef ::std::map<" << ks << ", " << vs << "> " << name << ';';

  if (!p->isLocal()) {
    string scoped = fixKwd(p->scoped());
    string scope = fixKwd(p->scope());

    H << sp << nl << "class __U__" << name << " { };";
//    H << nl << _dllExport << "void __write(OutputStream*, const " << name << "&);";
    H << nl << _dllExport << "void __write(OutputStream*, const " << name << "&, __U__" << name << ");";
//    H << nl << _dllExport << "void __read(InputStream*, " << name << "&);";
    H << nl << _dllExport << "void __read(InputStream*, " << name << "&, __U__" << name << ");";

    string typeStr = typeToString(keyType) + typeToString(valueType);
    if(visitedContainerTypes.find(typeStr) == visitedContainerTypes.end()) {
      H << nl << _dllExport << "std::string __toString(const " << name << "&);";

      C << sp << nl << "std::string" << nl << scope.substr(2) << "__toString(const " << name << "& arg)";
      C << sb;
      C << nl << "std::string result = \"{\";";
      C << nl << "for(" << name << "::const_iterator it = arg.begin(); it != arg.end(); ++it)";
      C << sb;
      C << nl << "result += \"<\";";
      C << nl << "result += __toString(it->first);";
      C << nl << "result += \",\";";
      C << nl << "result += __toString(it->second);";
      C << nl << "result += \">\";";
      C << nl << "result += \" \";";
      C << eb;
      C << nl << "trimEnd(result);";
      C << nl << "result += \"}\";";
      C << nl << "return result;";
      C << eb;

      visitedContainerTypes.insert(typeStr);
    }

//    C << sp << nl << "void" << nl << scope.substr(2)
//        << "__write(OutputStream* __os, const " << scoped << "& v)";
    C << sp << nl << "void" << nl << scope.substr(2)
        << "__write(OutputStream* __os, const " << scoped << "& v, " << scope << "__U__" << name << ")";

    C << sb;
    C << nl << "__os->write_int(ACE_CDR::Int(v.size()));";
    C << nl << scoped << "::const_iterator p;";
    C << nl << "for(p = v.begin(); p != v.end(); ++p)";
    C << sb;
    writeStreamMarshalUnmarshalCode(C, keyType, "p->first", true);
    writeStreamMarshalUnmarshalCode(C, valueType, "p->second", true);
    C << eb;
    C << eb;

//    C << sp << nl << "void" << nl << scope.substr(2) << "__read(InputStream* __is, " << scoped << "& v)";
    C << sp << nl << "void" << nl << scope.substr(2) << "__read(InputStream* __is, " << scoped
        << "& v, " << scope << "__U__" << name << ")";
    C << sb;
    C << nl << "ACE_CDR::Int sz;";
    C << nl << "__is->read_int(sz);";
    C << nl << "while(sz--)";
    C << sb;
    C << nl << "::std::pair<const " << ks << ", " << vs << "> pair;";
    string pf = string("const_cast<") + ks + "&>(pair.first)";
    writeStreamMarshalUnmarshalCode(C, keyType, pf, false);
    C << nl << scoped << "::iterator __i = v.insert(v.end(), pair);";
    writeStreamMarshalUnmarshalCode(C, valueType, "__i->second", false);
    C << eb;
    C << eb;
  }
}

void Slice::Gen::TypesVisitor::visitEnum(const EnumPtr& p)
{
  string name = fixKwd(p->name());
  EnumeratorList enumerators = p->getEnumerators();
  H << sp << nl << "enum " << name;
  H << sb;
  EnumeratorList::const_iterator en = enumerators.begin();
  while (en != enumerators.end()) {
    H << nl << fixKwd((*en)->name());
    if (++en != enumerators.end()) {
      H << ',';
    }
  }
  H << eb << ';';

  if (!p->isLocal()) {
    string scoped = fixKwd(p->scoped());
    string scope = fixKwd(p->scope());

    size_t sz = enumerators.size();
    assert(sz <= 0x7fffffff); // 64-bit enums are not supported

    H << sp << nl << _dllExport << "void __write(::IceInternal::BasicStream*, " << name << ");";
    H << nl << _dllExport << "void __read(::IceInternal::BasicStream*, " << name << "&);";

    C << sp << nl << "void" << nl << scope.substr(2)
        << "__write(::IceInternal::BasicStream* __os, " << scoped << " v)";
    C << sb;
    if (sz <= 0x7f) {
      C << nl << "__os->write(static_cast< ::Ice::Byte>(v));";
    } else if (sz <= 0x7fff) {
      C << nl << "__os->write(static_cast< ::Ice::Short>(v));";
    } else {
      C << nl << "__os->write(static_cast< ::Ice::Int>(v));";
    }
    C << eb;

    C << sp << nl << "void" << nl << scope.substr(2) << "__read(::IceInternal::BasicStream* __is, "
        << scoped << "& v)";
    C << sb;
    if (sz <= 0x7f) {
      C << nl << "::Ice::Byte val;";
      C << nl << "__is->read(val);";
      C << nl << "v = static_cast< " << scoped << ">(val);";
    } else if (sz <= 0x7fff) {
      C << nl << "::Ice::Short val;";
      C << nl << "__is->read(val);";
      C << nl << "v = static_cast< " << scoped << ">(val);";
    } else {
      C << nl << "::Ice::Int val;";
      C << nl << "__is->read(val);";
      C << nl << "v = static_cast< " << scoped << ">(val);";
    }
    C << eb;
  }
}

void Slice::Gen::TypesVisitor::visitConst(const ConstPtr& p)
{
  H << sp;
  H << nl << "const " << typeToString(p->type()) << " " << fixKwd(p->name()) << " = ";

  BuiltinPtr bp = BuiltinPtr::dynamicCast(p->type());
  if (bp && bp->kind() == Builtin::KindString) {
    //
    // Expand strings into the basic source character set. We can't use isalpha() and the like
    // here because they are sensitive to the current locale.
    //
    static const string basicSourceChars = "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789"
      "_{}[]#()<>%:;.?*+-/^&|~!=,\\\"' ";
    static const set<char> charSet(basicSourceChars.begin(), basicSourceChars.end());

    H << "\""; // Opening "

    const string val = p->value();
    for (string::const_iterator c = val.begin(); c != val.end(); ++c) {
      if (charSet.find(*c) == charSet.end()) {
        unsigned char uc = *c; // char may be signed, so make it positive
        ostringstream s;
        s << "\\"; // Print as octal if not in basic source character set
        s.flags(ios_base::oct);
        s.width(3);
        s.fill('0');
        s << static_cast<unsigned> (uc);
        H << s.str();
      } else {
        H << *c; // Print normally if in basic source character set
      }
    }

    H << "\""; // Closing "
  } else if (bp && bp->kind() == Builtin::KindLong) {
    H << "ICE_INT64(" << p->value() << ")";
  } else {
    EnumPtr ep = EnumPtr::dynamicCast(p->type());
    if (ep) {
      H << fixKwd(p->value());
    } else {
      H << p->value();
    }
  }

  H << ';';
}

void Slice::Gen::TypesVisitor::emitUpcall(const ExceptionPtr& base,
                                          const string& call,
                                          bool isLocal)
{
  C.zeroIndent();
  C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
  C.restoreIndent();
  C << nl << (base ? fixKwd(base->name()) : (isLocal ? "LocalException" : "UserException")) << call;
  C.zeroIndent();
  C << nl << "#else";
  C.restoreIndent();
  C << nl << (base ? fixKwd(base->scoped()) : (isLocal ? "::Ice::LocalException"
      : "::Ice::UserException")) << call;
  C.zeroIndent();
  C << nl << "#endif";
  C.restoreIndent();
}

Slice::Gen::ProxyDeclVisitor::ProxyDeclVisitor(Output& h, Output& c, const string& dllExport) :
  H(h), C(c), _dllExport(dllExport)
{
}

bool Slice::Gen::ProxyDeclVisitor::visitUnitStart(const UnitPtr& p)
{
  if (!p->hasNonLocalClassDecls()) {
    return false;
  }

  H << sp << nl << "namespace IcmProxy" << nl << '{';

  return true;
}

void Slice::Gen::ProxyDeclVisitor::visitUnitEnd(const UnitPtr& p)
{
  H << sp << nl << '}';
}

bool Slice::Gen::ProxyDeclVisitor::visitModuleStart(const ModulePtr& p)
{
  if (!p->hasNonLocalClassDecls()) {
    return false;
  }

  string name = fixKwd(p->name());

  H << sp << nl << "namespace " << name << nl << '{';

  return true;
}

void Slice::Gen::ProxyDeclVisitor::visitModuleEnd(const ModulePtr& p)
{
  H << sp << nl << '}';
}

void Slice::Gen::ProxyDeclVisitor::visitClassDecl(const ClassDeclPtr& p)
{
  if (p->isLocal()) {
    return;
  }

  string name = fixKwd(p->name());

  H << sp << nl << "class " << name << ';';
  //H << nl << _dllExport << "bool operator==(const " << name << "&, const " << name << "&);";
  //H << nl << _dllExport << "bool operator!=(const " << name << "&, const " << name << "&);";
  //H << nl << _dllExport << "bool operator<(const " << name << "&, const " << name << "&);";
}

Slice::Gen::ProxyVisitor::ProxyVisitor(Output& h, Output& c, const string& dllExport) :
  H(h), C(c), _useWstring(false), _dllExport(dllExport)
{
}

bool Slice::Gen::ProxyVisitor::visitUnitStart(const UnitPtr& p)
{
  if (!p->hasNonLocalClassDecls()) {
    return false;
  }

  H << sp << nl << "namespace IcmProxy" << nl << '{';

  return true;
}

void Slice::Gen::ProxyVisitor::visitUnitEnd(const UnitPtr& p)
{
  H << sp << nl << '}';
}

bool Slice::Gen::ProxyVisitor::visitModuleStart(const ModulePtr& p)
{
  if (!p->hasNonLocalClassDecls()) {
    return false;
  }

  string name = fixKwd(p->name());

  H << sp << nl << "namespace " << name << nl << '{';

  return true;
}

void Slice::Gen::ProxyVisitor::visitModuleEnd(const ModulePtr& p)
{
  H << sp << nl << '}';
}

bool Slice::Gen::ProxyVisitor::visitClassDefStart(const ClassDefPtr& p)
{
  if (p->isLocal()) {
    return false;
  }

  string name = fixKwd(p->name());
  string scoped = fixKwd(p->scoped());
  ClassList bases = p->bases();

  H << sp << nl << "class " << _dllExport << name << " : ";
  if (bases.empty()) {
    H << "virtual public IcmProxy::Object";
  } else {
    H.useCurrentPosAsIndent();
    ClassList::const_iterator q = bases.begin();
    while (q != bases.end()) {
      H << "virtual public ::IcmProxy" << fixKwd((*q)->scoped());
      if (++q != bases.end()) {
        H << ',' << nl;
      }
    }
    H.restoreIndent();
  }

  H << sb;
  H.dec();
  H << nl << "public:";
  H.inc();

  return true;
}

void Slice::Gen::ProxyVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
  string scoped = fixKwd(p->scoped());
  string scope = fixKwd(p->scope());

  H << eb << ';';

  string flatName = p->flattenedScope() + p->name() + "_ids";

  StringList ids;
  getIds(p, ids);

  StringList::const_iterator firstIter = ids.begin();
  StringList::const_iterator scopedIter = find(ids.begin(), ids.end(), p->scoped());
  assert(scopedIter != ids.end());
}

void Slice::Gen::ProxyVisitor::visitOperation(const OperationPtr& p)
{
  string name = p->name();
  string scoped = fixKwd(p->scoped());
  string scope = fixKwd(p->scope());

//  static const char* builtinTable[] = { "Byte", "bool", "Short", "Int",
//      "Long", "Float", "Double", "::std::string", "::Ice::ObjectPtr",
//      "::IcmProxy::Object*", "::Ice::LocalObjectPtr" };

  TypePtr ret = p->returnType();
  string retS = returnTypeToString(ret, p->getMetaData());
  string retString;
  string guardString;
  if (retS == "::std::string") {
    retString = "return \"\";";
    guardString = "ACE_GUARD_RETURN(ThreadMutex, guard, mutex, \"\");";
  } else if ((retS == "Short") || (retS == "Int") || (retS == "Long") || (retS == "Float") || (retS == "Double")) {
  	retString = "return -1;";
  	guardString = "ACE_GUARD_RETURN(ThreadMutex, guard, mutex, -1);";
  } else if (retS == "void") {
    retString = "return;";
    guardString = "ACE_GUARD(ThreadMutex, guard, mutex);";
  } else {
  	retString = "return " + retS + "();";
  	guardString = "ACE_GUARD_RETURN(ThreadMutex, guard, mutex, " + retS + "());";
  }

  ContainerPtr container = p->container();
  ClassDefPtr cl = ClassDefPtr::dynamicCast(container);

  vector<string> params;
  vector<string> paramsDecl;
  vector<string> paramsName;
  vector<string> args;

  vector<string> paramsAMI;
  vector<string> paramsDeclAMI;
  vector<string> argsAMI;

  ParamDeclList inParams;
  ParamDeclList outParams;
  ParamDeclList paramList = p->parameters();
  for (ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q) {
    string paramName = fixKwd((*q)->name());
    StringList metaData = (*q)->getMetaData();

    string typeString;
    if ((*q)->isOutParam()) {
      outParams.push_back(*q);
      typeString = outputTypeToString((*q)->type(), _useWstring, metaData);
    } else {
      inParams.push_back(*q);
      typeString = inputTypeToString((*q)->type(), _useWstring, metaData);
    }

    params.push_back(typeString);
    paramsDecl.push_back(typeString + ' ' + paramName);
    paramsName.push_back(paramName);
    args.push_back(paramName);

    if(!(*q)->isOutParam())
    {
        string inputTypeString = inputTypeToString((*q)->type(), _useWstring, metaData);

        paramsAMI.push_back(inputTypeString);
        paramsDeclAMI.push_back(inputTypeString + ' ' + paramName);
        argsAMI.push_back(paramName);
    }
  }

  paramsName.push_back("__outS");

  string thisPointer = fixKwd(scope.substr(0, scope.size() - 2)) + "*";

  H << sp;
  H << nl << retS << ' ' << fixKwd(name) << spar << paramsDecl << epar << ";";

  C << sp << nl << retS << nl << "IcmProxy" << scoped << spar << paramsDecl << epar;

  C << sb;

  C << nl << guardString;
  C << nl << "bool usingCachedTransport = (this->transport() != 0);";
  C << nl << "static const char* __operation(\"" << p->name() << "\");";
  C << nl << "Reference* ref = this->getReference ();";
  C << nl << "TwowayInvocation _invocation (ref, __operation, ref->communicator (), ref->getMaxWaitTime());";
  C << nl << "int ok = _invocation.start (this->transport ());";
  C << nl << "if (ok != 0)";
  C << "{";
  C.inc();
  C << nl << "IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );";
  C << nl << retString;
  C.dec();
  C << nl << "}";

  C << nl << "ok = _invocation.prepareHeader (1);";
  C << nl << "if (ok != 0)";
  C << "{";
  C.inc();
  C << nl << "IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );";
  C << nl << retString;
  C.dec();
  C << nl << "}";

  if (!inParams.empty()) {
    C << nl << "OutputStream* __os = _invocation.outStream();";
    writeMarshalCode(C, inParams, 0, StringList(), "__os", true);
  }

  C << nl << "ok = _invocation.invoke();";

////////////////////////////
  C << nl << "if(ok != 0 && usingCachedTransport) " << sb;
  C << nl << "ICC_ERROR(\"Cached transport error, retry a new connection!\");";
  C << nl << "this->transport(0);";
  C << nl << "TwowayInvocation _invocation (ref, __operation, ref->communicator (), ref->getMaxWaitTime());";
  C << nl << "ok = _invocation.start (this->transport ());";
  C << nl << "if (ok != 0) " << sb;
  C << nl << "IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );";
  C << nl << retString << eb;
  C << nl << "ok = _invocation.prepareHeader (1);";
  C << nl << "if (ok != 0) " << sb;
  C << nl << "IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );";
  C << nl << retString << eb;
  C << nl << "OutputStream* __os = _invocation.outStream();";
  writeMarshalCode(C, inParams, 0, StringList(), "__os", true);
  C << nl << "ok = _invocation.invoke();";
//  C << nl << eb;

  C << nl << "if (ok != 0)";
  C << "{";
  C.inc();
  C << nl << "IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );";
  C << nl << "this->transport(0);";
  C << nl << retString;
  C.dec();
  C << nl << "}";

  if(ret)
    C << nl << "InputStream* __is = _invocation.inpStream();";

  writeAllocateCode(C, ParamDeclList(), ret, p->getMetaData());
  writeUnmarshalCode(C, outParams, ret, p->getMetaData(), "__is");
  if (ret) {
  C << nl << "IcmProxy::setCallErrno( 0 );";
    C << nl << "return __ret;";
  }
  C <<  eb;
/////////////////////////////

  C << nl << "if (ok != 0)";
  C << "{";
  C.inc();
  C << nl << "IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );";
  C << nl << "this->transport(0);";
  C << nl << retString;
  C.dec();
  C << nl << "}";

  if(ret)
    C << nl << "InputStream* __is = _invocation.inpStream();";

  //
  // Generate a catch block for each legal user exception. This is necessary
  // to prevent an "impossible" user exception to be thrown if client and
  // and server use different exception specifications for an operation. For
  // example:
  //
  // Client compiled with:
  // exception A {};
  // exception B {};
  // interface I {
  //     void op() throws A;
  // };
  //
  // Server compiled with:
  // exception A {};
  // exception B {};
  // interface I {
  //     void op() throws B; // Differs from client
  // };
  //
  // We need the catch blocks so, if the server throws B from op(), the
  // client receives UnknownUserException instead of B.
  //
  ExceptionList throws = p->throws();
  throws.sort();
  throws.unique();
#if defined(__SUNPRO_CC)
  throws.sort(derivedToBaseCompare);
#else
  throws.sort(Slice::DerivedToBaseCompare());
#endif
  for (ExceptionList::const_iterator i = throws.begin(); i != throws.end(); ++i) {
    C << nl << "catch(const " << fixKwd((*i)->scoped()) << "&)";
    C << sb;
    C << nl << "throw;";
    C << eb;
  }
  //C << nl << "catch(const ::Ice::UserException& __ex)";
  //C << sb;
  //C << nl << "::Ice::UnknownUserException __uex(__FILE__, __LINE__);";
  //C << nl << "__uex.unknown = __ex.ice_name();";
  //C << nl << "throw __uex;";
  //C << eb;
  //C << eb;

  writeAllocateCode(C, ParamDeclList(), ret, p->getMetaData());
  writeUnmarshalCode(C, outParams, ret, p->getMetaData(), "__is");
  if (ret) {
	C << nl << "IcmProxy::setCallErrno( 0 );";
    C << nl << "return __ret;";
  }
  C << eb;


  if(cl->hasMetaData("ami") || p->hasMetaData("ami"))
  {
    string classNameAMI = "AMI_" + cl->name();
    string classScope = fixKwd(cl->scope());
    string classScopedAMI = classScope + classNameAMI;

    H << nl << "void " << name << "_async" << spar << (classScopedAMI + '_' + name + "*")
      << paramsAMI << epar << ';';
    //H << nl << "void " << name << "_async" << spar << ("const " + classScopedAMI + '_' + name + "Ptr&")
      //<< paramsAMI << "const ::Ice::Context&" << epar << ';';

    C << sp << nl << "void" << nl << "IcmProxy" << scope << name << "_async" << spar
      << (classScopedAMI + '_' + name + "* __cb") << paramsDeclAMI << epar;
    C << sb;

    //C << nl << name << "_async" << spar << "__cb" << argsAMI << "__defaultContext()" << epar << ';';
    C << nl << "static const char* __operation(\"" << p->name() << "\");";
    C << nl << "Reference* ref = this->getReference ();";
    C << nl << "TwowayAsynchInvocation _invocation (ref, __operation, ref->communicator (), __cb, ref->getMaxWaitTime());";
    C << nl << "int ok = _invocation.start (this->transport ());";
    C << nl << "if (ok != 0)" << sb;
    C << nl << "IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );";
    C << nl << "return;" << eb;
    C << nl << "ok = _invocation.prepareHeader (1);";
    C << nl << "if (ok != 0)" << sb;
    C << nl << "IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );";
    C << nl << "return;" << eb;
    if (!inParams.empty()) {
      //C << nl << "try";
      //C << sb;
      C << nl << "OutputStream* __os = _invocation.outStream();";
      writeMarshalCode(C, inParams, 0, StringList(), "__os", true);
      //C << eb;
      //C << nl << "catch(const ::Ice::LocalException& __ex)";
      //C << sb;
      //C << nl << "__outS.abort(__ex);";
      //C << eb;
    }
    C << nl << "int  _invokeStatus = _invocation.invoke();";
    C << nl << "if(_invokeStatus != 0)" << sb;
    C << nl << "IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );";
    C << nl << "this->transport(0);" << eb;

    C << eb;

    /*
    C << sp << nl << "void" << nl << "IcmProxy" << scope << name << "_async" << spar
      << ("const " + classScopedAMI + '_' + name + "Ptr& __cb") << paramsDeclAMI << "const ::Ice::Context& __ctx"
      << epar;
    C << sb;
    C << nl << "__cb->__invoke" << spar << "this" << argsAMI << "__ctx" << epar << ';';
    C << eb;
    */
  }
}

Slice::Gen::ObjectDeclVisitor::ObjectDeclVisitor(Output& h, Output& c, const string& dllExport) :
  H(h), C(c), _dllExport(dllExport)
{
}

bool Slice::Gen::ObjectDeclVisitor::visitModuleStart(const ModulePtr& p)
{
  if (!p->hasClassDecls()) {
    return false;
  }

  string name = fixKwd(p->name());

  H << sp << nl << "namespace " << name << nl << '{';

  return true;
}

void Slice::Gen::ObjectDeclVisitor::visitModuleEnd(const ModulePtr& p)
{
  H << sp << nl << '}';
}

void Slice::Gen::ObjectDeclVisitor::visitClassDecl(const ClassDeclPtr& p)
{
  string name = fixKwd(p->name());

  if (!p->isLocal()) {
    H << sp << nl << "#ifndef ICEE_PURE_CLIENT";
  }

  H << sp << nl << "class " << name << ';';
  H << nl << _dllExport << "bool operator==(const " << name << "&, const " << name << "&);";
  H << nl << _dllExport << "bool operator!=(const " << name << "&, const " << name << "&);";
  H << nl << _dllExport << "bool operator<(const " << name << "&, const " << name << "&);";

  if (!p->isLocal()) {
    H << sp << nl << "#endif // ICEE_PURE_CLIENT";
  }
}

Slice::Gen::ObjectVisitor::ObjectVisitor(Output& h, Output& c, const string& dllExport) :
  H(h), C(c), _useWstring(false), _dllExport(dllExport)
{
}

bool Slice::Gen::ObjectVisitor::visitModuleStart(const ModulePtr& p)
{
  if (!p->hasClassDefs()) {
    return false;
  }

  string name = fixKwd(p->name());

  H << sp << nl << "namespace " << name << nl << '{';

  return true;
}

void Slice::Gen::ObjectVisitor::visitModuleEnd(const ModulePtr& p)
{
  H << sp;
  H << nl << '}';
}

bool Slice::Gen::ObjectVisitor::visitClassDefStart(const ClassDefPtr& p)
{
  string name = fixKwd(p->name());
  string scoped = fixKwd(p->scoped());
  ClassList bases = p->bases();
  ClassDefPtr base;
  if (!bases.empty() && !bases.front()->isInterface()) {
    base = bases.front();
  }
  DataMemberList dataMembers = p->dataMembers();
  DataMemberList allDataMembers = p->allDataMembers();

  if (!p->isLocal()) {
    //H << sp << nl << "#ifndef ICEE_PURE_CLIENT";
  }

  H << sp << nl << "class " << _dllExport << name << " : ";
  H.useCurrentPosAsIndent();
  if (bases.empty()) {
    if (p->isLocal()) {
      //H << "virtual public ::Ice::LocalObject";
    } else {
      H << "virtual public Object";
    }
  } else {
    ClassList::const_iterator q = bases.begin();
    while (q != bases.end()) {
      H << "virtual public " << fixKwd((*q)->scoped());
      if (++q != bases.end()) {
        H << ',' << nl;
      }
    }
  }
  H.restoreIndent();
  H << sb;
  H.dec();
  H << nl << "public:" << sp;
  H.inc();

  vector<string> params;
  vector<string> allTypes;
  vector<string> allParamDecls;
  DataMemberList::const_iterator q;

  for (q = dataMembers.begin(); q != dataMembers.end(); ++q) {
    params.push_back(fixKwd((*q)->name()));
  }

  for (q = allDataMembers.begin(); q != allDataMembers.end(); ++q) {
    string typeName = inputTypeToString((*q)->type(), _useWstring, (*q)->getMetaData());
    allTypes.push_back(typeName);
    allParamDecls.push_back(typeName + " __ice_" + (*q)->name());
  }

  if (!p->isInterface()) {
    H << nl << name << "() {}";
    if (!allParamDecls.empty()) {
      H << nl;
      if (allParamDecls.size() == 1) {
        H << "explicit ";
      }
      H << name << spar << allTypes << epar << ';';
    }

    emitOneShotConstructor(p);


  }

  if (!p->isLocal()) {
    StringList ids;
    getIds(p, ids);

    StringList::const_iterator firstIter = ids.begin();
    StringList::const_iterator scopedIter = find(ids.begin(), ids.end(), p->scoped());
    assert(scopedIter != ids.end());
//    StringList::difference_type scopedPos = ice_distance(firstIter, scopedIter);

    StringList::const_iterator q;
/*
    H << sp;
    H << nl << "virtual bool ice_isA"
        << "(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;";
    H << nl << "virtual ::std::vector< ::std::string> ice_ids"
        << "(const ::Ice::Current& = ::Ice::Current()) const;";
    H << nl
        << "virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;";
    H << nl << "static const ::std::string& ice_staticId();";
    if (!dataMembers.empty()) {
      H << sp;
    }
*/
    string flatName = p->flattenedScope() + p->name() + "_ids";

    C << sp;
    C << nl << "static const ::std::string " << flatName << '[' << ids.size() << "] =";
    C << sb;
    q = ids.begin();
    while (q != ids.end()) {
      C << nl << '"' << *q << '"';
      if (++q != ids.end()) {
        C << ',';
      }
    }
    C << eb << ';';

    /*
    C << sp << nl << "#ifndef ICEE_PURE_CLIENT";

    C << sp;
    C << nl << "bool" << nl << fixKwd(p->scoped()).substr(2)
        << "::ice_isA(const ::std::string& _s, const ::Ice::Current&) const";
    C << sb;
    C << nl << "return ::std::binary_search(" << flatName << ", " << flatName << " + "
        << ids.size() << ", _s);";
    C << eb;

    C << sp;
    C << nl << "::std::vector< ::std::string>" << nl << fixKwd(p->scoped()).substr(2)
        << "::ice_ids(const ::Ice::Current&) const";
    C << sb;
    C << nl << "return ::std::vector< ::std::string>(&" << flatName << "[0], &" << flatName << '['
        << ids.size() << "]);";
    C << eb;

    C << sp;
    C << nl << "const ::std::string&" << nl << fixKwd(p->scoped()).substr(2)
        << "::ice_id(const ::Ice::Current&) const";
    C << sb;
    C << nl << "return " << flatName << '[' << scopedPos << "];";
    C << eb;

    C << sp;
    C << nl << "const ::std::string&" << nl << fixKwd(p->scoped()).substr(2) << "::ice_staticId()";
    C << sb;
    C << nl << "return " << flatName << '[' << scopedPos << "];";
    C << eb;
    */
  }

  return true;
}

void Slice::Gen::ObjectVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
  string scoped = fixKwd(p->scoped());
  string scope = fixKwd(p->scope());

  if (!p->isLocal()) {
    ClassList bases = p->bases();
    ClassDefPtr base;
    if (!bases.empty() && !bases.front()->isInterface()) {
      base = bases.front();
    }

    OperationList allOps = p->allOperations();
    if (!allOps.empty()) {
      StringList allOpNames;
#if defined(__IBMCPP__) && defined(NDEBUG)
      //
      // See comment for transform above
      //
      transform(allOps.begin(), allOps.end(), back_inserter(allOpNames),
          ::IceUtil::constMemFun<string,Operation>(&Contained::name));
#else
      transform(allOps.begin(), allOps.end(), back_inserter(allOpNames), ::IceUtil::constMemFun(&Contained::name));
#endif
      //allOpNames.push_back("ice_id");
      //allOpNames.push_back("ice_ids");
      //allOpNames.push_back("ice_isA");
      //allOpNames.push_back("ice_ping");
      allOpNames.sort();
      allOpNames.unique();

      StringList::const_iterator q;

      H << sp;
      H << nl
          << "virtual DispatchStatus __dispatch(ServerRequest& in);";

      string flatName = p->flattenedScope() + p->name() + "_all";
      C << sp;
      C << nl << "static ::std::string " << flatName << "[] =";
      C << sb;
      q = allOpNames.begin();
      while (q != allOpNames.end()) {
        C << nl << '"' << *q << '"';
        if (++q != allOpNames.end()) {
          C << ',';
        }
      }
      C << eb << ';';
      C << sp;
      C << nl << "DispatchStatus" << nl << scoped.substr(2)
          << "::__dispatch(ServerRequest& in)";
      C << sb;

      C << nl << "::std::pair< ::std::string*, ::std::string*> r = " << "::std::equal_range("
          << flatName << ", " << flatName << " + " << allOpNames.size() << ", in.operation());";
      C << nl << "if(r.first == r.second)";
      C << sb;
      C << nl << "return DispatchOperationNotExist;";
      C << eb;
      C << sp;
      C << nl << "switch(r.first - " << flatName << ')';
      C << sb;
      int i = 0;
      for (q = allOpNames.begin(); q != allOpNames.end(); ++q) {
        C << nl << "case " << i++ << ':';
        C << sb;
        C << nl << "return ___" << *q << "(in);";
        C << eb;
      }
      C << eb;
      C << sp;
      C << nl << "assert(false);";
      C << nl << "return DispatchOperationNotExist;";
      C << eb;
    }

  }

  H << eb << ';';

  if (!p->isLocal()) {
    //H << sp << nl << "#endif // ICEE_PURE_CLIENT";
  }

  /*
  if (p->isLocal()) {
    C << sp;
    C << nl << "bool" << nl << scope.substr(2) << "operator==(const " << scoped << "& l, const "
        << scoped << "& r)";
    C << sb;
    C << nl
        << "return static_cast<const ::Ice::LocalObject&>(l) == static_cast<const ::Ice::LocalObject&>(r);";
    C << eb;
    C << sp;
    C << nl << "bool" << nl << scope.substr(2) << "operator!=(const " << scoped << "& l, const "
        << scoped << "& r)";
    C << sb;
    C << nl
        << "return static_cast<const ::Ice::LocalObject&>(l) != static_cast<const ::Ice::LocalObject&>(r);";
    C << eb;
    C << sp;
    C << nl << "bool" << nl << scope.substr(2) << "operator<(const " << scoped << "& l, const "
        << scoped << "& r)";
    C << sb;
    C << nl
        << "return static_cast<const ::Ice::LocalObject&>(l) < static_cast<const ::Ice::LocalObject&>(r);";
    C << eb;
  } else {
    string name = p->name();

    C << sp;
    C << nl << "bool" << nl << scope.substr(2) << "operator==(const " << scoped << "& l, const "
        << scoped << "& r)";
    C << sb;
    C << nl
        << "return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);";
    C << eb;
    C << sp;
    C << nl << "bool" << nl << scope.substr(2) << "operator!=(const " << scoped << "& l, const "
        << scoped << "& r)";
    C << sb;
    C << nl
        << "return static_cast<const ::Ice::Object&>(l) != static_cast<const ::Ice::Object&>(r);";
    C << eb;
    C << sp;
    C << nl << "bool" << nl << scope.substr(2) << "operator<(const " << scoped << "& l, const "
        << scoped << "& r)";
    C << sb;
    C << nl
        << "return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);";
    C << eb;

    C << sp << nl << "#endif // ICEE_PURE_CLIENT";
  }
  */
}

bool Slice::Gen::ObjectVisitor::visitExceptionStart(const ExceptionPtr&)
{
  return false;
}

bool Slice::Gen::ObjectVisitor::visitStructStart(const StructPtr&)
{
  return false;
}

void Slice::Gen::ObjectVisitor::visitOperation(const OperationPtr& p)
{
  string name = p->name();
  string scoped = fixKwd(p->scoped());
  string scope = fixKwd(p->scope());

  TypePtr ret = p->returnType();
  string retS = returnTypeToString(ret, p->getMetaData());

  string params = "(";
  string args = "(";

  ContainerPtr container = p->container();
  ClassDefPtr cl = ClassDefPtr::dynamicCast(container);

  string classNameAMD = "AMH_" + cl->name();
  string classScope = fixKwd(cl->scope());
  string classScopedAMD = classScope + classNameAMD;

  string paramsAMD = "(" + classScopedAMD + '_' + name + "*";
  string argsAMD = "(__cb";

  ParamDeclList inParams;
  ParamDeclList outParams;
  ParamDeclList paramList = p->parameters();
  for (ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q) {

    if (q != paramList.begin()) {
      params += ", ";
      args += ", ";
    }

    string paramName = fixKwd((*q)->name());
    TypePtr type = (*q)->type();
    bool isOutParam = (*q)->isOutParam();
    StringList metaData = (*q)->getMetaData();

    string typeString;
    if (isOutParam) {
      outParams.push_back(*q);
      typeString = outputTypeToString(type, _useWstring, metaData);
    } else {
      inParams.push_back(*q);
      typeString = inputTypeToString(type, _useWstring, metaData);
    }

    params += typeString;
    args += paramName;

    if(!isOutParam)
    {
      paramsAMD += ", " + typeString;
      argsAMD += ", " + paramName;
    }
  }

  params += ')';
  args += ')';

  paramsAMD += ")";
  argsAMD += ")";

  bool nonmutating = p->mode() == Operation::Nonmutating;
    bool amd = !cl->isLocal() && (cl->hasMetaData("amd") || p->hasMetaData("amd"));

  H << sp;
  if(!amd)
  {
    H << nl << "virtual " << retS << ' ' << fixKwd(name) << params << (nonmutating ? " const" : "")
        << " = 0;";
  }
  else
  {
	    H << nl << "virtual void " << name << "_async" << paramsAMD
	      << (nonmutating ? " const" : "") << " = 0;";
  }

  if (!cl->isLocal()) {
    H << nl << "DispatchStatus ___" << name
        << "(ServerRequest&)" << (nonmutating ? " const" : "")
        << ';';

    C << sp;
    C << nl << "DispatchStatus" << nl << scope.substr(2) << "___" << name
        << "(ServerRequest& __inS)"
        << (nonmutating ? " const" : "");
    C << sb;

    if(!amd)
    {
      ExceptionList throws = p->throws();
      throws.sort();
      throws.unique();

      //
      // Arrange exceptions into most-derived to least-derived order. If we don't
      // do this, a base exception handler can appear before a derived exception
      // handler, causing compiler warnings and resulting in the base exception
      // being marshaled instead of the derived exception.
      //

  #if defined(__SUNPRO_CC)
      throws.sort(derivedToBaseCompare);
  #else
      throws.sort(Slice::DerivedToBaseCompare());
  #endif

      //C << nl << "__checkMode(" << operationModeToString(p->mode()) << ", __current.mode);";

      if (!inParams.empty()) {
        C << nl << "InputStream* __is = __inS.incoming();";
      }
      if (ret || !outParams.empty() || !throws.empty()) {
        C << nl << "OutputStream* __os = __inS.outgoing();";
      }

      writeAllocateCode(C, inParams, 0, StringList(), true);
      writeUnmarshalCode(C, inParams, 0, StringList(), "__is", true);
      writeAllocateCode(C, outParams, 0, StringList());
      if (!throws.empty()) {
        C << nl << "try";
        C << sb;
      }
      C << nl;
      if (ret) {
        C << retS << " __ret = ";
      }
      C << fixKwd(name) << args << ';';
      C << nl << "__inS.initReply ();";
      writeMarshalCode(C, outParams, ret, p->getMetaData(), "__os");
      if (!throws.empty()) {
        C << eb;
        ExceptionList::const_iterator r;
        for (r = throws.begin(); r != throws.end(); ++r) {
          C << nl << "catch(const " << fixKwd((*r)->scoped()) << "& __ex)";
          C << sb;
          C << nl << "__os->write(__ex);";
          C << nl << "return DispatchUserException;";
          C << eb;
        }
      }
      C << nl << "return DispatchOK;";
    }
    else
    {
        //C << nl << "__checkMode(" << operationModeToString(p->mode()) << ", __current.mode);";

        if(!inParams.empty())
        {
	        C << nl << "InputStream* __is = __inS.incoming();";
          C << nl << "OutputStream* __os = __inS.outgoing();";
        }
        writeAllocateCode(C, inParams, 0, StringList(),  true);
        writeUnmarshalCode(C, inParams, 0, StringList(), "__is", true);
        if(p->sendsClasses())
        {
	        C << nl << "__is->readPendingObjects();";
        }
        C << nl << classScopedAMD << '_' << name << "* __cb = new IcmAsync" << classScopedAMD << '_' << name
          << "(__inS);";

        C << nl << "__inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));";
        C << nl << name << "_async" << argsAMD << ';';
        C << nl << "return DispatchAsync;";
    }
    C << eb;
  }
}

void Slice::Gen::ObjectVisitor::visitDataMember(const DataMemberPtr& p)
{
  string name = fixKwd(p->name());
  string s = typeToString(p->type(), p->getMetaData());
  H << nl << s << ' ' << name << ';';
}

bool Slice::Gen::ObjectVisitor::emitVirtualBaseInitializers(const ClassDefPtr& p)
{
  DataMemberList allDataMembers = p->allDataMembers();
  if (allDataMembers.empty()) {
    return false;
  }

  ClassList bases = p->bases();
  if (!bases.empty() && !bases.front()->isInterface()) {
    if (emitVirtualBaseInitializers(bases.front())) {
      C << ',';
    }
  }

  string upcall = "(";
  DataMemberList::const_iterator q;
  for (q = allDataMembers.begin(); q != allDataMembers.end(); ++q) {
    if (q != allDataMembers.begin()) {
      upcall += ", ";
    }
    upcall += "__ice_" + (*q)->name();
  }
  upcall += ")";

  C << nl << fixKwd(p->name()) << upcall;

  return true;
}

void Slice::Gen::ObjectVisitor::emitOneShotConstructor(const ClassDefPtr& p)
{
  DataMemberList allDataMembers = p->allDataMembers();
  DataMemberList::const_iterator q;

  vector<string> allParamDecls;

  for (q = allDataMembers.begin(); q != allDataMembers.end(); ++q) {
    string typeName = inputTypeToString((*q)->type(), _useWstring, (*q)->getMetaData());
    allParamDecls.push_back(typeName + " __ice_" + (*q)->name());
  }

  if (!allDataMembers.empty()) {
    C << sp << nl << p->scoped().substr(2) << "::" << fixKwd(p->name()) << spar << allParamDecls
        << epar << " :";
    C.inc();

    DataMemberList dataMembers = p->dataMembers();

    ClassList bases = p->bases();
    ClassDefPtr base;
    if (!bases.empty() && !bases.front()->isInterface()) {
      if (emitVirtualBaseInitializers(bases.front())) {
        if (!dataMembers.empty()) {
          C << ',';
        }
      }
    }

    if (!dataMembers.empty()) {
      C << nl;
    }
    for (q = dataMembers.begin(); q != dataMembers.end(); ++q) {
      if (q != dataMembers.begin()) {
        C << ',' << nl;
      }
      string memberName = fixKwd((*q)->name());
      C << memberName << '(' << "__ice_" << memberName << ')';
    }

    C.dec();
    C << sb;
    C << eb;
  }
}

void Slice::Gen::ObjectVisitor::emitUpcall(const ClassDefPtr& base, const string& call)
{
  C.zeroIndent();
  C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
  C.restoreIndent();
  C << nl << (base ? fixKwd(base->name()) : "Object") << call;
  C.zeroIndent();
  C << nl << "#else";
  C.restoreIndent();
  C << nl << (base ? fixKwd(base->scoped()) : "::Ice::Object") << call;
  C.zeroIndent();
  C << nl << "#endif";
  C.restoreIndent();
}

Slice::Gen::IceInternalVisitor::IceInternalVisitor(Output& h, Output& c, const string& dllExport) :
  H(h), C(c), _dllExport(dllExport)
{
}

bool Slice::Gen::IceInternalVisitor::visitUnitStart(const UnitPtr& p)
{
  if (!p->hasClassDecls()) {
    return false;
  }

  H << sp;
  H << nl << "namespace IceInternal" << nl << '{';

  return true;
}

void Slice::Gen::IceInternalVisitor::visitUnitEnd(const UnitPtr& p)
{
  H << sp;
  H << nl << '}';
}

void Slice::Gen::IceInternalVisitor::visitClassDecl(const ClassDeclPtr& p)
{
  string scoped = fixKwd(p->scoped());

  if (!p->isLocal()) {
    H << sp << nl << "#ifndef ICEE_PURE_CLIENT";
  }

  H << sp;
  H << nl << _dllExport << "void incRef(" << scoped << "*);";
  H << nl << _dllExport << "void decRef(" << scoped << "*);";

  if (!p->isLocal()) {
    H << sp << nl << "#endif // ICEE_PURE_CLIENT";

    H << sp;
    H << nl << _dllExport << "void incRef(::IcmProxy" << scoped << "*);";
    H << nl << _dllExport << "void decRef(::IcmProxy" << scoped << "*);";
  }
}

bool Slice::Gen::IceInternalVisitor::visitClassDefStart(const ClassDefPtr& p)
{
  string scoped = fixKwd(p->scoped());

  if (!p->isLocal()) {
    C << sp << nl << "#ifndef ICEE_PURE_CLIENT";
  }

  C << sp;
  C << nl << "void" << nl << "IceInternal::incRef(" << scoped << "* p)";
  C << sb;
  C << nl << "p->__incRef();";
  C << eb;

  C << sp;
  C << nl << "void" << nl << "IceInternal::decRef(" << scoped << "* p)";
  C << sb;
  C << nl << "p->__decRef();";
  C << eb;

  if (!p->isLocal()) {
    C << sp << nl << "#endif // ICEE_PURE_CLIENT";

    C << sp;
    C << nl << "void" << nl << "IceInternal::incRef(::IcmProxy" << scoped << "* p)";
    C << sb;
    C << nl << "p->__incRef();";
    C << eb;

    C << sp;
    C << nl << "void" << nl << "IceInternal::decRef(::IcmProxy" << scoped << "* p)";
    C << sb;
    C << nl << "p->__decRef();";
    C << eb;
  }

  return true;
}

Slice::Gen::HandleVisitor::HandleVisitor(Output& h, Output& c, const string& dllExport) :
  H(h), C(c), _dllExport(dllExport)
{
}

bool Slice::Gen::HandleVisitor::visitModuleStart(const ModulePtr& p)
{
  if (!p->hasClassDecls()) {
    return false;
  }

  string name = fixKwd(p->name());

  H << sp;
  H << nl << "namespace " << name << nl << '{';

  return true;
}

void Slice::Gen::HandleVisitor::visitModuleEnd(const ModulePtr& p)
{
  H << sp;
  H << nl << '}';
}

void Slice::Gen::HandleVisitor::visitClassDecl(const ClassDeclPtr& p)
{
  string name = p->name();
  string scoped = fixKwd(p->scoped());

  if (!p->isLocal()) {
    //H << sp << nl << "#ifndef ICEE_PURE_CLIENT";
  }

  //H << sp;
  //H << nl << "typedef ::IceInternal::Handle< " << scoped << "> " << name << "Ptr;";

  if (!p->isLocal()) {
    //H << sp << nl << "#endif // ICEE_PURE_CLIENT" << sp;

    //H << nl << "typedef ::IceInternal::ProxyHandle< ::IcmProxy" << scoped << "> " << name << "Prx;";

    H << sp;
    H << nl << _dllExport << "void __write(OutputStream*, IcmProxy" << scoped << "* v);";
    H << nl << _dllExport << "void __read(InputStream*, IcmProxy" << scoped << "* &v);";
  }
}

bool Slice::Gen::HandleVisitor::visitClassDefStart(const ClassDefPtr& p)
{
  if (!p->isLocal()) {
    string name = p->name();
    string scoped = fixKwd(p->scoped());
    string scope = fixKwd(p->scope());

    string factory;
    string type;
    if (!p->isAbstract()) {
      type = scoped + "::ice_staticId()";
      factory = scoped + "::ice_factory()";
    } else {
      type = "\"\"";
      factory = "0";
    }

    C << sp;
    C << nl << "void" << nl << scope.substr(2)
        << "__write(OutputStream* __os, IcmProxy" << scope << name << "* v)";
    C << sb;
    C << nl << "__os->write_obj(v);";
    C << eb;

    C << sp;
    C << nl << "void" << nl << scope.substr(2) << "__read(InputStream* __is, IcmProxy"
        << scope << name << "* &v)";
    C << sb;
    C << nl << "::IcmProxy::Object* proxy;";
    C << nl << "__is->read_obj(proxy);";
    C << nl << "if(!proxy)";
    C << sb;
    C << nl << "v = 0;";
    C << eb;
    C << nl << "else";
    C << sb;
    C << nl << "v = new ::IcmProxy" << scoped << ';';
    C << nl << "v->copyFrom(proxy);";
    C << eb;
    C << eb;
  }

  return true;
}

Slice::Gen::ImplVisitor::ImplVisitor(Output& h, Output& c, const string& dllExport) :
  H(h), C(c), _useWstring(false), _dllExport(dllExport)
{
}

void Slice::Gen::ImplVisitor::writeDecl(Output& out, const string& name, const TypePtr& type)
{
  out << nl << typeToString(type) << ' ' << name;

  BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
  if (builtin) {
    switch (builtin->kind())
    {
    case Builtin::KindBool: {
      out << " = false";
      break;
    }
    case Builtin::KindByte:
    case Builtin::KindShort:
    case Builtin::KindInt:
    case Builtin::KindLong: {
      out << " = 0";
      break;
    }
    case Builtin::KindFloat:
    case Builtin::KindDouble: {
      out << " = 0.0";
      break;
    }
    case Builtin::KindObject:
    case Builtin::KindString:
    case Builtin::KindObjectProxy:
    case Builtin::KindLocalObject: {
      break;
    }
    }
  }

  EnumPtr en = EnumPtr::dynamicCast(type);
  if (en) {
    EnumeratorList enumerators = en->getEnumerators();
    out << " = " << fixKwd(en->scope()) << fixKwd(enumerators.front()->name());
  }

  out << ';';
}

void Slice::Gen::ImplVisitor::writeReturn(Output& out,
                                          const TypePtr& type,
                                          const StringList& metaData)
{
  BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
  if (builtin) {
    switch (builtin->kind())
    {
    case Builtin::KindBool: {
      out << nl << "return false;";
      break;
    }
    case Builtin::KindByte:
    case Builtin::KindShort:
    case Builtin::KindInt:
    case Builtin::KindLong: {
      out << nl << "return 0;";
      break;
    }
    case Builtin::KindFloat:
    case Builtin::KindDouble: {
      out << nl << "return 0.0;";
      break;
    }
    case Builtin::KindString: {
      out << nl << "return ::std::string();";
      break;
    }
    case Builtin::KindObject:
    case Builtin::KindObjectProxy:
    case Builtin::KindLocalObject: {
      out << nl << "return 0;";
      break;
    }
    }
  } else {
    ProxyPtr prx = ProxyPtr::dynamicCast(type);
    if (prx) {
      out << nl << "return 0;";
    } else {
      ClassDeclPtr cl = ClassDeclPtr::dynamicCast(type);
      if (cl) {
        out << nl << "return 0;";
      } else {
        StructPtr st = StructPtr::dynamicCast(type);
        if (st) {
          out << nl << "return " << fixKwd(st->scoped()) << "();";
        } else {
          EnumPtr en = EnumPtr::dynamicCast(type);
          if (en) {
            EnumeratorList enumerators = en->getEnumerators();
            out << nl << "return " << fixKwd(en->scope()) << fixKwd(enumerators.front()->name())
                << ';';
          } else {
            SequencePtr seq = SequencePtr::dynamicCast(type);
            if (seq) {
              out << nl << "return " << typeToString(seq, metaData) << "();";
            } else {
              DictionaryPtr dict = DictionaryPtr::dynamicCast(type);
              assert(dict);
              out << nl << "return " << fixKwd(dict->scoped()) << "();";
            }
          }
        }
      }
    }
  }
}

bool Slice::Gen::ImplVisitor::visitModuleStart(const ModulePtr& p)
{
  if (!p->hasClassDefs()) {
    return false;
  }

  string name = fixKwd(p->name());

  H << sp << nl << "namespace " << name << nl << '{';

  return true;
}

void Slice::Gen::ImplVisitor::visitModuleEnd(const ModulePtr& p)
{
  H << sp;
  H << nl << '}';
}

bool Slice::Gen::ImplVisitor::visitClassDefStart(const ClassDefPtr& p)
{
  if (!p->isAbstract()) {
    return false;
  }

  string name = p->name();
  string scope = fixKwd(p->scope());
  string cls = scope.substr(2) + name + "I";

  ClassList bases = p->bases();
  ClassDefPtr base;
  if (!bases.empty() && !bases.front()->isInterface()) {
    base = bases.front();
  }

  H << sp;
  H << nl << "class " << name << "I : ";
  H.useCurrentPosAsIndent();
  H << "virtual public " << fixKwd(name);
  for (ClassList::const_iterator q = bases.begin(); q != bases.end(); ++q) {
    H << ',' << nl << "virtual public " << fixKwd((*q)->scope());
    if ((*q)->isAbstract()) {
      H << (*q)->name() << "I";
    } else {
      H << fixKwd((*q)->name());
    }
  }
  H.restoreIndent();

  H << sb;
  H.dec();
  H << nl << "public:";
  H.inc();

  OperationList ops = p->operations();
  OperationList::const_iterator r;

  for (r = ops.begin(); r != ops.end(); ++r) {
    OperationPtr op = (*r);
    string opName = op->name();

    TypePtr ret = op->returnType();
    string retS = returnTypeToString(ret, op->getMetaData());

    H << sp << nl << "virtual " << retS << ' ' << fixKwd(opName) << '(';
    H.useCurrentPosAsIndent();
    ParamDeclList paramList = op->parameters();
    ParamDeclList::const_iterator q;
    for (q = paramList.begin(); q != paramList.end(); ++q) {
      if (q != paramList.begin()) {
        H << ',' << nl;
      }

      StringList metaData = (*q)->getMetaData();
#if defined(__SUNPRO_CC) && (__SUNPRO_CC==0x550)
      //
      // Work around for Sun CC 5.5 bug #4853566
      //
      string typeString;
      if((*q)->isOutParam())
      {
        typeString = outputTypeToString((*q)->type(), metaData);
      }
      else
      {
        typeString = inputTypeToString((*q)->type(), metaData);
      }
#else
      string typeString = (*q)->isOutParam() ? outputTypeToString((*q)->type(), _useWstring, metaData)
          : inputTypeToString((*q)->type(), _useWstring, metaData);
#endif
      H << typeString;
    }
    if (!p->isLocal()) {
      if (!paramList.empty()) {
        H << ',' << nl;
      }
      H << "const Ice::Current&";
    }
    H.restoreIndent();

    bool nonmutating = op->mode() == Operation::Nonmutating;

    H << ")" << (nonmutating ? " const" : "") << ';';

    C << sp << nl << retS << nl;
    C << scope.substr(2) << name << "I::" << fixKwd(opName) << '(';
    C.useCurrentPosAsIndent();
    for (q = paramList.begin(); q != paramList.end(); ++q) {
      if (q != paramList.begin()) {
        C << ',' << nl;
      }

      StringList metaData = (*q)->getMetaData();
#if defined(__SUNPRO_CC) && (__SUNPRO_CC==0x550)
      //
      // Work around for Sun CC 5.5 bug #4853566
      //
      string typeString;
      if((*q)->isOutParam())
      {
        typeString = outputTypeToString((*q)->type(), metaData);
      }
      else
      {
        typeString = inputTypeToString((*q)->type(), metaData);
      }
#else
      string typeString = (*q)->isOutParam() ? outputTypeToString((*q)->type(), _useWstring, metaData)
          : inputTypeToString((*q)->type(), _useWstring, metaData);
#endif
      C << typeString << ' ' << fixKwd((*q)->name());
    }
    if (!p->isLocal()) {
      if (!paramList.empty()) {
        C << ',' << nl;
      }
      C << "const Ice::Current& current";
    }
    C.restoreIndent();
    C << ')';
    C << (nonmutating ? " const" : "");
    C << sb;

    if (ret) {
      writeReturn(C, ret, op->getMetaData());
    }

    C << eb;
  }

  H << eb << ';';

  return true;
}


Slice::Gen::AsyncVisitor::AsyncVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport), _useWstring(false)
{
}

bool
Slice::Gen::AsyncVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls() || (!p->hasContentsWithMetaData("ami") && !p->hasContentsWithMetaData("amd")))
    {
	    return false;
    }

    _useWstring = setUseWstring(p, _useWstringHist, _useWstring);

    string name = fixKwd(p->name());

    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::AsyncVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';

    _useWstring = resetUseWstring(_useWstringHist);
}

bool
Slice::Gen::AsyncVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    _useWstring = setUseWstring(p, _useWstringHist, _useWstring);
    return true;
}

void
Slice::Gen::AsyncVisitor::visitClassDefEnd(const ClassDefPtr&)
{
    _useWstring = resetUseWstring(_useWstringHist);
}

void
Slice::Gen::AsyncVisitor::visitOperation(const OperationPtr& p)
{
  ContainerPtr container = p->container();
  ClassDefPtr cl = ClassDefPtr::dynamicCast(container);

  if(cl->isLocal() ||
     (!cl->hasMetaData("ami") && !p->hasMetaData("ami") && !cl->hasMetaData("amd") && !p->hasMetaData("amd")))
  {
    return;
  }

  string name = p->name();

  string className = cl->name();
  string classNameAMI = "AMI_" + className;
  string classNameAMD = "AMH_" + className;
  string classScope = fixKwd(cl->scope());
  string classScopedAMI = classScope + classNameAMI;
  string classScopedAMD = classScope + classNameAMD;
  string proxyName = classScope + className + "Prx";

  vector<string> params;
  vector<string> paramsAMD;
  vector<string> paramsDecl;
  vector<string> args;

  vector<string> paramsInvoke;
  vector<string> paramsDeclInvoke;

  paramsInvoke.push_back("const " + proxyName + "&");
  paramsDeclInvoke.push_back("const " + proxyName + "& __prx");

  TypePtr ret = p->returnType();
  string retS = inputTypeToString(ret, _useWstring, p->getMetaData());

  if(ret)
  {
    params.push_back(retS);
    paramsAMD.push_back(inputTypeToString(ret, _useWstring, p->getMetaData()));
    paramsDecl.push_back(retS + " __ret");
    args.push_back("__ret");
  }

  ParamDeclList inParams;
  ParamDeclList outParams;
  ParamDeclList paramList = p->parameters();
  for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
  {
    string paramName = fixKwd((*q)->name());
    TypePtr type = (*q)->type();
    string typeString = inputTypeToString(type, _useWstring, (*q)->getMetaData());

    if((*q)->isOutParam())
    {
        params.push_back(typeString);
        paramsAMD.push_back(inputTypeToString(type, _useWstring, (*q)->getMetaData()));
        paramsDecl.push_back(typeString + ' ' + paramName);
        args.push_back(paramName);

        outParams.push_back(*q);
    }
    else
    {
        paramsInvoke.push_back(typeString);
        paramsDeclInvoke.push_back(typeString + ' ' + paramName);

        inParams.push_back(*q);
    }
  }

  paramsInvoke.push_back("const ::Ice::Context&");
  paramsDeclInvoke.push_back("const ::Ice::Context& __ctx");

  if(cl->hasMetaData("ami") || p->hasMetaData("ami"))
  {
    H << sp << nl << "class " << _dllExport << classNameAMI << '_' << name
      << " : public ReplyHandler";
    H << sb;
    H.dec();
    H << nl << "public:";
    H.inc();
    H << sp;
    H << nl << "virtual void response" << spar << params << epar << " = 0;";
    //H << nl << "virtual void ice_exception(const ::Ice::Exception&) = 0;";
    //H << sp;
    //H << nl << "void __invoke" << spar << paramsInvoke << epar << ';';
    //H << sp;
    H.dec();
    H << nl << "protected:";
    H.inc();
    H << sp;
    H << nl << "virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);";
    H << eb << ';';
    //H << sp << nl << "typedef ::IceUtil::Handle< " << classScopedAMI << '_' << name << "> " << classNameAMI
    //  << '_' << name  << "Ptr;";

    string flatName = p->flattenedScope() + name + "_name";
/*
    C << sp << nl << "void" << nl << classScopedAMI.substr(2) << '_' << name << "::__invoke" << spar
      << paramsDeclInvoke << epar;
    C << sb;
    C << nl << "try";
    C << sb;
    C << nl << "__prepare(__prx, " << flatName << ", " << operationModeToString(p->mode()) << ", __ctx);";
    writeMarshalCode(C, inParams, 0, StringList(), true);
    if(p->sendsClasses())
    {
        C << nl << "__os->writePendingObjects();";
    }
    C << nl << "__os->endWriteEncaps();";
    C << eb;
    C << nl << "catch(const ::Ice::LocalException& __ex)";
    C << sb;
    C << nl << "__finished(__ex);";
    C << nl << "return;";
    C << eb;
    C << nl << "__send();";
    C << eb;
*/
    C << sp << nl << "void" << nl << classScopedAMI.substr(2) << '_' << name << "::__response(InputStream* in, ACE_CDR::UInt replyStatus)";
    C << sb;
    C << nl << "if (replyStatus == 0)";
    C << sb;
    writeAllocateCode(C, outParams, ret, p->getMetaData(), true);
    /*
    C << nl << "try";
    C << sb;
    C << nl << "if(!__ok)";
    C << sb;
    C << nl << "try";
    C << sb;
    C << nl << "__is->throwException();";
    C << eb;

    //
    // Generate a catch block for each legal user exception.
    // (See comment in DelegateMVisitor::visitOperation() for details.)
    //
    ExceptionList throws = p->throws();
    throws.sort();
    throws.unique();
  #if defined(__SUNPRO_CC)
    throws.sort(derivedToBaseCompare);
  #else
    throws.sort(Slice::DerivedToBaseCompare());
  #endif
    for(ExceptionList::const_iterator i = throws.begin(); i != throws.end(); ++i)
    {
        string scoped = (*i)->scoped();
        C << nl << "catch(const " << fixKwd((*i)->scoped()) << "& __ex)";
        C << sb;
        C << nl << "ice_exception(__ex);";
        C << nl << "return;";
        C << eb;
    }
    C << nl << "catch(const ::Ice::UserException& __ex)";
    C << sb;
    C << nl << "throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());";
    C << eb;
    C << eb;
*/
    writeUnmarshalCode(C, outParams, 0, StringList(), "in", true);
    if(ret)
    {
        //writeMarshalUnmarshalCode(C, ret, "__ret", false, "", true, p->getMetaData(), true);
        writeStreamMarshalUnmarshalCode(C, ret, "__ret", false, "in", p->getMetaData());
    }
    /*
    if(p->returnsClasses())
    {
        C << nl << "__is->readPendingObjects();";
    }
    C << eb;
    C << nl << "catch(const ::Ice::LocalException& __ex)";
    C << sb;
    C << nl << "__finished(__ex);";
    C << nl << "return;";
    C << eb;
    */
    C << nl << "this->response" << spar << args << epar << ';';
    C << eb;

    C << eb;
  }

  if(cl->hasMetaData("amd") || p->hasMetaData("amd"))
  {
    H << sp << nl << "class " << _dllExport << classNameAMD << '_' << name;
      //<< " : virtual public ::IceUtil::Shared";
    H << sb;
    H.dec();
    H << nl << "public:";
    H.inc();
    H << sp;
    H << nl << "virtual void response" << spar << paramsAMD << epar << " = 0;";
    //H << nl << "virtual void ice_exception(const ::Ice::Exception&) = 0;";
    //H << nl << "virtual void ice_exception(const ::std::exception&) = 0;";
    //H << nl << "virtual void ice_exception() = 0;";
    H << eb << ';';
    //H << sp << nl << "typedef ::IceUtil::Handle< " << classScopedAMD << '_' << name << "> "
    //  << classNameAMD << '_' << name  << "Ptr;";
  }
}

Slice::Gen::AsyncImplVisitor::AsyncImplVisitor(Output& h, Output& c, const string& dllExport) :
    H(h), C(c), _dllExport(dllExport), _useWstring(false)
{
}

bool
Slice::Gen::AsyncImplVisitor::visitUnitStart(const UnitPtr& p)
{
    if(!p->hasNonLocalClassDecls() || !p->hasContentsWithMetaData("amd"))
    {
	return false;
    }

    H << sp << nl << "namespace IcmAsync" << nl << '{';

    return true;
}

void
Slice::Gen::AsyncImplVisitor::visitUnitEnd(const UnitPtr& p)
{
    H << sp << nl << '}';
}

bool
Slice::Gen::AsyncImplVisitor::visitModuleStart(const ModulePtr& p)
{
    if(!p->hasNonLocalClassDecls() || !p->hasContentsWithMetaData("amd"))
    {
	return false;
    }

    _useWstring = setUseWstring(p, _useWstringHist, _useWstring);

    string name = fixKwd(p->name());

    H << sp << nl << "namespace " << name << nl << '{';

    return true;
}

void
Slice::Gen::AsyncImplVisitor::visitModuleEnd(const ModulePtr& p)
{
    H << sp << nl << '}';

    _useWstring = resetUseWstring(_useWstringHist);
}

bool
Slice::Gen::AsyncImplVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    _useWstring = setUseWstring(p, _useWstringHist, _useWstring);
    return true;
}

void
Slice::Gen::AsyncImplVisitor::visitClassDefEnd(const ClassDefPtr&)
{
    _useWstring = resetUseWstring(_useWstringHist);
}

void
Slice::Gen::AsyncImplVisitor::visitOperation(const OperationPtr& p)
{
    ContainerPtr container = p->container();
    ClassDefPtr cl = ClassDefPtr::dynamicCast(container);

    if(cl->isLocal() || (!cl->hasMetaData("amd") && !p->hasMetaData("amd")))
    {
	    return;
    }

    string name = p->name();

    string classNameAMD = "AMH_" + cl->name();
    string classScope = fixKwd(cl->scope());
    string classScopedAMD = classScope + classNameAMD;

    string params;
    string paramsDecl;
    string args;

    ExceptionList throws = p->throws();
    throws.sort();
    throws.unique();

    //
    // Arrange exceptions into most-derived to least-derived order. If we don't
    // do this, a base exception handler can appear before a derived exception
    // handler, causing compiler warnings and resulting in the base exception
    // being marshaled instead of the derived exception.
    //
#if defined(__SUNPRO_CC)
    throws.sort(derivedToBaseCompare);
#else
    throws.sort(Slice::DerivedToBaseCompare());
#endif

    TypePtr ret = p->returnType();
    string retS = inputTypeToString(ret, _useWstring, p->getMetaData());

    if(ret)
    {
	    params += retS;
	    paramsDecl += retS;
	    paramsDecl += ' ';
	    paramsDecl += "__ret";
	    args += "__ret";
    }

    ParamDeclList outParams;
    ParamDeclList paramList = p->parameters();
    for(ParamDeclList::const_iterator q = paramList.begin(); q != paramList.end(); ++q)
    {
	    if((*q)->isOutParam())
	    {
        string paramName = fixKwd((*q)->name());
        TypePtr type = (*q)->type();
        string typeString = inputTypeToString(type, _useWstring, (*q)->getMetaData());

        if(ret || !outParams.empty())
        {
	        params += ", ";
	        paramsDecl += ", ";
	        args += ", ";
        }

        params += typeString;
        paramsDecl += typeString;
        paramsDecl += ' ';
        paramsDecl += paramName;
        args += paramName;

        outParams.push_back(*q);
	    }
    }

    H << sp << nl << "class " << _dllExport << classNameAMD << '_' << name
      << " : public " << classScopedAMD  << '_' << name << ", public AmhResponseHandler";
    H << sb;
    H.dec();
    H << nl << "public:";
    H.inc();

    H << sp;
    H << nl << classNameAMD << '_' << name << "(ServerRequest&);";

    H << sp;
    H << nl << "virtual void response(" << params << ");";
    H << eb << ';';

    C << sp << nl << "IcmAsync" << classScopedAMD << '_' << name << "::" << classNameAMD << '_' << name
      << "(ServerRequest& serverRequest) :";
    C.inc();
    C << nl << "AmhResponseHandler (serverRequest)";
    C.dec();
    C << sb;
    C << eb;

    C << sp << nl << "void" << nl << "IcmAsync" << classScopedAMD << '_' << name << "::response("
      << paramsDecl << ')';
    C << sb;
    C << nl << "if(getConnectionError())" << sb;
    C << nl << "delete this;";
    C << nl << "return;" << eb;
    C << nl << "this->initReply();";
    if(ret || !outParams.empty())
    {
	    writeMarshalCode(C, outParams, 0, StringList(), "this->mOutStream", true);
	    if(ret)
	    {
	        //writeMarshalUnmarshalCode(C, ret, "__ret", true, "", true, p->getMetaData(), true);
          writeStreamMarshalUnmarshalCode(C,ret,"__ret",true,"this->mOutStream",p->getMetaData());
	    }
	    if(p->returnsClasses())
	    {
	        C << nl << "__os->writePendingObjects();";
	    }
    }
    C << nl << "this->sendReply();";
    C << nl << "delete this;";
    C << eb;
}


bool Slice::Gen::MetaDataVisitor::visitModuleStart(const ModulePtr& p)
{
  //
  // Validate global metadata.
  //
  DefinitionContextPtr dc = p->definitionContext();
  assert(dc);
  StringList globalMetaData = dc->getMetaData();
  string file = dc->filename();
  static const string prefix = "cpp:";
  for (StringList::const_iterator q = globalMetaData.begin(); q != globalMetaData.end(); ++q) {
    string s = *q;
    if (_history.count(s) == 0) {
      if (s.find(prefix) == 0) {
        string ss = s.substr(prefix.size());
        if (ss.find("include:") == 0) {
          continue;
        }
        cout << file << ": warning: ignoring invalid global metadata `" << s << "'" << endl;
      }
      _history.insert(s);
    }
  }

  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
  return true;
}

void Slice::Gen::MetaDataVisitor::visitModuleEnd(const ModulePtr&)
{
}

void Slice::Gen::MetaDataVisitor::visitClassDecl(const ClassDeclPtr& p)
{
  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
}

bool Slice::Gen::MetaDataVisitor::visitClassDefStart(const ClassDefPtr& p)
{
  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
  return true;
}

void Slice::Gen::MetaDataVisitor::visitClassDefEnd(const ClassDefPtr&)
{
}

bool Slice::Gen::MetaDataVisitor::visitExceptionStart(const ExceptionPtr& p)
{
  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
  return true;
}

void Slice::Gen::MetaDataVisitor::visitExceptionEnd(const ExceptionPtr&)
{
}

bool Slice::Gen::MetaDataVisitor::visitStructStart(const StructPtr& p)
{
  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
  return true;
}

void Slice::Gen::MetaDataVisitor::visitStructEnd(const StructPtr&)
{
}

void Slice::Gen::MetaDataVisitor::visitOperation(const OperationPtr& p)
{
  StringList metaData = p->getMetaData();
  TypePtr returnType = p->returnType();
  if (!metaData.empty()) {
    if (!returnType) {
      for (StringList::const_iterator q = metaData.begin(); q != metaData.end(); ++q) {
        if (q->find("cpp:type:", 0) == 0 || q->find("cpp:array", 0) == 0 || q->find("cpp:range", 0)
            == 0) {
          cout << p->definitionContext()->filename() << ":" << p->line()
              << ": warning: invalid metadata for operation" << endl;
          break;
        }
      }
    } else {
      validate(returnType, metaData, p->definitionContext()->filename(), p->line(), false);
    }
  }

  ParamDeclList params = p->parameters();
  for (ParamDeclList::iterator q = params.begin(); q != params.end(); ++q) {
    validate((*q)->type(), (*q)->getMetaData(), p->definitionContext()->filename(), (*q)->line(), !(*q)->isOutParam());
  }
}

void Slice::Gen::MetaDataVisitor::visitParamDecl(const ParamDeclPtr& p)
{
  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
}

void Slice::Gen::MetaDataVisitor::visitDataMember(const DataMemberPtr& p)
{
  validate(p->type(), p->getMetaData(), p->definitionContext()->filename(), p->line());
}

void Slice::Gen::MetaDataVisitor::visitList(const ListPtr& p)
{
  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
}

void Slice::Gen::MetaDataVisitor::visitSequence(const SequencePtr& p)
{
  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
}

void Slice::Gen::MetaDataVisitor::visitDictionary(const DictionaryPtr& p)
{
  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
}

void Slice::Gen::MetaDataVisitor::visitEnum(const EnumPtr& p)
{
  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
}

void Slice::Gen::MetaDataVisitor::visitConst(const ConstPtr& p)
{
  validate(p, p->getMetaData(), p->definitionContext()->filename(), p->line());
}

void Slice::Gen::MetaDataVisitor::validate(const SyntaxTreeBasePtr& cont,
                                           const StringList& metaData,
                                           const string& file,
                                           const string& line,
                                           bool inParam)
{
  static const string prefix = "cpp:";
  for (StringList::const_iterator p = metaData.begin(); p != metaData.end(); ++p) {
    string s = *p;
    if (_history.count(s) == 0) {
      if (s.find(prefix) == 0) {
        if (SequencePtr::dynamicCast(cont)) {
          string ss = s.substr(prefix.size());
          if (ss.find("type:") == 0 || (inParam && (ss == "array" || ss.find("range") == 0))) {
            continue;
          }
        }
        cout << file << ":" << line << ": warning: ignoring invalid metadata `" << s << "'" << endl;
      }
      _history.insert(s);
    }
  }
}

void Slice::Gen::validateMetaData(const UnitPtr& u)
{
  MetaDataVisitor visitor;
  u->visit(&visitor, false);
}

void Slice::Gen::printHeader(Output& out)
{
  static const char* header =
      "// **********************************************************************\n"
        "//\n"
        "// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.\n"
        "//\n"
        "// This copy of Ice-E is licensed to you under the terms described in the\n"
        "// ICEE_LICENSE file included in this distribution.\n"
        "//\n"
        "// **********************************************************************\n";

  out << header;
  out << "\n// Ice-E version " << ICEE_STRING_VERSION;
}

void Slice::Gen::printVersionCheck(Output& out)
{
  out << "\n";
  out << "\n#ifndef ICEE_IGNORE_VERSION";
  out << "\n#   if ICEE_INT_VERSION / 100 != " << ICEE_INT_VERSION / 100;
  out << "\n#       error IceE version mismatch!";
  out << "\n#   endif";
  out << "\n#   if ICEE_INT_VERSION % 100 < " << ICEE_INT_VERSION % 100;
  out << "\n#       error IceE patch level mismatch!";
  out << "\n#   endif";
  out << "\n#endif";
}

void Slice::Gen::printDllExportStuff(Output& out, const string& dllExport)
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

bool
Slice::Gen::setUseWstring(ContainedPtr p, list<bool>& hist, bool use)
{
    hist.push_back(use);
    StringList metaData = p->getMetaData();
    if(find(metaData.begin(), metaData.end(), "cpp:type:wstring") != metaData.end())
    {
        use = true;
    }
    else if(find(metaData.begin(), metaData.end(), "cpp:type:string") != metaData.end())
    {
        use = false;
    }
    return use;
}

bool
Slice::Gen::resetUseWstring(list<bool>& hist)
{
    bool use = hist.back();
    hist.pop_back();
    return use;
}
