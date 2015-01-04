// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/Functional.h>
#include <Slice/Parser.h>
#include <Slice/GrammarUtil.h>
#include <string.h>

using namespace std;
using namespace Slice;

extern FILE* slice_in;
extern int slice_debug;

namespace Slice
{

Unit* unit;

}

// ----------------------------------------------------------------------
// toLower() helper function
// ----------------------------------------------------------------------

static void toLower(string& s)
{
  transform(s.begin(), s.end(), s.begin(), ::tolower);
}

// ----------------------------------------------------------------------
// DefinitionContext
// ----------------------------------------------------------------------

Slice::DefinitionContext::DefinitionContext(int includeLevel) :
  _includeLevel(includeLevel), _seenDefinition(false)
{
}

string Slice::DefinitionContext::filename() const
{
  return _filename;
}

int Slice::DefinitionContext::includeLevel() const
{
  return _includeLevel;
}

bool Slice::DefinitionContext::seenDefinition() const
{
  return _seenDefinition;
}

void Slice::DefinitionContext::setFilename(const string& filename)
{
  _filename = filename;
}

void Slice::DefinitionContext::setSeenDefinition()
{
  _seenDefinition = true;
}

bool Slice::DefinitionContext::hasMetaData() const
{
  return !_metaData.empty();
}

void Slice::DefinitionContext::setMetaData(const StringList& metaData)
{
  _metaData = metaData;
}

string Slice::DefinitionContext::findMetaData(const string& prefix) const
{
  for (StringList::const_iterator p = _metaData.begin(); p != _metaData.end(); ++p) {
    if ((*p).find(prefix) == 0) {
      return *p;
    }
  }

  return string();
}

StringList Slice::DefinitionContext::getMetaData() const
{
  return _metaData;
}

// ----------------------------------------------------------------------
// SyntaxTreeBase
// ----------------------------------------------------------------------

void Slice::SyntaxTreeBase::destroy()
{
  _unit = 0;
}

UnitPtr Slice::SyntaxTreeBase::unit() const
{
  return _unit;
}

DefinitionContextPtr Slice::SyntaxTreeBase::definitionContext() const
{
  return _definitionContext;
}

void Slice::SyntaxTreeBase::visit(ParserVisitor*, bool)
{
}

Slice::SyntaxTreeBase::SyntaxTreeBase(const UnitPtr& unit) :
  _unit(unit)
{
  if (_unit) {
    _definitionContext = unit->currentDefinitionContext();
  }
}

// ----------------------------------------------------------------------
// Type
// ----------------------------------------------------------------------

Slice::Type::Type(const UnitPtr& unit) :
  SyntaxTreeBase(unit)
{
}

// ----------------------------------------------------------------------
// Builtin
// ----------------------------------------------------------------------

bool Slice::Builtin::isLocal() const
{
  return _kind == KindLocalObject;
}

string Slice::Builtin::typeId() const
{
  switch (_kind)
  {
  case KindByte: {
    return "byte";
    break;
  }
  case KindBool: {
    return "bool";
    break;
  }
  case KindShort: {
    return "short";
    break;
  }
  case KindInt: {
    return "int";
    break;
  }
  case KindLong: {
    return "long";
    break;
  }
  case KindFloat: {
    return "float";
    break;
  }
  case KindDouble: {
    return "double";
    break;
  }
  case KindString: {
    return "string";
    break;
  }
  case KindObject: {
    return "::Ice::Object";
    break;
  }
  case KindObjectProxy: {
    return "::Ice::Object*";
    break;
  }
  case KindLocalObject: {
    return "::Ice::LocalObject";
    break;
  }
  }
  assert(false);
  return ""; // Keep the compiler happy.
}

bool Slice::Builtin::usesClasses() const
{
  return _kind == KindObject;
}

size_t Slice::Builtin::minWireSize() const
{
  static size_t minWireSizeTable[] = { 1, // KindByte
      1, // KindBool
      2, // KindShort
      4, // KindInt
      8, // KindLong
      4, // KindFloat
      8, // KindDouble
      1, // KindString: at least one byte for an empty string.
      4, // KindObject: at least 4 bytes (to marshal an index instead of an instance).
      2 // KindObjectProxy: at least an empty identity for a nil proxy, that is, 2 bytes.
      };

  assert(_kind != KindLocalObject);
  return minWireSizeTable[_kind];
}

bool Slice::Builtin::isVariableLength() const
{
  return _kind == KindString || _kind == KindObject || _kind == KindObjectProxy;
}

Builtin::Kind Slice::Builtin::kind() const
{
  return _kind;
}

string Builtin::kindAsString() const
{
  return builtinTable[_kind];
}

const char* Slice::Builtin::builtinTable[] = { "byte", "bool", "short", "int", "long", "float",
    "double", "string", "Object", "Object*", "LocalObject" };

Slice::Builtin::Builtin(const UnitPtr& unit, Kind kind) :
  SyntaxTreeBase(unit), Type(unit), _kind(kind)
{
  //
  // Builtin types do not have a definition context.
  //
  _definitionContext = 0;
}

// ----------------------------------------------------------------------
// Contained
// ----------------------------------------------------------------------

ContainerPtr Slice::Contained::container() const
{
  return _container;
}

string Slice::Contained::name() const
{
  return _name;
}

string Slice::Contained::scoped() const
{
  return _scoped;
}

string Slice::Contained::scope() const
{
  string::size_type idx = _scoped.rfind("::");
  assert(idx != string::npos);
  return string(_scoped, 0, idx + 2);
}

string Slice::Contained::flattenedScope() const
{
  string s = scope();
  string flattenedScope;
  for (string::const_iterator r = s.begin(); r != s.end(); ++r) {
    flattenedScope += ((*r) == ':') ? '_' : *r;
  }
  return flattenedScope;
}

string Slice::Contained::file() const
{
  return _file;
}

string Slice::Contained::line() const
{
  return _line;
}

string Slice::Contained::comment() const
{
  return _comment;
}

int Slice::Contained::includeLevel() const
{
  return _includeLevel;
}

void Slice::Contained::updateIncludeLevel()
{
  _includeLevel = min(_includeLevel, _unit->currentIncludeLevel());
}

bool Slice::Contained::hasMetaData(const string& meta) const
{
  return find(_metaData.begin(), _metaData.end(), meta) != _metaData.end();
}

list<string> Slice::Contained::getMetaData() const
{
  return _metaData;
}

void Slice::Contained::setMetaData(const list<string>& metaData)
{
  _metaData = metaData;
}

//
// TODO: remove this method once "cs:" and "vb:" prefix are hard errors.
//
void Slice::Contained::addMetaData(const string& s)
{
  _metaData.push_back(s);
}

bool Slice::Contained::operator<(const Contained& rhs) const
{
  return _scoped < rhs._scoped;
}

bool Slice::Contained::operator==(const Contained& rhs) const
{
  return _scoped == rhs._scoped;
}

bool Slice::Contained::operator!=(const Contained& rhs) const
{
  return _scoped != rhs._scoped;
}

Slice::Contained::Contained(const ContainerPtr& container, const string& name) :
  SyntaxTreeBase(container->unit()), _container(container), _name(name)
{
  ContainedPtr cont = ContainedPtr::dynamicCast(_container);
  if (cont) {
    _scoped = cont->scoped();
  }
  _scoped += "::" + _name;
  assert(_unit);
  _unit->addContent(this);
  _file = _unit->currentFile();
  ostringstream s;
  s << _unit->currentLine();
  _line = s.str();
  _comment = _unit->currentComment();
  _includeLevel = _unit->currentIncludeLevel();
}

// ----------------------------------------------------------------------
// Container
// ----------------------------------------------------------------------

void Slice::Container::destroy()
{
  for_each(_contents.begin(), _contents.end(), ::IceUtil::voidMemFun(&SyntaxTreeBase::destroy));
  _contents.clear();
  _introducedMap.clear();
  SyntaxTreeBase::destroy();
}

ModulePtr Slice::Container::createModule(const string& name)
{
  checkPrefix(name);
  ContainedList matches = _unit->findContents(thisScope() + name);
  matches.sort(); // Modules can occur many times...
  matches.unique(); // ... but we only want one instance of each.

  for (ContainedList::const_iterator p = matches.begin(); p != matches.end(); ++p) {
    bool differsOnlyInCase = !_unit->caseSensitive() && matches.front()->name() != name;
    ModulePtr module = ModulePtr::dynamicCast(*p);
    if (module) {
      if (differsOnlyInCase) // Modules can be reopened only if they are capitalized correctly.
      {
        string msg = "module `" + name
            + "' is capitalized inconsistently with its previous name: `";
        msg += module->name() + "'";
        _unit->error(msg);
        return 0;
      }
    } else if (!differsOnlyInCase) {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
      msg += "' as module";
      _unit->error(msg);
      return 0;
    } else {
      string msg = "module `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " name `" + matches.front()->name() + "'";
      _unit->error(msg);
      return 0;
    }
  }

  if (!nameIsLegal(name, "module")) {
    return 0;
  }

  ModulePtr q = new Module(this, name);
  _contents.push_back(q);
  return q;
}

ClassDefPtr Slice::Container::createClassDef(const string& name,
                                             bool intf,
                                             const ClassList& bases,
                                             bool local)
{
  checkPrefix(name);
  ContainedList matches = _unit->findContents(thisScope() + name);
  for (ContainedList::const_iterator p = matches.begin(); p != matches.end(); ++p) {
    ClassDeclPtr decl = ClassDeclPtr::dynamicCast(*p);
    if (decl) {
      if (checkInterfaceAndLocal(name, false, intf, decl->isInterface(), local, decl->isLocal())) {
        continue;
      }
      return 0;
    }

    bool differsOnlyInCase = !_unit->caseSensitive() && matches.front()->name() != name;
    ClassDefPtr def = ClassDefPtr::dynamicCast(*p);
    if (def) {
      if (differsOnlyInCase) {
        string msg = intf ? "interface" : "class";
        msg += " definition `" + name + "' is capitalized inconsistently with its previous name: `";
        msg += def->name() + "'";
        _unit->error(msg);
      } else {
        if (_unit->ignRedefs()) {
          def->updateIncludeLevel();
          return def;
        }

        string msg = "redefinition of ";
        msg += intf ? "interface" : "class";
        msg += " `" + name + "'";
        _unit->error(msg);
      }
    } else if (!_unit->caseSensitive() && differsOnlyInCase) {
      string msg = intf ? "interface" : "class";
      msg = " definition `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " name `" + matches.front()->name() + "'";
      _unit->error(msg);
    } else {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name()
          + "' as ";
      msg += intf ? "interface" : "class";
      _unit->error(msg);
    }
    return 0;
  }

  if (!nameIsLegal(name, intf ? "interface" : "class")) {
    return 0;
  }

  if (!checkForGlobalDef(name, intf ? "interface" : "class")) {
    return 0;
  }

  ClassDecl::checkBasesAreLegal(name, intf, local, bases, _unit);

  ClassDefPtr def = new ClassDef(this, name, intf, bases, local);
  _contents.push_back(def);

  for (ContainedList::const_iterator q = matches.begin(); q != matches.end(); ++q) {
    ClassDeclPtr decl = ClassDeclPtr::dynamicCast(*q);
    decl->_definition = def;
  }

  //
  // Implicitly create a class declaration for each class
  // definition. This way the code generator can rely on always
  // having a class declaration available for lookup.
  //
  ClassDeclPtr decl = createClassDecl(name, intf, local);
  def->_declaration = decl;

  return def;
}

ClassDeclPtr Slice::Container::createClassDecl(const string& name, bool intf, bool local)
{
  checkPrefix(name);

  ClassDefPtr def;

  ContainedList matches = _unit->findContents(thisScope() + name);
  for (ContainedList::const_iterator p = matches.begin(); p != matches.end(); ++p) {
    ClassDefPtr clDef = ClassDefPtr::dynamicCast(*p);
    if (clDef) {
      if (checkInterfaceAndLocal(name, true, intf, clDef->isInterface(), local, clDef->isLocal())) {
        assert(!def);
        def = clDef;
        continue;
      }
      return 0;
    }

    ClassDeclPtr clDecl = ClassDeclPtr::dynamicCast(*p);
    if (clDecl) {
      if (checkInterfaceAndLocal(name, false, intf, clDecl->isInterface(), local, clDecl->isLocal())) {
        continue;
      }
      return 0;
    }

    bool differsOnlyInCase = !_unit->caseSensitive() && matches.front()->name() != name;
    if (differsOnlyInCase) {
      string msg = "class declaration `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " name `" + matches.front()->name() + "'";
      _unit->error(msg);
    } else {
      string msg = "declaration of already defined `";
      msg += name;
      msg += "' as ";
      msg += intf ? "interface" : "class";
      _unit->error(msg);
      return 0;
    }
  }

  if (!nameIsLegal(name, intf ? "interface" : "class")) {
    return 0;
  }

  if (!checkForGlobalDef(name, intf ? "interface" : "class")) {
    return 0;
  }

  //
  // Multiple declarations are permissible. But if we do already
  // have a declaration for the class in this container, we don't
  // create another one.
  //
  for (ContainedList::const_iterator q = _contents.begin(); q != _contents.end(); ++q) {
    if ((*q)->name() == name) {
      ClassDeclPtr decl = ClassDeclPtr::dynamicCast(*q);
      if (decl) {
        return decl;
      }

      assert(ClassDefPtr::dynamicCast(*q));
    }
  }

  _unit->currentContainer();
  ClassDeclPtr decl = new ClassDecl(this, name, intf, local);
  _contents.push_back(decl);

  if (def) {
    decl->_definition = def;
  }

  //by qinghua 2009/09/23
  if(local) {
    std::string includeFile = name + ".h";
    this->_unit->addIncludeFile(includeFile);
  }
  return decl;
}

ExceptionPtr Slice::Container::createException(const string& name,
                                               const ExceptionPtr& base,
                                               bool local)
{
  checkPrefix(name);

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    ExceptionPtr p = ExceptionPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (matches.front()->name() == name) {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
      msg += "' as exception";
      _unit->error(msg);
    } else if (!_unit->caseSensitive()) {
      string msg = "exception `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " `" + matches.front()->name() + "'";
      _unit->error(msg);
    }
  }

  nameIsLegal(name, "exception"); // Don't return here -- we create the exception anyway

  checkForGlobalDef(name, "exception"); // Don't return here -- we create the exception anyway

  //
  // If this definition is non-local, base cannot be local.
  //
  if (!local && base && base->isLocal()) {
    _unit->error("non-local exception `" + name + "' cannot have local base exception `"
        + base->name() + "'");
  }

  ExceptionPtr p = new Exception(this, name, base, local);
  _contents.push_back(p);
  return p;
}

StructPtr Slice::Container::createStruct(const string& name, bool local)
{
  checkPrefix(name);

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    StructPtr p = StructPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (matches.front()->name() == name) {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
      msg += "' as struct";
      _unit->error(msg);
    } else if (!_unit->caseSensitive()) {
      string msg = "struct `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " `" + matches.front()->name() + "'";
      _unit->error(msg);
    }
  }

  nameIsLegal(name, "structure"); // Don't return here -- we create the struct anyway.

  checkForGlobalDef(name, "structure"); // Don't return here -- we create the struct anyway.

  StructPtr p = new Struct(this, name, local);
  _contents.push_back(p);
  return p;
}

ListPtr Slice::Container::createList(const string& name, const TypePtr& type, bool local)
{
  checkPrefix(name);

  if (_unit->profile() == IceE && !local) {
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if ((builtin && builtin->kind() == Builtin::KindObject) || ClassDeclPtr::dynamicCast(type)) {
      /* commented by qinghua
      string msg = "Sequence `" + name + "' cannot contain object values.";
      _unit->error(msg);
      return 0;
      */
    }
  }

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    ListPtr p = ListPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (matches.front()->name() == name) {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
      msg += "' as list";
      _unit->error(msg);
    } else if (!_unit->caseSensitive()) {
      string msg = "list `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " `" + matches.front()->name() + "'";
      _unit->error(msg);
    }
  }

  nameIsLegal(name, "list"); // Don't return here -- we create the sequence anyway.

  checkForGlobalDef(name, "list"); // Don't return here -- we create the sequence anyway.

  //
  // If sequence is non-local, element type cannot be local.
  //
  if (!local && type->isLocal()) {
    /* commented by qinghua
    string msg = "non-local sequence `" + name + "' cannot have local element type";
    _unit->error(msg);
    */
  }

  ListPtr p = new List(this, name, type, local);
  _contents.push_back(p);
  return p;
}

SequencePtr Slice::Container::createSequence(const string& name, const TypePtr& type, bool local)
{
  checkPrefix(name);

  if (_unit->profile() == IceE && !local) {
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if ((builtin && builtin->kind() == Builtin::KindObject) || ClassDeclPtr::dynamicCast(type)) {
      /* commented by qinghua
      string msg = "Sequence `" + name + "' cannot contain object values.";
      _unit->error(msg);
      return 0;
      */
    }
  }

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    SequencePtr p = SequencePtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (matches.front()->name() == name) {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
      msg += "' as sequence";
      _unit->error(msg);
    } else if (!_unit->caseSensitive()) {
      string msg = "sequence `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " `" + matches.front()->name() + "'";
      _unit->error(msg);
    }
  }

  nameIsLegal(name, "sequence"); // Don't return here -- we create the sequence anyway.

  checkForGlobalDef(name, "sequence"); // Don't return here -- we create the sequence anyway.

  //
  // If sequence is non-local, element type cannot be local.
  //
  if (!local && type->isLocal()) {
    /* commented by qinghua
    string msg = "non-local sequence `" + name + "' cannot have local element type";
    _unit->error(msg);
    */
  }

  SequencePtr p = new Sequence(this, name, type, local);
  _contents.push_back(p);
  return p;
}

DictionaryPtr Slice::Container::createDictionary(const string& name,
                                                 const TypePtr& keyType,
                                                 const TypePtr& valueType,
                                                 bool local)
{
  checkPrefix(name);

  if (_unit->profile() == IceE && !local) {
    BuiltinPtr builtin = BuiltinPtr::dynamicCast(valueType);
    if ((builtin && builtin->kind() == Builtin::KindObject) || ClassDeclPtr::dynamicCast(valueType)) {
      /* commented by qinghua
      string msg = "Dictionary `" + name + "' cannot contain object values.";
      _unit->error(msg);
      return 0;
      */
    }
  }

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    DictionaryPtr p = DictionaryPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (matches.front()->name() == name) {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
      msg += "' as dictionary";
      _unit->error(msg);
    } else if (!_unit->caseSensitive()) {
      string msg = "dictionary `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " `" + matches.front()->name() + "'";
      _unit->error(msg);
    }
  }

  nameIsLegal(name, "dictionary"); // Don't return here -- we create the dictionary anyway.

  checkForGlobalDef(name, "dictionary"); // Don't return here -- we create the dictionary anyway.

  if (!Dictionary::legalKeyType(keyType)) {
    _unit->error("dictionary `" + name + "' uses an illegal key type");
    return 0;
  }

  if (!local) {
    if (keyType->isLocal()) {
      string msg = "non-local dictionary `" + name + "' cannot have local key type";
      _unit->error(msg);
    }
    if (valueType->isLocal()) {
      string msg = "non-local dictionary `" + name + "' cannot have local value type";
      _unit->error(msg);
    }
  }

  DictionaryPtr p = new Dictionary(this, name, keyType, valueType, local);
  _contents.push_back(p);
  return p;
}

EnumPtr Slice::Container::createEnum(const string& name, bool local)
{
  checkPrefix(name);

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    EnumPtr p = EnumPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (matches.front()->name() == name) {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
      msg += "' as enumeration";
      _unit->error(msg);
    } else if (!_unit->caseSensitive()) {
      string msg = "enumeration `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " `" + matches.front()->name() + "'";
      _unit->error(msg);
    }
  }

  nameIsLegal(name, "enumeration"); // Don't return here -- we create the enumeration anyway.

  checkForGlobalDef(name, "enumeration"); // Don't return here -- we create the enumeration anyway.

  EnumPtr p = new Enum(this, name, local);
  _contents.push_back(p);
  return p;
}

EnumeratorPtr Slice::Container::createEnumerator(const string& name)
{
  checkPrefix(name);

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    EnumeratorPtr p = EnumeratorPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (matches.front()->name() == name) {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
      msg += "' as enumerator";
      _unit->error(msg);
    } else if (!_unit->caseSensitive()) {
      string msg = "enumerator `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " `" + matches.front()->name() + "'";
      _unit->error(msg);
    }
  }

  nameIsLegal(name, "enumerator"); // Don't return here -- we create the enumerator anyway.

  EnumeratorPtr p = new Enumerator(this, name);
  _contents.push_back(p);
  return p;
}

ConstPtr Slice::Container::createConst(const string name,
                                       const TypePtr& constType,
                                       const SyntaxTreeBasePtr& literalType,
                                       const string& value)
{
  checkPrefix(name);

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    ConstPtr p = ConstPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (matches.front()->name() == name) {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
      msg += "' as constant";
      _unit->error(msg);
    } else if (!_unit->caseSensitive()) {
      string msg = "constant `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " `" + matches.front()->name() + "'";
      _unit->error(msg);
    }
  }

  nameIsLegal(name, "constant"); // Don't return here -- we create the constant anyway.

  checkForGlobalDef(name, "constant"); // Don't return here -- we create the constant anyway.

  //
  // Check that the constant type is legal.
  //
  if (!Const::isLegalType(name, constType, _unit)) {
    return 0;
  }

  //
  // Check that the type of the constant is compatible with the type of the initializer.
  //
  if (!Const::typesAreCompatible(name, constType, literalType, value, _unit)) {
    return 0;
  }

  //
  // Check that the initializer is in range.
  //
  if (!Const::isInRange(name, constType, value, _unit)) {
    return 0;
  }

  ConstPtr p = new Const(this, name, constType, value);
  _contents.push_back(p);
  return p;
}

TypeList Slice::Container::lookupType(const string& scoped, bool printError)
{
  //
  // Remove whitespace.
  //
  string sc = scoped;
  string::size_type pos;
  while ((pos = sc.find_first_of(" \t\r\n")) != string::npos) {
    sc.erase(pos, 1);
  }

  //
  // Check for builtin type.
  //
  for (unsigned int i = 0; i < sizeof(Builtin::builtinTable) / sizeof(const char*); ++i) {
    if (sc == Builtin::builtinTable[i]) {
      TypeList result;
      result.push_back(_unit->builtin(static_cast<Builtin::Kind> (i)));
      return result;
    }
  }

  //
  // Not a builtin type, try to look up a constructed type.
  //
  return lookupTypeNoBuiltin(scoped, printError);
}

TypeList Slice::Container::lookupTypeNoBuiltin(const string& scoped, bool printError)
{
  //
  // Remove whitespace.
  //
  string sc = scoped;
  string::size_type pos;
  while ((pos = sc.find_first_of(" \t\r\n")) != string::npos) {
    sc.erase(pos, 1);
  }

  //
  // Absolute scoped name?
  //
  if (sc.size() >= 2 && sc[0] == ':') {
    return _unit->lookupTypeNoBuiltin(sc.substr(2), printError);
  }

  TypeList results;
  if (sc.rfind('*') == sc.length() - 1) {
    //
    // Proxies.
    //
    ContainedList matches = _unit->findContents(thisScope() + sc.substr(0, sc.length() - 1));
    for (ContainedList::const_iterator p = matches.begin(); p != matches.end(); ++p) {
      ClassDefPtr def = ClassDefPtr::dynamicCast(*p);
      if (def) {
        continue; // Ignore class definitions.
      }

      if (printError && !_unit->caseSensitive() && matches.front()->scoped() != (thisScope() + sc)) {
        string msg = (*p)->kindOf() + " name `" + scoped;
        msg += "' is capitalized inconsistently with its previous name: `";
        msg += matches.front()->scoped() + "'";
        _unit->error(msg);
      }

      ClassDeclPtr cl = ClassDeclPtr::dynamicCast(*p);
      if (!cl) {
        if (printError) {
          string msg = "`";
          msg += sc;
          msg += "' must be class or interface";
          _unit->error(msg);
        }
        return TypeList();
      }
      results.push_back(new Proxy(cl));
    }
  } else {
    //
    // Non-Proxies.
    //
    ContainedList matches = _unit->findContents(thisScope() + sc);
    for (ContainedList::const_iterator p = matches.begin(); p != matches.end(); ++p) {
      ClassDefPtr def = ClassDefPtr::dynamicCast(*p);
      if (def) {
        continue; // Ignore class definitions.
      }

      if (printError && !_unit->caseSensitive() && matches.front()->scoped() != (thisScope() + sc)) {
        string msg = (*p)->kindOf() + " name `" + scoped;
        msg += "' is capitalized inconsistently with its previous name: `";
        msg += matches.front()->scoped() + "'";
        _unit->error(msg);
      }

      ExceptionPtr ex = ExceptionPtr::dynamicCast(*p);
      if (ex) {
        if (printError) {
          string msg = "`";
          msg += sc;
          msg += "' is an exception, which cannot be used as a type";
          _unit->error(msg);
        }
        return TypeList();
      }

      TypePtr type = TypePtr::dynamicCast(*p);
      if (!type) {
        if (printError) {
          string msg = "`";
          msg += sc;
          msg += "' is not a type";
          _unit->error(msg);
        }
        return TypeList();
      }
      results.push_back(type);
    }
  }

  if (results.empty()) {
    ContainedPtr contained = ContainedPtr::dynamicCast(this);
    if (!contained) {
      if (printError) {
        string msg = "`";
        msg += sc;
        msg += "' is not defined";
        _unit->error(msg);
      }
      return TypeList();
    }
    return contained->container()->lookupTypeNoBuiltin(sc, printError);
  } else {
    return results;
  }
}

ContainedList Slice::Container::lookupContained(const string& scoped, bool printError)
{
  //
  // Remove whitespace.
  //
  string sc = scoped;
  string::size_type pos;
  while ((pos = sc.find_first_of(" \t\r\n")) != string::npos) {
    sc.erase(pos, 1);
  }

  //
  // Absolute scoped name?
  //
  if (sc.size() >= 2 && sc[0] == ':') {
    return _unit->lookupContained(sc.substr(2), printError);
  }

  ContainedList matches = _unit->findContents(thisScope() + sc);
  ContainedList results;
  for (ContainedList::const_iterator p = matches.begin(); p != matches.end(); ++p) {
    if (!ClassDefPtr::dynamicCast(*p)) // Ignore class definitions.
    {
      results.push_back(*p);

      if (printError && !_unit->caseSensitive() && (*p)->scoped() != (thisScope() + sc)) {
        string msg = (*p)->kindOf() + " name `" + scoped;
        msg += "' is capitalized inconsistently with its previous name: `" + (*p)->scoped() + "'";
        _unit->error(msg);
      }
    }
  }

  if (results.empty()) {
    ContainedPtr contained = ContainedPtr::dynamicCast(this);
    if (!contained) {
      if (printError) {
        string msg = "`";
        msg += sc;
        msg += "' is not defined";
        _unit->error(msg);
      }
      return ContainedList();
    }
    return contained->container()->lookupContained(sc, printError);
  } else {
    return results;
  }
}

ExceptionPtr Slice::Container::lookupException(const string& scoped, bool printError)
{
  ContainedList contained = lookupContained(scoped, printError);
  if (contained.empty()) {
    return 0;
  }

  ExceptionList exceptions;
  for (ContainedList::iterator p = contained.begin(); p != contained.end(); ++p) {
    ExceptionPtr ex = ExceptionPtr::dynamicCast(*p);
    if (!ex) {
      if (printError) {
        string msg = "`";
        msg += scoped;
        msg += "' is not an exception";
        _unit->error(msg);
      }
      return 0;
    }
    exceptions.push_back(ex);
  }
  assert(exceptions.size() == 1);
  return exceptions.front();
}

ModuleList Slice::Container::modules() const
{
  ModuleList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ModulePtr q = ModulePtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

ClassList Slice::Container::classes() const
{
  ClassList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ClassDefPtr q = ClassDefPtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

ExceptionList Slice::Container::exceptions() const
{
  ExceptionList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ExceptionPtr q = ExceptionPtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

StructList Slice::Container::structs() const
{
  StructList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    StructPtr q = StructPtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

SequenceList Slice::Container::sequences() const
{
  SequenceList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    SequencePtr q = SequencePtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

DictionaryList Slice::Container::dictionaries() const
{
  DictionaryList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    DictionaryPtr q = DictionaryPtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

EnumList Slice::Container::enums() const
{
  EnumList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    EnumPtr q = EnumPtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

bool Slice::Container::hasNonLocalClassDecls() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ClassDeclPtr cl = ClassDeclPtr::dynamicCast(*p);
    if (cl && !cl->isLocal()) {
      return true;
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasNonLocalClassDecls()) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasNonLocalClassDefs() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ClassDefPtr cl = ClassDefPtr::dynamicCast(*p);
    if (cl && !cl->isLocal()) {
      return true;
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasNonLocalClassDefs()) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasNonLocalSequences() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    SequencePtr s = SequencePtr::dynamicCast(*p);
    if (s && !s->isLocal()) {
      return true;
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasNonLocalSequences()) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasNonLocalDictionaries() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    DictionaryPtr d = DictionaryPtr::dynamicCast(*p);
    if (d && !d->isLocal()) {
      return true;
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasNonLocalDictionaries()) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasNonLocalExceptions() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ExceptionPtr q = ExceptionPtr::dynamicCast(*p);
    if (q && !q->isLocal()) {
      return true;
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasNonLocalExceptions()) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasClassDecls() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    if (ClassDeclPtr::dynamicCast(*p)) {
      return true;
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasClassDecls()) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasClassDefs() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    if (ClassDefPtr::dynamicCast(*p)) {
      return true;
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasClassDefs()) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasAbstractClassDefs() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ClassDefPtr cl = ClassDefPtr::dynamicCast(*p);
    if (cl && cl->isAbstract()) {
      return true;
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasAbstractClassDefs()) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasDataOnlyClasses() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ClassDefPtr q = ClassDefPtr::dynamicCast(*p);
    if (q) {
      if (!q->isAbstract()) {
        return true;
      }
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasDataOnlyClasses()) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasOtherConstructedOrExceptions() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    if (ConstructedPtr::dynamicCast(*p) && !ClassDeclPtr::dynamicCast(*p)
        && !ClassDefPtr::dynamicCast(*p)) {
      return true;
    }

    if (ExceptionPtr::dynamicCast(*p)) {
      return true;
    }

    if (ConstPtr::dynamicCast(*p)) {
      return true;
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasOtherConstructedOrExceptions()) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasContentsWithMetaData(const string& meta) const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    if ((*p)->hasMetaData(meta)) {
      return true;
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasContentsWithMetaData(meta)) {
      return true;
    }
  }

  return false;
}

bool Slice::Container::hasAsyncOps() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ClassDefPtr cl = ClassDefPtr::dynamicCast(*p);
    if (cl && !cl->isLocal()) {
      OperationList ops = cl->operations();
      if (!ops.empty() && (cl->hasMetaData("ami") || cl->hasMetaData("amd"))) {
        return true;
      }
      for (OperationList::const_iterator i = ops.begin(); i != ops.end(); ++i) {
        OperationPtr op = *i;
        if (op->hasMetaData("ami") || op->hasMetaData("amd")) {
          return true;
        }
      }
    }

    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container && container->hasAsyncOps()) {
      return true;
    }
  }

  return false;
}

string Slice::Container::thisScope() const
{
  string s;
  ContainedPtr contained = ContainedPtr::dynamicCast(const_cast<Container*> (this));
  if (contained) {
    s = contained->scoped();
  }
  s += "::";
  return s;
}

void Slice::Container::mergeModules()
{
  for (ContainedList::iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ModulePtr mod1 = ModulePtr::dynamicCast(*p);
    if (!mod1) {
      continue;
    }

    DefinitionContextPtr dc1 = mod1->definitionContext();
    assert(dc1);
    StringList metaData1 = dc1->getMetaData();
    metaData1.sort();
    metaData1.unique();

    ContainedList::iterator q = p;
    ++q;
    while (q != _contents.end()) {
      ModulePtr mod2 = ModulePtr::dynamicCast(*q);
      if (!mod2) {
        ++q;
        continue;
      }

      if (mod1->name() != mod2->name()) {
        ++q;
        continue;
      }

      //
      // Compare the global metadata of the two modules being merged.
      //
      DefinitionContextPtr dc2 = mod2->definitionContext();
      assert(dc2);
      StringList metaData2 = dc2->getMetaData();
      metaData2.sort();
      metaData2.unique();
      if (metaData1 != metaData2) {
        unit()->warning("global metadata mismatch for module `" + mod1->name() + "' in files "
            + dc1->filename() + " and " + dc2->filename());
      }

      mod1->_contents.splice(mod1->_contents.end(), mod2->_contents);

      if (mod1->_comment.length() < mod2->_comment.length()) {
        mod1->_comment.swap(mod2->_comment);
      }

      mod1->_includeLevel = min(mod1->_includeLevel, mod2->_includeLevel);

      _unit->removeContent(*q);
      q = _contents.erase(q);
    }

    mod1->mergeModules();
  }
}

void Slice::Container::sort()
{
  _contents.sort();
}

void Slice::Container::sortContents(bool sortFields)
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ContainerPtr container = ContainerPtr::dynamicCast(*p);
    if (container) {
      if (!sortFields) {
        if (StructPtr::dynamicCast(container) || ClassDefPtr::dynamicCast(container)
            || ExceptionPtr::dynamicCast(container)) {
          continue;
        }
      }
      //
      // Don't sort operation definitions, otherwise parameters are shown in the
      // wrong order in the synopsis.
      //
      if (!OperationPtr::dynamicCast(container)) {
        container->sort();
      }
      container->sortContents(sortFields);
    }
  }
}

void Slice::Container::visit(ParserVisitor* visitor, bool all)
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    if (all || (*p)->includeLevel() == 0) {
      (*p)->visit(visitor, all);
    }
  }
}

void Slice::Container::containerRecDependencies(set<ConstructedPtr>& dependencies)
{
  ContainedList::iterator p;
  for (p = _contents.begin(); p != _contents.end(); ++p) {
    ConstructedPtr constructed = ConstructedPtr::dynamicCast(*p);
    if (constructed && dependencies.find(constructed) != dependencies.end()) {
      dependencies.insert(constructed);
      constructed->recDependencies(dependencies);
    }
  }
}

bool Slice::Container::checkIntroduced(const string& scoped, ContainedPtr namedThing)
{
  if (scoped[0] == ':') // Only unscoped names introduce anything.
  {
    return true;
  }

  //
  // Split off first component.
  //
  string::size_type pos = scoped.find("::");
  string firstComponent = pos == string::npos ? scoped : scoped.substr(0, pos);

  //
  // If we don't have a type, the thing that is introduced is the contained for
  // the first component.
  //
  if (namedThing == 0) {
    ContainedList cl = lookupContained(firstComponent, false);
    if (namedThing == 0) {
      if (cl.empty()) {
        return true; // Ignore types whose creation failed previously.
      }
    }
    namedThing = cl.front();
  } else {
    //
    // For each scope, get the container until we have the container
    // for the first scope (which is the introduced one).
    //
    ContainerPtr c;
    bool first = true;
    while (pos != string::npos) {
      if (first) {
        c = namedThing->container();
      } else {
        ContainedPtr contained = ContainedPtr::dynamicCast(c);
        if (contained) {
          c = contained->container();
        }
      }
      first = false;
      if (pos != string::npos) {
        pos = scoped.find("::", pos + 2);
      }
    }
    if (ContainedPtr::dynamicCast(c)) {
      namedThing = ContainedPtr::dynamicCast(c);
    }
  }

  //
  // Check if the first component is in the introduced map of this scope.
  //
  map<string, ContainedPtr, CICompare>::const_iterator it = _introducedMap.find(firstComponent);
  if (it == _introducedMap.end()) {
    //
    // We've just introduced the first component to the current scope.
    //
    _introducedMap[firstComponent] = namedThing; // No, insert it
  } else {
    //
    // We've previously introduced the first component to the current scope,
    // check that it has not changed meaning.
    //
    if (!_unit->caseSensitive() && it->second != namedThing) {
      _unit->error("`" + firstComponent + "' has changed meaning");
      return false;
    }
  }
  return true;
}

bool Slice::Container::nameIsLegal(const string& newName, const char* newConstruct)
{
  ModulePtr module = ModulePtr::dynamicCast(this);

  //
  // Check whether the enclosing module has the same name.
  //
  if (module) {
    if (newName == module->name()) {
      string msg = newConstruct;
      msg += " name `" + newName
          + "' must differ from the name of its immediately enclosing module";
      _unit->error(msg);
      return false;
    }
    if (!_unit->caseSensitive()) {
      string name = newName;
      toLower(name);
      string thisName = module->name();
      toLower(thisName);
      if (name == thisName) {
        string msg = newConstruct;
        msg += " name `" + name
            + "' cannot differ only in capitalization from its immediately enclosing "
              "module name `" + module->name() + "'";
        _unit->error(msg);
        return false;
      }
    }
    module = ModulePtr::dynamicCast(module->container()); // Get enclosing module for test below.
  }

  //
  // Check whether any of the enclosing modules have the same name.
  //
  while (module) {
    if (newName == module->name()) {
      string msg = newConstruct;
      msg += " name `" + newName + "' must differ from the name of enclosing module `"
          + module->name() + "' (first defined at " + module->file() + ":" + module->line() + ")";
      _unit->error(msg);
      return false;
    }
    if (!_unit->caseSensitive()) {
      string name = newName;
      toLower(name);
      string thisName = module->name();
      toLower(thisName);
      if (name == thisName) {
        string msg = newConstruct;
        msg += " name `" + name + "' cannot differ only in capitalization from enclosing module `"
            + module->name() + "' (first defined at " + module->file() + ":" + module->line() + ")";
        _unit->error(msg);
        return false;
      }
    }
    module = ModulePtr::dynamicCast(module->container());
  }

  return true;
}

bool Slice::Container::checkForGlobalDef(const string& name, const char* newConstruct)
{
  if (dynamic_cast<Unit*> (this) && strcmp(newConstruct, "module")) {
    static const string vowels = "aeiou";
    string glottalStop;
    if (vowels.find_first_of(newConstruct[0]) != string::npos) {
      glottalStop = "n";
    }
    _unit->error("`" + name + "': a" + glottalStop + " " + newConstruct
        + " can be defined only at module scope");
    return false;
  }
  return true;
}

Slice::Container::Container(const UnitPtr& unit) :
  SyntaxTreeBase(unit)
{
}

bool Slice::Container::checkInterfaceAndLocal(const string& name,
                                              bool defined,
                                              bool intf,
                                              bool intfOther,
                                              bool local,
                                              bool localOther)
{
  string definedOrDeclared;
  if (defined) {
    definedOrDeclared = "defined";
  } else {
    definedOrDeclared = "declared";
  }

  if (!intf && intfOther) {
    string msg = "class `";
    msg += name;
    msg += "' was ";
    msg += definedOrDeclared;
    msg += " as interface";
    _unit->error(msg);
    return false;
  }

  if (intf && !intfOther) {
    string msg = "interface `";
    msg += name;
    msg += "' was ";
    msg += definedOrDeclared;
    msg += " as class";
    _unit->error(msg);
    return false;
  }

  if (!local && localOther) {
    string msg = "non-local `";
    msg += name;
    msg += "' was ";
    msg += definedOrDeclared;
    msg += " local";
    _unit->error(msg);
    return false;
  }

  if (local && !localOther) {
    string msg = "local `";
    msg += name;
    msg += "' was ";
    msg += definedOrDeclared;
    msg += " non-local";
    _unit->error(msg);
    return false;
  }

  return true;
}

void Slice::Container::checkPrefix(const string& name) const
{
  if (_unit->currentIncludeLevel() == 0 && !_unit->allowIcePrefix()) {
    if (name.size() >= 3) {
      string prefix3;
      prefix3 += ::tolower(name[0]);
      prefix3 += ::tolower(name[1]);
      prefix3 += ::tolower(name[2]);
      if (prefix3 == "ice") {
        _unit->error("illegal identifier `" + name + "': `" + name.substr(0, 3)
            + "' prefix is reserved");
      }
    }
  }
}

// ----------------------------------------------------------------------
// Module
// ----------------------------------------------------------------------

Contained::ContainedType Slice::Module::containedType() const
{
  return ContainedTypeModule;
}

bool Slice::Module::uses(const ContainedPtr&) const
{
  return false;
}

string Slice::Module::kindOf() const
{
  return "module";
}

void Slice::Module::visit(ParserVisitor* visitor, bool all)
{
  if (visitor->visitModuleStart(this)) {
    Container::visit(visitor, all);
    visitor->visitModuleEnd(this);
  }
}

Slice::Module::Module(const ContainerPtr& container, const string& name) :
  SyntaxTreeBase(container->unit()), Container(container->unit()), Contained(container, name)
{
}

// ----------------------------------------------------------------------
// Constructed
// ----------------------------------------------------------------------

bool Slice::Constructed::isLocal() const
{
  return _local;
}

string Slice::Constructed::typeId() const
{
  return scoped();
}

ConstructedList Slice::Constructed::dependencies()
{
  set<ConstructedPtr> resultSet;
  recDependencies(resultSet);

#if defined(__SUNPRO_CC) && defined(_RWSTD_NO_MEMBER_TEMPLATES)
  // TODO: find a more usable work-around for this std lib limitation.
  ConstructedList result;
  set<ConstructedPtr>::iterator it = resultSet.begin();
  while(it != resultSet.end())
  {
    result.push_back(*it++);
  }
  return result;
#else
  return ConstructedList(resultSet.begin(), resultSet.end());
#endif
}

Slice::Constructed::Constructed(const ContainerPtr& container, const string& name, bool local) :
  SyntaxTreeBase(container->unit()), Type(container->unit()), Contained(container, name),
      _local(local)
{
}

// ----------------------------------------------------------------------
// ClassDecl
// ----------------------------------------------------------------------

void Slice::ClassDecl::destroy()
{
  _definition = 0;
  SyntaxTreeBase::destroy();
}

ClassDefPtr Slice::ClassDecl::definition() const
{
  return _definition;
}

bool Slice::ClassDecl::isInterface() const
{
  return _interface;
}

Contained::ContainedType Slice::ClassDecl::containedType() const
{
  return ContainedTypeClass;
}

bool Slice::ClassDecl::uses(const ContainedPtr&) const
{
  return false;
}

bool Slice::ClassDecl::usesClasses() const
{
  return true;
}

size_t Slice::ClassDecl::minWireSize() const
{
  return 4; // At least four bytes for an instance, if the instance is marshaled as an index.
}

bool Slice::ClassDecl::isVariableLength() const
{
  return true;
}

string Slice::ClassDecl::kindOf() const
{
  string s;
  if (isLocal()) {
    s += "local ";
  }
  s += _interface ? "interface" : "class";
  return s;
}

void Slice::ClassDecl::visit(ParserVisitor* visitor, bool)
{
  visitor->visitClassDecl(this);
}

void Slice::ClassDecl::recDependencies(set<ConstructedPtr>& dependencies)
{
  if (_definition) {
    _definition->containerRecDependencies(dependencies);
    ClassList bases = _definition->bases();
    ClassList::iterator p;
    for (p = bases.begin(); p != bases.end(); ++p) {
      (*p)->declaration()->recDependencies(dependencies);
    }
  }
}

void Slice::ClassDecl::checkBasesAreLegal(const string& name,
                                          bool intf,
                                          bool local,
                                          const ClassList& bases,
                                          const UnitPtr& unit)
{
  //
  // Local definitions cannot have non-local bases, and vice versa.
  //
  for (ClassList::const_iterator p = bases.begin(); p != bases.end(); ++p) {
    if (local != (*p)->isLocal()) {
      ostringstream msg;
      msg << (local ? "local" : "non-local") << " " << (intf ? "interface" : "class") << " `"
          << name << "' cannot have " << ((*p)->isLocal() ? "local" : "non-local") << " base "
          << ((*p)->isInterface() ? "interface" : "class") << " `" << (*p)->name() << "'";
      unit->error(msg.str());
    }
  }

  //
  // Check whether, for multiple inheritance, any of the bases define
  // the same operations.
  //
  if (bases.size() > 1) {
    //
    // We have multiple inheritance. Build a list of paths through the
    // inheritance graph, such that multiple inheritance is legal if
    // the union of the names defined in classes on each path are disjoint.
    //
    GraphPartitionList gpl;
    for (ClassList::const_iterator p = bases.begin(); p != bases.end(); ++p) {
      ClassList cl;
      gpl.push_back(cl);
      addPartition(gpl, gpl.rbegin(), *p);
    }

    //
    // We now have a list of partitions, with each partition containing
    // a list of class definitions. Turn the list of partitions of class
    // definitions into a list of sets of strings, with each
    // set containing the names of operations and data members defined in
    // the classes in each partition.
    //
    StringPartitionList spl = toStringPartitionList(gpl);

    //
    // Multiple inheritance is legal if no two partitions contain a common
    // name (that is, if the union of the intersections of all possible pairs
    // of partitions is empty).
    //
    checkPairIntersections(spl, name, unit);
  }
}

Slice::ClassDecl::ClassDecl(const ContainerPtr& container,
                            const string& name,
                            bool intf,
                            bool local) :
  SyntaxTreeBase(container->unit()), Type(container->unit()), Contained(container, name),
      Constructed(container, name, local), _interface(intf)
{
  _unit->currentContainer();
}

//
// Return true if the class definition cdp is on one of the class lists in gpl, false otherwise.
//
bool Slice::ClassDecl::isInList(const GraphPartitionList& gpl, const ClassDefPtr cdp)
{
  for (GraphPartitionList::const_iterator i = gpl.begin(); i != gpl.end(); ++i) {
    if (find(i->begin(), i->end(), cdp) != i->end()) {
      return true;
    }
  }
  return false;
}

void Slice::ClassDecl::addPartition(GraphPartitionList& gpl,
                                    GraphPartitionList::reverse_iterator tail,
                                    const ClassDefPtr base)
{
  //
  // If this base is on one of the partition lists already, do nothing.
  //
  if (isInList(gpl, base)) {
    return;
  }
  //
  // Put the current base at the end of the current partition.
  //
  tail->push_back(base);
  //
  // If the base has bases in turn, recurse, adding the first base
  // of base (the left-most "grandbase") to the current partition.
  //
  if (base->bases().size()) {
    addPartition(gpl, tail, *(base->bases().begin()));
  }
  //
  // If the base has multiple bases, each of the "grandbases"
  // except for the left-most (which we just dealt with)
  // adds a new partition.
  //
  if (base->bases().size() > 1) {
    ClassList grandBases = base->bases();
    ClassList::const_iterator i = grandBases.begin();
    while (++i != grandBases.end()) {
      ClassList cl;
      gpl.push_back(cl);
      addPartition(gpl, gpl.rbegin(), *i);
    }
  }
}

//
// Convert the list of partitions of class definitions into a
// list of lists, with each member list containing the operation
// names defined by the interfaces in each partition.
//
Slice::ClassDecl::StringPartitionList Slice::ClassDecl::toStringPartitionList(const GraphPartitionList& gpl)
{
  StringPartitionList spl;
  for (GraphPartitionList::const_iterator i = gpl.begin(); i != gpl.end(); ++i) {
    StringList sl;
    spl.push_back(sl);
    for (ClassList::const_iterator j = i->begin(); j != i->end(); ++j) {
      OperationList operations = (*j)->operations();
      for (OperationList::const_iterator l = operations.begin(); l != operations.end(); ++l) {
        spl.rbegin()->push_back((*l)->name());
      }
    }
  }
  return spl;
}

//
// For all (unique) pairs of string lists, check whether an identifier in one list occurs
// in the other and, if so, complain.
//
void Slice::ClassDecl::checkPairIntersections(const StringPartitionList& l,
                                              const string& name,
                                              const UnitPtr& unit)
{
  set<string> reported;
  for (StringPartitionList::const_iterator i = l.begin(); i != l.end(); ++i) {
    StringPartitionList::const_iterator cursor = i;
    ++cursor;
    for (StringPartitionList::const_iterator j = cursor; j != l.end(); ++j) {
      for (StringList::const_iterator s1 = i->begin(); s1 != i->end(); ++s1) {
        for (StringList::const_iterator s2 = j->begin(); s2 != j->end(); ++s2) {
          if ((*s1) == (*s2) && reported.find(*s1) == reported.end()) {
            string msg = "ambiguous multiple inheritance: `" + name;
            msg += "' inherits operation `" + *s1 + "' from two or more unrelated base interfaces";
            unit->error(msg);
            reported.insert(*s1);
          } else if (!unit->caseSensitive() && !CICompare()(*s1, *s2) && !CICompare()(*s2, *s1)
              && reported.find(*s1) == reported.end() && reported.find(*s2) == reported.end()) {
            string msg = "ambiguous multiple inheritance: `" + name;
            msg += "' inherits operations `" + *s1 + "' and `" + *s2;
            msg += "', which differ only in capitalization, from unrelated base interfaces";
            unit->error(msg);
            reported.insert(*s1);
            reported.insert(*s2);
          }
        }
      }
    }
  }
}

// ----------------------------------------------------------------------
// ClassDef
// ----------------------------------------------------------------------

void Slice::ClassDef::destroy()
{
  _declaration = 0;
  _bases.empty();
  Container::destroy();
}

OperationPtr Slice::ClassDef::createOperation(const string& name,
                                              const TypePtr& returnType,
                                              Operation::Mode mode)
{
  checkPrefix(name);

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    OperationPtr p = OperationPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (!_unit->caseSensitive() && matches.front()->name() != name) {
      string msg = "operation `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " `" + matches.front()->name() + "'";
      _unit->error(msg);
    }
    string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
    msg += "' as operation `" + name + "'";
    _unit->error(msg);
    return 0;
  }

  //
  // Check whether enclosing interface/class has the same name.
  //
  if (name == this->name()) {
    string msg = isInterface() ? "interface" : "class";
    msg += " name `" + name + "' cannot be used as operation name";
    _unit->error(msg);
    return 0;
  }
  if (!_unit->caseSensitive()) {
    string newName = name;
    toLower(newName);
    string thisName = this->name();
    toLower(thisName);
    if (newName == thisName) {
      string msg = "operation `" + name + "' differs only in capitalization from enclosing ";
      msg += isInterface() ? "interface" : "class";
      msg += " name `" + this->name() + "'";
      _unit->error(msg);
    }
  }

  //
  // Check whether any bases have defined something with the same name already.
  //
  for (ClassList::const_iterator p = _bases.begin(); p != _bases.end(); ++p) {
    ContainedList cl;
    OperationList ol = (*p)->allOperations();
    copy(ol.begin(), ol.end(), back_inserter(cl));
    DataMemberList dml = (*p)->allDataMembers();
    copy(dml.begin(), dml.end(), back_inserter(cl));
    for (ContainedList::const_iterator q = cl.begin(); q != cl.end(); ++q) {
      if ((*q)->name() == name) {
        string msg = "operation `" + name;
        msg += "' is already defined as a";
        static const string vowels = "aeiou";
        string kindOf = (*q)->kindOf();
        if (vowels.find_first_of(kindOf[0]) != string::npos) {
          msg += "n";
        }
        msg += " " + kindOf + " in a base interface or class";
        _unit->error(msg);
        return 0;
      }
      if (!_unit->caseSensitive()) {
        string baseName = (*q)->name();
        toLower(baseName);
        string newName = name;
        toLower(newName);
        if (baseName == newName) {
          string msg = "operation `" + name + "' differs only in capitalization from "
              + (*q)->kindOf();
          msg += " `" + (*q)->name() + "', which is defined in a base interface or class";
          _unit->error(msg);
        }
      }
    }
  }

  //
  // Non-local class/interface cannot have operation with local return type.
  //
  if (!isLocal() && returnType && returnType->isLocal()) {
    /* by qinghua
    string msg = "non-local " + this->kindOf() + " `" + this->name() + "' cannot have operation `";
    msg += name + "' with local return type";
    _unit->error(msg);
    */
  }

  _hasOperations = true;
  OperationPtr op = new Operation(this, name, returnType, mode);
  _contents.push_back(op);
  return op;
}

DataMemberPtr Slice::ClassDef::createDataMember(const string& name, const TypePtr& type)
{
  checkPrefix(name);

  if (_unit->profile() == IceE) {
    if (!isLocal()) {
      BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
      if ((builtin && builtin->kind() == Builtin::KindObject)) {
        string msg = "Class data member `" + name + "' cannot be a value object.";
        _unit->error(msg);
        return 0;
      }

      ClassDeclPtr classDecl = ClassDeclPtr::dynamicCast(type);
      if (classDecl != 0 && !classDecl->isLocal()) {
        string msg = "Class data member `" + name + "' cannot be a value object.";
        _unit->error(msg);
        return 0;
      }
    }
  }

  assert(!isInterface());
  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    DataMemberPtr p = DataMemberPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (!_unit->caseSensitive() && matches.front()->name() != name) {
      string msg = "data member `" + name + "' differs only in capitalization from ";
      msg += matches.front()->kindOf() + " `" + matches.front()->name() + "'";
      _unit->error(msg);
    } else {
      string msg = "redefinition of " + matches.front()->kindOf() + " `" + matches.front()->name();
      msg += "' as data member `" + name + "'";
      _unit->error(msg);
      return 0;
    }
  }

  //
  // Check whether enclosing class has the same name.
  //
  if (name == this->name()) {
    string msg = "class name `";
    msg += name;
    msg += "' cannot be used as data member name";
    _unit->error(msg);
    return 0;
  }
  if (!_unit->caseSensitive()) {
    string newName = name;
    toLower(newName);
    string thisName = this->name();
    toLower(thisName);
    if (newName == thisName) {
      string msg = "data member `" + name
          + "' differs only in capitalization from enclosing class name `";
      msg += this->name() + "'";
      _unit->error(msg);
    }
  }

  //
  // Check whether any bases have defined something with the same name already.
  //
  for (ClassList::const_iterator p = _bases.begin(); p != _bases.end(); ++p) {
    ContainedList cl;
    OperationList ol = (*p)->allOperations();
    copy(ol.begin(), ol.end(), back_inserter(cl));
    DataMemberList dml = (*p)->allDataMembers();
    copy(dml.begin(), dml.end(), back_inserter(cl));
    for (ContainedList::const_iterator q = cl.begin(); q != cl.end(); ++q) {
      if ((*q)->name() == name) {
        string msg = "data member `" + name;
        msg += "' is already defined as a";
        static const string vowels = "aeiou";
        string kindOf = (*q)->kindOf();
        if (vowels.find_first_of(kindOf[0]) != string::npos) {
          msg += "n";
        }
        msg += " " + kindOf + " in a base interface or class";
        _unit->error(msg);
        return 0;
      }
      if (!_unit->caseSensitive()) {
        string baseName = (*q)->name();
        toLower(baseName);
        string newName = name;
        toLower(newName);
        if (baseName == newName) {
          string msg = "data member `" + name + "' differs only in capitalization from "
              + (*q)->kindOf();
          msg += " `" + (*q)->name() + "', which is defined in a base interface or class";
          _unit->error(msg);
        }
      }
    }
  }

  //
  // If data member is local, enclosing class/interface must be local.
  //
  if (!isLocal() && type->isLocal()) {
    string msg = "non-local " + kindOf() + "`" + this->name() + "' cannot contain local member `"
        + name + "'";
    _unit->error(msg);
  }

  _hasDataMembers = true;
  DataMemberPtr member = new DataMember(this, name, type);
  _contents.push_back(member);
  return member;
}

ClassDeclPtr Slice::ClassDef::declaration() const
{
  return _declaration;
}

ClassList Slice::ClassDef::bases() const
{
  return _bases;
}

ClassList Slice::ClassDef::allBases() const
{
  ClassList result = _bases;
  result.sort();
  result.unique();
  for (ClassList::const_iterator p = _bases.begin(); p != _bases.end(); ++p) {
    ClassList li = (*p)->allBases();
    result.merge(li);
    result.unique();
  }
  return result;
}

OperationList Slice::ClassDef::operations() const
{
  OperationList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    OperationPtr q = OperationPtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

OperationList Slice::ClassDef::allOperations() const
{
  OperationList result = operations();
  result.sort();
  result.unique();
  for (ClassList::const_iterator p = _bases.begin(); p != _bases.end(); ++p) {
    OperationList li = (*p)->allOperations();
    result.merge(li);
    result.unique();
  }
  return result;
}

DataMemberList Slice::ClassDef::dataMembers() const
{
  DataMemberList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    DataMemberPtr q = DataMemberPtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

//
// Return the data members of this class and its parent classes, in base-to-derived order.
//
DataMemberList Slice::ClassDef::allDataMembers() const
{
  DataMemberList result;

  //
  // Check if we have a base class. If so, recursively
  // get the data members of the base(s).
  //
  if (!_bases.empty() && !_bases.front()->isInterface()) {
    result = _bases.front()->allDataMembers();
  }

  //
  // Append this class's data members.
  //
  DataMemberList myMembers = dataMembers();
  result.splice(result.end(), myMembers);

  return result;
}

DataMemberList Slice::ClassDef::classDataMembers() const
{
  DataMemberList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    DataMemberPtr q = DataMemberPtr::dynamicCast(*p);
    if (q) {
      BuiltinPtr builtin = BuiltinPtr::dynamicCast(q->type());
      if ((builtin && builtin->kind() == Builtin::KindObject)
          || ClassDeclPtr::dynamicCast(q->type())) {
        result.push_back(q);
      }
    }
  }
  return result;
}

//
// Return the class data members of this class and its parent classes, in base-to-derived order.
//
DataMemberList Slice::ClassDef::allClassDataMembers() const
{
  DataMemberList result;

  //
  // Check if we have a base class. If so, recursively
  // get the class data members of the base(s).
  //
  if (!_bases.empty() && !_bases.front()->isInterface()) {
    result = _bases.front()->allClassDataMembers();
  }

  //
  // Append this class's class members.
  //
  DataMemberList myMembers = classDataMembers();
  result.splice(result.end(), myMembers);

  return result;
}

bool Slice::ClassDef::canBeCyclic() const
{
  if (!_bases.empty() && !_bases.front()->isInterface() && _bases.front()->canBeCyclic()) {
    return true;
  }
  DataMemberList dml = dataMembers();
  for (DataMemberList::const_iterator i = dml.begin(); i != dml.end(); ++i) {
    if ((*i)->type()->usesClasses()) {
      return true;
    }
  }
  return false;
}

bool Slice::ClassDef::isAbstract() const
{
  if (isInterface() || _bases.size() > 1) // Is this an interface, or does it derive from interfaces?
  {
    return true;
  }

  if (!_bases.empty() && _bases.front()->isAbstract()) {
    return true;
  }

  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    if (OperationPtr::dynamicCast(*p)) {
      return true;
    }
  }

  return false;
}

bool Slice::ClassDef::isInterface() const
{
  return _interface;
}

bool Slice::ClassDef::isA(const string& id) const
{
  if (id == _scoped) {
    return true;
  }
  for (ClassList::const_iterator p = _bases.begin(); p != _bases.end(); ++p) {
    if ((*p)->isA(id)) {
      return true;
    }
  }
  return false;
}

bool Slice::ClassDef::isLocal() const
{
  return _local;
}

bool Slice::ClassDef::hasDataMembers() const
{
  return _hasDataMembers;
}

bool Slice::ClassDef::hasOperations() const
{
  return _hasOperations;
}

Contained::ContainedType Slice::ClassDef::containedType() const
{
  return ContainedTypeClass;
}

bool Slice::ClassDef::uses(const ContainedPtr&) const
{
  // No uses() implementation here. DataMember and Operation have
  // their own uses().
  return false;
}

string Slice::ClassDef::kindOf() const
{
  string s;
  if (isLocal()) {
    s += "local ";
  }
  s += isInterface() ? "interface" : "class";
  return s;
}

void Slice::ClassDef::visit(ParserVisitor* visitor, bool all)
{
  if (visitor->visitClassDefStart(this)) {
    Container::visit(visitor, all);
    visitor->visitClassDefEnd(this);
  }
}

Slice::ClassDef::ClassDef(const ContainerPtr& container,
                          const string& name,
                          bool intf,
                          const ClassList& bases,
                          bool local) :
  SyntaxTreeBase(container->unit()), Container(container->unit()), Contained(container, name),
      _interface(intf), _hasDataMembers(false), _hasOperations(false), _bases(bases), _local(local)
{
  //
  // First element of bases may be a class, all others must be
  // interfaces.
  //
#ifndef NDEBUG
  for (ClassList::const_iterator p = _bases.begin(); p != _bases.end(); ++p) {
    assert(p == _bases.begin() || (*p)->isInterface());
  }
#endif
}

// ----------------------------------------------------------------------
// Proxy
// ----------------------------------------------------------------------

bool Slice::Proxy::isLocal() const
{
  return __class->isLocal();
}

string Slice::Proxy::typeId() const
{
  return __class->scoped();
}

bool Slice::Proxy::usesClasses() const
{
  return false;
}

size_t Slice::Proxy::minWireSize() const
{
  return 2; // At least two bytes for a nil proxy (empty name and empty category strings).
}

bool Slice::Proxy::isVariableLength() const
{
  return true;
}

ClassDeclPtr Slice::Proxy::_class() const
{
  return __class;
}

Slice::Proxy::Proxy(const ClassDeclPtr& cl) :
  SyntaxTreeBase(cl->unit()), Type(cl->unit()), __class(cl)
{
}

// ----------------------------------------------------------------------
// Exception
// ----------------------------------------------------------------------

void Slice::Exception::destroy()
{
  _base = 0;
  Container::destroy();
}

DataMemberPtr Slice::Exception::createDataMember(const string& name, const TypePtr& type)
{
  checkPrefix(name);

  if (_unit->profile() == IceE) {
    if (!isLocal()) {
      BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
      if ((builtin && builtin->kind() == Builtin::KindObject)) {
        string msg = "Exception data member `" + name + "' cannot be a value object.";
        _unit->error(msg);
        return 0;
      }

      ClassDeclPtr classDecl = ClassDeclPtr::dynamicCast(type);
      if (classDecl != 0 && !classDecl->isLocal()) {
        string msg = "Exception data member `" + name + "' cannot be a value object.";
        _unit->error(msg);
        return 0;
      }
    }
  }

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    DataMemberPtr p = DataMemberPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (!_unit->caseSensitive() && matches.front()->name() != name) {
      string msg = "exception member `" + name + "' differs only in capitalization from ";
      msg += "exception member `" + matches.front()->name() + "'";
      _unit->error(msg);
    } else {
      string msg = "redefinition of exception member `" + name + "'";
      _unit->error(msg);
      return 0;
    }
  }

  //
  // Check whether enclosing exception has the same name.
  //
  if (name == this->name()) {
    string msg = "exception name `";
    msg += name;
    msg += "' cannot be used as exception member name";
    _unit->error(msg);
    return 0;
  }
  if (!_unit->caseSensitive()) {
    string newName = name;
    toLower(newName);
    string thisName = this->name();
    toLower(thisName);
    if (newName == thisName) {
      string msg = "exception member `" + name + "' differs only in capitalization ";
      msg += "from enclosing exception name `" + this->name() + "'";
      _unit->error(msg);
    }
  }

  //
  // Check whether any bases have defined a member with the same name already.
  //
  ExceptionList bl = allBases();
  for (ExceptionList::const_iterator q = bl.begin(); q != bl.end(); ++q) {
    ContainedList cl;
    DataMemberList dml = (*q)->dataMembers();
    copy(dml.begin(), dml.end(), back_inserter(cl));
    for (ContainedList::const_iterator r = cl.begin(); r != cl.end(); ++r) {
      if ((*r)->name() == name) {
        string msg = "exception member `" + name + "' is already defined in a base exception";
        _unit->error(msg);
        return 0;
      }
      if (!_unit->caseSensitive()) {
        string baseName = (*r)->name();
        toLower(baseName);
        string newName = name;
        toLower(newName);
        if (baseName == newName) {
          string msg = "exception member `" + name
              + "' differs only in capitalization from exception member `";
          msg += (*r)->name() + "', which is defined in a base exception";
          _unit->error(msg);
        }
      }
    }
  }

  //
  // If data member is local, enclosing class/interface must be local.
  //
  if (!isLocal() && type->isLocal()) {
    string msg = "non-local " + kindOf() + "`" + this->name() + "' cannot contain local member `"
        + name + "'";
    _unit->error(msg);
  }

  DataMemberPtr p = new DataMember(this, name, type);
  _contents.push_back(p);
  return p;
}

DataMemberList Slice::Exception::dataMembers() const
{
  DataMemberList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    DataMemberPtr q = DataMemberPtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

//
// Return the data members of this exception and its parent exceptions, in base-to-derived order.
//
DataMemberList Slice::Exception::allDataMembers() const
{
  DataMemberList result;

  //
  // Check if we have a base exception. If so, recursively
  // get the data members of the base exception(s).
  //
  if (base()) {
    result = base()->allDataMembers();
  }

  //
  // Append this exceptions's data members.
  //
  DataMemberList myMembers = dataMembers();
  result.splice(result.end(), myMembers);

  return result;
}

DataMemberList Slice::Exception::classDataMembers() const
{
  DataMemberList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    DataMemberPtr q = DataMemberPtr::dynamicCast(*p);
    if (q) {
      BuiltinPtr builtin = BuiltinPtr::dynamicCast(q->type());
      if ((builtin && builtin->kind() == Builtin::KindObject)
          || ClassDeclPtr::dynamicCast(q->type())) {
        result.push_back(q);
      }
    }
  }
  return result;
}

//
// Return the class data members of this exception and its parent exceptions, in base-to-derived order.
//
DataMemberList Slice::Exception::allClassDataMembers() const
{
  DataMemberList result;

  //
  // Check if we have a base exception. If so, recursively
  // get the class data members of the base exception(s).
  //
  if (base()) {
    result = base()->allClassDataMembers();
  }

  //
  // Append this exceptions's class data members.
  //
  DataMemberList myMembers = classDataMembers();
  result.splice(result.end(), myMembers);

  return result;
}

ExceptionPtr Slice::Exception::base() const
{
  return _base;
}

ExceptionList Slice::Exception::allBases() const
{
  ExceptionList result;
  if (_base) {
    result = _base->allBases();
    result.push_front(_base);
  }
  return result;
}

bool Slice::Exception::isBaseOf(const ExceptionPtr& other) const
{
  if (this->scoped() == other->scoped()) {
    return false;
  }
  ExceptionList bases = other->allBases();
  for (ExceptionList::const_iterator i = bases.begin(); i != bases.end(); ++i) {
    if ((*i)->scoped() == scoped()) {
      return true;
    }
  }
  return false;
}

bool Slice::Exception::isLocal() const
{
  return _local;
}

Contained::ContainedType Slice::Exception::containedType() const
{
  return ContainedTypeException;
}

bool Slice::Exception::uses(const ContainedPtr&) const
{
  // No uses() implementation here. DataMember has its own uses().
  return false;
}

bool Slice::Exception::usesClasses() const
{
  DataMemberList dml = dataMembers();
  for (DataMemberList::const_iterator i = dml.begin(); i != dml.end(); ++i) {
    if ((*i)->type()->usesClasses()) {
      return true;
    }
  }
  if (_base) {
    return _base->usesClasses();
  }
  return false;
}

string Slice::Exception::kindOf() const
{
  return "exception";
}

void Slice::Exception::visit(ParserVisitor* visitor, bool all)
{
  if (visitor->visitExceptionStart(this)) {
    Container::visit(visitor, all);
    visitor->visitExceptionEnd(this);
  }
}

Slice::Exception::Exception(const ContainerPtr& container,
                            const string& name,
                            const ExceptionPtr& base,
                            bool local) :
  SyntaxTreeBase(container->unit()), Container(container->unit()), Contained(container, name),
      _base(base), _local(local)
{
}

// ----------------------------------------------------------------------
// Struct
// ----------------------------------------------------------------------

DataMemberPtr Slice::Struct::createDataMember(const string& name, const TypePtr& type)
{
  checkPrefix(name);

  if (_unit->profile() == IceE) {
    if (!isLocal()) {
      BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
      if ((builtin && builtin->kind() == Builtin::KindObject)) {
        string msg = "Struct data member `" + name + "' cannot be a value object.";
        _unit->error(msg);
        return 0;
      }
      ClassDeclPtr classDecl = ClassDeclPtr::dynamicCast(type);
      if (classDecl != 0 && !classDecl->isLocal()) {
        string msg = "Struct data member `" + name + "' cannot be a value object.";
        _unit->error(msg);
        return 0;
      }
    }
  }

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    DataMemberPtr p = DataMemberPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (!_unit->caseSensitive() && matches.front()->name() != name) {
      string msg = "member `" + name + "' differs only in capitalization from ";
      msg += "member `" + matches.front()->name() + "'";
      _unit->error(msg);
    } else {
      string msg = "redefinition of struct member `" + name + "'";
      _unit->error(msg);
      return 0;
    }
  }

  //
  // Check whether enclosing struct has the same name.
  //
  if (name == this->name()) {
    string msg = "struct name `";
    msg += name;
    msg += "' cannot be used as member name";
    _unit->error(msg);
    return 0;
  }
  if (!_unit->caseSensitive()) {
    string newName = name;
    toLower(newName);
    string thisName = this->name();
    toLower(thisName);
    if (newName == thisName) {
      string msg = "struct member `" + name
          + "' differs only in capitalization from enclosing struct name `";
      msg += this->name() + "'";
      _unit->error(msg);
    }
  }

  //
  // Structures cannot contain themselves.
  //
  if (type.get() == this) {
    string msg = "struct `";
    msg += this->name();
    msg += "' cannot contain itself";
    _unit->error(msg);
    return 0;
  }

  //
  // If data member is local, enclosing class/interface must be local.
  //
  if (!isLocal() && type->isLocal()) {
    string msg = "non-local " + kindOf() + "`" + this->name() + "' cannot contain local member `"
        + name + "'";
    _unit->error(msg);
  }

  DataMemberPtr p = new DataMember(this, name, type);
  _contents.push_back(p);
  return p;
}

DataMemberList Slice::Struct::dataMembers() const
{
  DataMemberList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    DataMemberPtr q = DataMemberPtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

DataMemberList Slice::Struct::classDataMembers() const
{
  DataMemberList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    DataMemberPtr q = DataMemberPtr::dynamicCast(*p);
    if (q) {
      BuiltinPtr builtin = BuiltinPtr::dynamicCast(q->type());
      if ((builtin && builtin->kind() == Builtin::KindObject)
          || ClassDeclPtr::dynamicCast(q->type())) {
        result.push_back(q);
      }
    }
  }
  return result;
}

Contained::ContainedType Slice::Struct::containedType() const
{
  return ContainedTypeStruct;
}

bool Slice::Struct::uses(const ContainedPtr&) const
{
  return false;
}

bool Slice::Struct::usesClasses() const
{
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    DataMemberPtr q = DataMemberPtr::dynamicCast(*p);
    if (q) {
      TypePtr t = q->type();
      if (t->usesClasses()) {
        return true;
      }
    }
  }
  return false;
}

size_t Slice::Struct::minWireSize() const
{
  //
  // At least the sum of the minimum member sizes.
  //
  size_t sz = 0;
  DataMemberList dml = dataMembers();
  for (DataMemberList::const_iterator i = dml.begin(); i != dml.end(); ++i) {
    sz += (*i)->type()->minWireSize();
  }
  return sz;
}

bool Slice::Struct::isVariableLength() const
{
  DataMemberList dml = dataMembers();
  for (DataMemberList::const_iterator i = dml.begin(); i != dml.end(); ++i) {
    if ((*i)->type()->isVariableLength()) {
      return true;
    }
  }
  return false;
}

string Slice::Struct::kindOf() const
{
  return "struct";
}

void Slice::Struct::visit(ParserVisitor* visitor, bool all)
{
  if (visitor->visitStructStart(this)) {
    Container::visit(visitor, all);
    visitor->visitStructEnd(this);
  }
}

void Slice::Struct::recDependencies(set<ConstructedPtr>& dependencies)
{
  containerRecDependencies(dependencies);
}

Slice::Struct::Struct(const ContainerPtr& container, const string& name, bool local) :
  SyntaxTreeBase(container->unit()), Container(container->unit()), Type(container->unit()),
      Contained(container, name), Constructed(container, name, local)
{
}

// ----------------------------------------------------------------------
// List
// ----------------------------------------------------------------------

TypePtr Slice::List::type() const
{
  return _type;
}

Contained::ContainedType Slice::List::containedType() const
{
  return ContainedTypeList;
}

bool Slice::List::uses(const ContainedPtr& contained) const
{
  ContainedPtr contained2 = ContainedPtr::dynamicCast(_type);
  if (contained2 && contained2 == contained) {
    return true;
  }

  return false;
}

bool Slice::List::usesClasses() const
{
  return _type->usesClasses();
}

size_t Slice::List::minWireSize() const
{
  return 1; // An empty list.
}

bool Slice::List::isVariableLength() const
{
  return true;
}

string Slice::List::kindOf() const
{
  return "list";
}

void Slice::List::visit(ParserVisitor* visitor, bool)
{
  visitor->visitList(this);
}

void Slice::List::recDependencies(set<ConstructedPtr>& dependencies)
{
  ConstructedPtr constructed = ConstructedPtr::dynamicCast(_type);
  if (constructed && dependencies.find(constructed) != dependencies.end()) {
    dependencies.insert(constructed);
    constructed->recDependencies(dependencies);
  }
}

Slice::List::List(const ContainerPtr& container,
                          const string& name,
                          const TypePtr& type,
                          bool local) :
  SyntaxTreeBase(container->unit()), Type(container->unit()), Contained(container, name),
      Constructed(container, name, local), _type(type)
{
}

// ----------------------------------------------------------------------
// Sequence
// ----------------------------------------------------------------------

TypePtr Slice::Sequence::type() const
{
  return _type;
}

Contained::ContainedType Slice::Sequence::containedType() const
{
  return ContainedTypeSequence;
}

bool Slice::Sequence::uses(const ContainedPtr& contained) const
{
  ContainedPtr contained2 = ContainedPtr::dynamicCast(_type);
  if (contained2 && contained2 == contained) {
    return true;
  }

  return false;
}

bool Slice::Sequence::usesClasses() const
{
  return _type->usesClasses();
}

size_t Slice::Sequence::minWireSize() const
{
  return 1; // An empty sequence.
}

bool Slice::Sequence::isVariableLength() const
{
  return true;
}

string Slice::Sequence::kindOf() const
{
  return "sequence";
}

void Slice::Sequence::visit(ParserVisitor* visitor, bool)
{
  visitor->visitSequence(this);
}

void Slice::Sequence::recDependencies(set<ConstructedPtr>& dependencies)
{
  ConstructedPtr constructed = ConstructedPtr::dynamicCast(_type);
  if (constructed && dependencies.find(constructed) != dependencies.end()) {
    dependencies.insert(constructed);
    constructed->recDependencies(dependencies);
  }
}

Slice::Sequence::Sequence(const ContainerPtr& container,
                          const string& name,
                          const TypePtr& type,
                          bool local) :
  SyntaxTreeBase(container->unit()), Type(container->unit()), Contained(container, name),
      Constructed(container, name, local), _type(type)
{
}

// ----------------------------------------------------------------------
// Dictionary
// ----------------------------------------------------------------------

TypePtr Slice::Dictionary::keyType() const
{
  return _keyType;
}

TypePtr Slice::Dictionary::valueType() const
{
  return _valueType;
}

Contained::ContainedType Slice::Dictionary::containedType() const
{
  return ContainedTypeDictionary;
}

bool Slice::Dictionary::uses(const ContainedPtr& contained) const
{
  {
    ContainedPtr contained2 = ContainedPtr::dynamicCast(_keyType);
    if (contained2 && contained2 == contained) {
      return true;
    }
  }

  {
    ContainedPtr contained2 = ContainedPtr::dynamicCast(_valueType);
    if (contained2 && contained2 == contained) {
      return true;
    }
  }

  return false;
}

bool Slice::Dictionary::usesClasses() const
{
  return _valueType->usesClasses();
}

size_t Slice::Dictionary::minWireSize() const
{
  return 1; // An empty dictionary.
}

bool Slice::Dictionary::isVariableLength() const
{
  return true;
}

string Slice::Dictionary::kindOf() const
{
  return "dictionary";
}

void Slice::Dictionary::visit(ParserVisitor* visitor, bool)
{
  visitor->visitDictionary(this);
}

void Slice::Dictionary::recDependencies(set<ConstructedPtr>& dependencies)
{
  {
    ConstructedPtr constructed = ConstructedPtr::dynamicCast(_keyType);
    if (constructed && dependencies.find(constructed) != dependencies.end()) {
      dependencies.insert(constructed);
      constructed->recDependencies(dependencies);
    }
  }

  {
    ConstructedPtr constructed = ConstructedPtr::dynamicCast(_valueType);
    if (constructed && dependencies.find(constructed) != dependencies.end()) {
      dependencies.insert(constructed);
      constructed->recDependencies(dependencies);
    }
  }
}

//
// Check that the key type of a dictionary is legal. Legal types are
// integral types, string, and sequences and structs containing only
// other legal key types.
//
bool Slice::Dictionary::legalKeyType(const TypePtr& type)
{
  BuiltinPtr bp = BuiltinPtr::dynamicCast(type);
  if (bp) {
    switch (bp->kind())
    {
    case Builtin::KindByte:
    case Builtin::KindBool:
    case Builtin::KindShort:
    case Builtin::KindInt:
    case Builtin::KindLong:
    case Builtin::KindString: {
      return true;
      break;
    }

    case Builtin::KindFloat:
    case Builtin::KindDouble:
    case Builtin::KindObject:
    case Builtin::KindObjectProxy:
    case Builtin::KindLocalObject: {
      return false;
      break;
    }
    }
  }

  EnumPtr ep = EnumPtr::dynamicCast(type);
  if (ep) {
    return true;
  }

  SequencePtr seqp = SequencePtr::dynamicCast(type);
  if (seqp && legalKeyType(seqp->type())) {
    return true;
  }

  StructPtr strp = StructPtr::dynamicCast(type);
  if (strp) {
    DataMemberList dml = strp->dataMembers();
    for (DataMemberList::const_iterator mem = dml.begin(); mem != dml.end(); ++mem) {
      if (!legalKeyType((*mem)->type())) {
        return false;
      }
    }
    return true;
  }

  return false;
}

Slice::Dictionary::Dictionary(const ContainerPtr& container,
                              const string& name,
                              const TypePtr& keyType,
                              const TypePtr& valueType,
                              bool local) :
  SyntaxTreeBase(container->unit()), Type(container->unit()), Contained(container, name),
      Constructed(container, name, local), _keyType(keyType), _valueType(valueType)
{
}

// ----------------------------------------------------------------------
// Enum
// ----------------------------------------------------------------------

void Slice::Enum::destroy()
{
  _enumerators.clear();
  SyntaxTreeBase::destroy();
}

EnumeratorList Slice::Enum::getEnumerators()
{
  return _enumerators;
}

void Slice::Enum::setEnumerators(const EnumeratorList& ens)
{
  _enumerators = ens;
  for (EnumeratorList::iterator p = _enumerators.begin(); p != _enumerators.end(); ++p) {
    (*p)->_type = this;
  }
}

Contained::ContainedType Slice::Enum::containedType() const
{
  return ContainedTypeEnum;
}

bool Slice::Enum::uses(const ContainedPtr&) const
{
  return false;
}

bool Slice::Enum::usesClasses() const
{
  return false;
}

size_t Slice::Enum::minWireSize() const
{
  size_t sz = _enumerators.size();
  if (sz <= 0x7f) {
    return 1;
  }
  if (sz <= 0x7fff) {
    return 2;
  }
  return 4;
}

bool Slice::Enum::isVariableLength() const
{
  return false;
}

string Slice::Enum::kindOf() const
{
  return "enumeration";
}

void Slice::Enum::visit(ParserVisitor* visitor, bool)
{
  visitor->visitEnum(this);
}

void Slice::Enum::recDependencies(set<ConstructedPtr>&)
{
  // An Enum does not have any dependencies.
}

Slice::Enum::Enum(const ContainerPtr& container, const string& name, bool local) :
  SyntaxTreeBase(container->unit()), Type(container->unit()), Contained(container, name),
      Constructed(container, name, local)
{
}

// ----------------------------------------------------------------------
// Enumerator
// ----------------------------------------------------------------------

EnumPtr Slice::Enumerator::type() const
{
  return _type;
}

Contained::ContainedType Slice::Enumerator::containedType() const
{
  return ContainedTypeEnumerator;
}

bool Slice::Enumerator::uses(const ContainedPtr&) const
{
  return false;
}

string Slice::Enumerator::kindOf() const
{
  return "enumerator";
}

Slice::Enumerator::Enumerator(const ContainerPtr& container, const string& name) :
  SyntaxTreeBase(container->unit()), Contained(container, name)
{
}

// ----------------------------------------------------------------------
// Const
// ----------------------------------------------------------------------

TypePtr Slice::Const::type() const
{
  return _type;
}

string Slice::Const::value() const
{
  return _value;
}

Contained::ContainedType Slice::Const::containedType() const
{
  return ContainedTypeConstant;
}

bool Slice::Const::uses(const ContainedPtr& contained) const
{
  ContainedPtr contained2 = ContainedPtr::dynamicCast(_type);
  return (contained2 && contained2 == contained);
}

string Slice::Const::kindOf() const
{
  return "constant";
}

void Slice::Const::visit(ParserVisitor* visitor, bool)
{
  visitor->visitConst(this);
}

bool Slice::Const::isLegalType(const string& name, const TypePtr& constType, const UnitPtr& unit)
{
  if (constType == 0) {
    return false;
  }

  BuiltinPtr ct = BuiltinPtr::dynamicCast(constType);
  if (ct) {
    switch (ct->kind())
    {
    case Builtin::KindBool:
    case Builtin::KindByte:
    case Builtin::KindShort:
    case Builtin::KindInt:
    case Builtin::KindLong:
    case Builtin::KindFloat:
    case Builtin::KindDouble:
    case Builtin::KindString: {
      return true;
      break;
    }
    default: {
      string msg = "constant `" + name + "' has illegal type: `" + ct->kindAsString() + "'";
      unit->error(msg);
      return false;
      break;
    }
    }
  }

  EnumPtr ep = EnumPtr::dynamicCast(constType);
  if (!ep) {
    string msg = "constant `" + name + "' has illegal type";
    unit->error(msg);
    return false;
  }
  return true;
}

bool Slice::Const::typesAreCompatible(const string& name,
                                      const TypePtr& constType,
                                      const SyntaxTreeBasePtr& literalType,
                                      const string& value,
                                      const UnitPtr& unit)
{
  BuiltinPtr ct = BuiltinPtr::dynamicCast(constType);

#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
  // Strange Sun C++ 5.3 bug.
  const IceUtil::HandleBase<SyntaxTreeBase>& hb = literalType;
  BuiltinPtr lt = BuiltinPtr::dynamicCast(hb);
#else
  BuiltinPtr lt = BuiltinPtr::dynamicCast(literalType);
#endif

  if (ct && lt) {
    switch (ct->kind())
    {
    case Builtin::KindBool: {
      if (lt->kind() == Builtin::KindBool) {
        return true;
      }
      break;
    }
    case Builtin::KindByte:
    case Builtin::KindShort:
    case Builtin::KindInt:
    case Builtin::KindLong: {
      if (lt->kind() == Builtin::KindLong) {
        return true;
      }
      break;
    }
    case Builtin::KindFloat:
    case Builtin::KindDouble: {
      if (lt->kind() == Builtin::KindDouble) {
        return true;
      }
      break;
    }
    case Builtin::KindString: {
      if (lt->kind() == Builtin::KindString) {
        return true;
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

    string msg = "initializer of type `" + lt->kindAsString();
    msg += "' is incompatible with the type `" + ct->kindAsString() + "' of constant `" + name
        + "'";
    unit->error(msg);
    return false;
  }

  if (ct && !lt) {
    string msg = "type of initializer is incompatible with the type `" + ct->kindAsString();
    msg += "' of constant `" + name + "'";
    unit->error(msg);
    return false;
  }

  EnumPtr enumP = EnumPtr::dynamicCast(constType);
  assert(enumP);
  EnumeratorPtr enumeratorP = EnumeratorPtr::dynamicCast(literalType);
  if (!enumeratorP) {
    string msg = "type of initializer is incompatible with the type of constant `" + name + "'";
    unit->error(msg);
    return false;
  }
  EnumeratorList elist = enumP->getEnumerators();
  if (find(elist.begin(), elist.end(), enumeratorP) == elist.end()) {
    string msg = "enumerator `" + value + "' is not defined in enumeration `" + enumP->scoped()
        + "'";
    unit->error(msg);
    return false;
  }
  return true;
}

bool Slice::Const::isInRange(const string& name,
                             const TypePtr& constType,
                             const string& value,
                             const UnitPtr& unit)
{
  BuiltinPtr ct = BuiltinPtr::dynamicCast(constType);
  if (!ct) {
    return true; // Enums are checked elsewhere.
  }

  switch (ct->kind())
  {
  case Builtin::KindByte: {
    IceUtil::Int64 l = IceUtil::strToInt64(value.c_str(), 0, 0);
    if (l < ByteMin || l > ByteMax) {
      string msg = "initializer `" + value + "' for constant `" + name
          + "' out of range for type byte";
      unit->error(msg);
      return false;
    }
    break;
  }
  case Builtin::KindShort: {
    IceUtil::Int64 l = IceUtil::strToInt64(value.c_str(), 0, 0);
    if (l < Int16Min || l > Int16Max) {
      string msg = "initializer `" + value + "' for constant `" + name
          + "' out of range for type short";
      unit->error(msg);
      return false;
    }
    break;
  }
  case Builtin::KindInt: {
    IceUtil::Int64 l = IceUtil::strToInt64(value.c_str(), 0, 0);
    if (l < Int32Min || l > Int32Max) {
      string msg = "initializer `" + value + "' for constant `" + name
          + "' out of range for type int";
      unit->error(msg);
      return false;
    }
    break;
  }

  default: {
    break;
  }
  }
  return true; // Everything else is either in range or doesn't need checking.
}

Slice::Const::Const(const ContainerPtr& container,
                    const string& name,
                    const TypePtr& type,
                    const string& value) :
  SyntaxTreeBase(container->unit()), Contained(container, name), _type(type), _value(value)
{
}

// ----------------------------------------------------------------------
// Operation
// ----------------------------------------------------------------------

TypePtr Slice::Operation::returnType() const
{
  return _returnType;
}

Operation::Mode Slice::Operation::mode() const
{
  return _mode;
}

ParamDeclPtr Slice::Operation::createParamDecl(const string& name,
                                               const TypePtr& type,
                                               bool isOutParam)
{
  checkPrefix(name);

  if (_unit->profile() == IceE) {
    ClassDefPtr cl = ClassDefPtr::dynamicCast(this->container());
    assert(cl);
    if (!cl->isLocal()) {
      BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
      if ((builtin && builtin->kind() == Builtin::KindObject)) {
        string msg = "Object `" + name + "' cannot be passed by value.";
        _unit->error(msg);
        return 0;
      }
      ClassDeclPtr classDecl = ClassDeclPtr::dynamicCast(type);
      if (classDecl != 0 && !classDecl->isLocal()) {
        string msg = "Object `" + name + "' cannot be passed by value.";
        _unit->error(msg);
        return 0;
      }
    }
  }

  ContainedList matches = _unit->findContents(thisScope() + name);
  if (!matches.empty()) {
    ParamDeclPtr p = ParamDeclPtr::dynamicCast(matches.front());
    if (p) {
      if (_unit->ignRedefs()) {
        p->updateIncludeLevel();
        return p;
      }
    }
    if (!_unit->caseSensitive() && matches.front()->name() != name) {
      string msg = "parameter `" + name + "' differs only in capitalization from ";
      msg += "parameter `" + matches.front()->name() + "'";
      _unit->error(msg);
    } else {
      string msg = "redefinition of parameter `" + name + "'";
      _unit->error(msg);
      return 0;
    }
  }

  //
  // Check whether enclosing operation has the same name.
  //
  if (name == this->name()) {
    string msg = "operation name `";
    msg += name;
    msg += "' cannot be used as parameter name";
    _unit->error(msg);
    return 0;
  }
  if (!_unit->caseSensitive()) {
    string newName = name;
    toLower(newName);
    string thisName = this->name();
    toLower(thisName);
    if (newName == thisName) {
      string msg = "parameter `" + name + "' differs only in capitalization from operation name `";
      msg += this->name() + "'";
      _unit->error(msg);
    }
  }

  //
  // Check that in parameters don't follow out parameters.
  //
  if (!_contents.empty()) {
    ParamDeclPtr p = ParamDeclPtr::dynamicCast(_contents.back());
    assert(p);
    if (p->isOutParam() && !isOutParam) {
      _unit->error("`" + name + "': in parameters cannot follow out parameters");
    }
  }

  //
  // Non-local class/interface cannot have operation with local parameters.
  //
  ClassDefPtr cl = ClassDefPtr::dynamicCast(this->container());
  assert(cl);
  if (type->isLocal() && !cl->isLocal()) {
    /* by qinghua
    string msg = "non-local " + cl->kindOf() + " `" + cl->name()
        + "' cannot have local parameter `";
    msg += name + "' in operation `" + this->name() + "'";
    _unit->error(msg);
    */
  }

  ParamDeclPtr p = new ParamDecl(this, name, type, isOutParam);
  _contents.push_back(p);
  return p;
}

ParamDeclList Slice::Operation::parameters() const
{
  ParamDeclList result;
  for (ContainedList::const_iterator p = _contents.begin(); p != _contents.end(); ++p) {
    ParamDeclPtr q = ParamDeclPtr::dynamicCast(*p);
    if (q) {
      result.push_back(q);
    }
  }
  return result;
}

ExceptionList Slice::Operation::throws() const
{
  return _throws;
}

void Slice::Operation::setExceptionList(const ExceptionList& el)
{
  _throws = el;

  //
  // Check that no exception occurs more than once in the throws clause.
  //
  ExceptionList uniqueExceptions = el;
  uniqueExceptions.sort();
  uniqueExceptions.unique();
  if (uniqueExceptions.size() != el.size()) {
    //
    // At least one exception appears twice.
    //
    ExceptionList tmp = el;
    tmp.sort();
    ExceptionList duplicates;
    set_difference(tmp.begin(), tmp.end(), uniqueExceptions.begin(), uniqueExceptions.end(), back_inserter(duplicates));
    string msg = "operation `" + name() + "' has a throws clause with ";
    if (duplicates.size() == 1) {
      msg += "a ";
    }
    msg += "duplicate exception";
    if (duplicates.size() > 1) {
      msg += "s";
    }
    ExceptionList::const_iterator i = duplicates.begin();
    msg += ": `" + (*i)->name() + "'";
    for (i = ++i; i != duplicates.end(); ++i) {
      msg += ", `" + (*i)->name() + "'";
    }
    _unit->error(msg);
  }

  //
  // If the interface is non-local, no local exception can be thrown.
  //
  ClassDefPtr cl = ClassDefPtr::dynamicCast(container());
  assert(cl);
  if (!cl->isLocal()) {
    for (ExceptionList::const_iterator ep = el.begin(); ep != el.end(); ++ep) {
      if ((*ep)->isLocal()) {
        /* by qinghua
        string msg = "non-local " + cl->kindOf() + " `" + cl->name() + "' cannot have operation `";
        msg += name() + "' throwing local exception `" + (*ep)->name() + "'";
        _unit->error(msg);
        */
      }
    }
  }
}

Contained::ContainedType Slice::Operation::containedType() const
{
  return ContainedTypeOperation;
}

bool Slice::Operation::uses(const ContainedPtr& contained) const
{
  {
    ContainedPtr contained2 = ContainedPtr::dynamicCast(_returnType);
    if (contained2 && contained2 == contained) {
      return true;
    }
  }

  ExceptionList::const_iterator q;

  for (q = _throws.begin(); q != _throws.end(); ++q) {
    ContainedPtr contained2 = ContainedPtr::dynamicCast(*q);
    if (contained2 && contained2 == contained) {
      return true;
    }
  }

  return false;
}

bool Slice::Operation::sendsClasses() const
{
  ParamDeclList pdl = parameters();
  for (ParamDeclList::const_iterator i = pdl.begin(); i != pdl.end(); ++i) {
    if (!(*i)->isOutParam() && (*i)->type()->usesClasses()) {
      return true;
    }
  }
  return false;
}

bool Slice::Operation::returnsClasses() const
{
  TypePtr t = returnType();
  if (t && t->usesClasses()) {
    return true;
  }
  ParamDeclList pdl = parameters();
  for (ParamDeclList::const_iterator i = pdl.begin(); i != pdl.end(); ++i) {
    if ((*i)->isOutParam() && (*i)->type()->usesClasses()) {
      return true;
    }
  }
  return false;
}

bool Slice::Operation::returnsData() const
{
  TypePtr t = returnType();
  if (t) {
    return true;
  }
  ParamDeclList pdl = parameters();
  for (ParamDeclList::const_iterator i = pdl.begin(); i != pdl.end(); ++i) {
    if ((*i)->isOutParam()) {
      return true;
    }
  }
  if (!throws().empty()) {
    return true;
  }
  return false;
}

string Slice::Operation::kindOf() const
{
  return "operation";
}

void Slice::Operation::visit(ParserVisitor* visitor, bool)
{
  visitor->visitOperation(this);
}

Slice::Operation::Operation(const ContainerPtr& container,
                            const string& name,
                            const TypePtr& returnType,
                            Mode mode) :
  SyntaxTreeBase(container->unit()), Contained(container, name), Container(container->unit()),
      _returnType(returnType), _mode(mode)
{
  if (_unit->profile() == IceE) {
    ClassDefPtr cl = ClassDefPtr::dynamicCast(this->container());
    assert(cl);
    if (!cl->isLocal()) {
      BuiltinPtr builtin = BuiltinPtr::dynamicCast(returnType);
      if ((builtin && builtin->kind() == Builtin::KindObject)) {
        string msg = "Method `" + name + "' cannot return an object by value.";
        _unit->error(msg);
      }
      ClassDeclPtr classDecl = ClassDeclPtr::dynamicCast(returnType);
      if (classDecl != 0 && !classDecl->isLocal()) {
        string msg = "Method `" + name + "' cannot return an object by value.";
        _unit->error(msg);
      }
    }
  }
}

// ----------------------------------------------------------------------
// ParamDecl
// ----------------------------------------------------------------------

TypePtr Slice::ParamDecl::type() const
{
  return _type;
}

bool Slice::ParamDecl::isOutParam() const
{
  return _isOutParam;
}

Contained::ContainedType Slice::ParamDecl::containedType() const
{
  return ContainedTypeDataMember;
}

bool Slice::ParamDecl::uses(const ContainedPtr& contained) const
{
  ContainedPtr contained2 = ContainedPtr::dynamicCast(_type);
  if (contained2 && contained2 == contained) {
    return true;
  }

  return false;
}

string Slice::ParamDecl::kindOf() const
{
  return "parameter declaration";
}

void Slice::ParamDecl::visit(ParserVisitor* visitor, bool)
{
  visitor->visitParamDecl(this);
}

Slice::ParamDecl::ParamDecl(const ContainerPtr& container,
                            const string& name,
                            const TypePtr& type,
                            bool isOutParam) :
  SyntaxTreeBase(container->unit()), Contained(container, name), _type(type),
      _isOutParam(isOutParam)
{
}

// ----------------------------------------------------------------------
// DataMember
// ----------------------------------------------------------------------

TypePtr Slice::DataMember::type() const
{
  return _type;
}

Contained::ContainedType Slice::DataMember::containedType() const
{
  return ContainedTypeDataMember;
}

bool Slice::DataMember::uses(const ContainedPtr& contained) const
{
  ContainedPtr contained2 = ContainedPtr::dynamicCast(_type);
  if (contained2 && contained2 == contained) {
    return true;
  }

  return false;
}

string Slice::DataMember::kindOf() const
{
  return "data member";
}

void Slice::DataMember::visit(ParserVisitor* visitor, bool)
{
  visitor->visitDataMember(this);
}

Slice::DataMember::DataMember(const ContainerPtr& container,
                              const string& name,
                              const TypePtr& type) :
  SyntaxTreeBase(container->unit()), Contained(container, name), _type(type)
{
}

// ----------------------------------------------------------------------
// Unit
// ----------------------------------------------------------------------

UnitPtr Slice::Unit::createUnit(bool ignRedefs, bool all, bool allowIcePrefix, bool caseSensitive)
{
  return new Unit(ignRedefs, all, allowIcePrefix, caseSensitive);
}

bool Slice::Unit::ignRedefs() const
{
  return _ignRedefs;
}

bool Slice::Unit::allowIcePrefix() const
{
  return _allowIcePrefix;
}

bool Slice::Unit::caseSensitive() const
{
  return _caseSensitive;
}
void Slice::Unit::setComment(const string& comment)
{
  _currentComment = "";

  string::size_type end = 0;
  while (true) {
    string::size_type begin = comment.find_first_not_of(" \t\r\n*", end);
    if (begin == string::npos) {
      break;
    }

    end = comment.find('\n', begin);
    if (end != string::npos) {
      if (end + 1 > begin) {
        _currentComment += comment.substr(begin, end + 1 - begin);
      }
    } else {
      end = comment.find_last_not_of(" \t\r\n*");
      if (end != string::npos) {
        if (end + 1 > begin) {
          _currentComment += comment.substr(begin, end + 1 - begin);
        }
      }
      break;
    }
  }
}

string Slice::Unit::currentComment()
{
  string comment = "";
  comment.swap(_currentComment);
  return comment;
}

string Slice::Unit::currentFile() const
{
  DefinitionContextPtr dc = currentDefinitionContext();
  if (dc) {
    return dc->filename();
  } else {
    return string();
  }
}

int Slice::Unit::currentLine() const
{
  return _currentLine;
}

void Slice::Unit::nextLine()
{
  _currentLine++;
}

void Slice::Unit::scanPosition(const char* s)
{
  assert(*s == '#');

  string line(s + 1); // Skip leading #
  eraseWhiteSpace(line);
  if (line.find("line", 0) == 0) // Erase optional "line"
  {
    line.erase(0, 4);
    eraseWhiteSpace(line);
  }

  string::size_type idx;

  _currentLine = atoi(line.c_str()) - 1; // Read line number

  idx = line.find_first_of(" \t\r"); // Erase line number
  if (idx != string::npos) {
    line.erase(0, idx);
  }
  eraseWhiteSpace(line);

  //
  // If the string ends in <whitespace>1 or <whitespace>2, it is a push or pop directive.
  //
  enum LineType
  {
    File,
    Push,
    Pop
  };

  LineType type = File;

  idx = line.find_last_of(" \t\r");
  if (idx != string::npos) {
    ++idx;
    if (line.substr(idx) == "1") {
      type = Push;
      line.erase(idx);
      eraseWhiteSpace(line);

    } else if (line.substr(idx) == "2") {
      type = Pop;
      line.erase(idx);
      eraseWhiteSpace(line);
    }
  }

  string currentFile;
  if (!line.empty()) {
    if (line[0] == '"') {
      idx = line.rfind('"');
      if (idx != string::npos) {
        currentFile = line.substr(1, idx - 1);
      }
    } else {
      currentFile = line;
    }
  }

  switch (type)
  {
  case Push: {
    if (++_currentIncludeLevel == 1) {
      if (find(_includeFiles.begin(), _includeFiles.end(), currentFile) == _includeFiles.end()) {
        _includeFiles.push_back(currentFile);
      }
    }
    pushDefinitionContext();
    _currentComment = "";
    break;
  }
  case Pop: {
    --_currentIncludeLevel;
    popDefinitionContext();
    _currentComment = "";
    break;
  }
  default: {
    break; // Do nothing
  }
  }
  if (!currentFile.empty()) {
    DefinitionContextPtr dc = currentDefinitionContext();
    assert(dc);
    dc->setFilename(currentFile);
  }
}

int Slice::Unit::currentIncludeLevel() const
{
  if (_all) {
    return 0;
  } else {
    return _currentIncludeLevel;
  }
}

void Slice::Unit::setGlobalMetaData(const StringList& metaData)
{
  DefinitionContextPtr dc = currentDefinitionContext();
  assert(dc);
  if (dc->hasMetaData()) {
    error("global metadata can only be set once per file");
  } else if (dc->seenDefinition()) {
    error("global metadata must appear before any definitions");
  } else {
    dc->setMetaData(metaData);
  }
}

void Slice::Unit::setSeenDefinition()
{
  DefinitionContextPtr dc = currentDefinitionContext();
  assert(dc);
  dc->setSeenDefinition();
}

void Slice::Unit::error(const char* s)
{
  string file = currentFile();
  if (!file.empty()) {
    cout << file << ':' << _currentLine << ": ";
  }
  cout << s << endl;
  _errors++;
}

void Slice::Unit::error(const string& s)
{
  error(s.c_str());
}

void Slice::Unit::warning(const char* s) const
{
  string file = currentFile();
  if (!file.empty()) {
    cout << file << ':' << _currentLine << ": ";
  }
  cout << "warning: " << s << endl;
}

void Slice::Unit::warning(const string& s) const
{
  warning(s.c_str());
}

ContainerPtr Slice::Unit::currentContainer() const
{
  assert(!_containerStack.empty());
  return _containerStack.top();
}

void Slice::Unit::pushContainer(const ContainerPtr& cont)
{
  _containerStack.push(cont);
}

void Slice::Unit::popContainer()
{
  assert(!_containerStack.empty());
  _containerStack.pop();
}

DefinitionContextPtr Slice::Unit::currentDefinitionContext() const
{
  DefinitionContextPtr dc;
  if (!_definitionContextStack.empty()) {
    dc = _definitionContextStack.top();
  }
  return dc;
}

void Slice::Unit::pushDefinitionContext()
{
  _definitionContextStack.push(new DefinitionContext(_currentIncludeLevel));
}

void Slice::Unit::popDefinitionContext()
{
  assert(!_definitionContextStack.empty());
  _definitionContextStack.pop();
}

void Slice::Unit::addContent(const ContainedPtr& contained)
{
  string scoped = contained->scoped();
  if (!caseSensitive()) {
    toLower(scoped);
  }
  _contentMap[scoped].push_back(contained);
}

void Slice::Unit::removeContent(const ContainedPtr& contained)
{
  string scoped = contained->scoped();
  if (!caseSensitive()) {
    toLower(scoped);
  }
  map<string, ContainedList>::iterator p = _contentMap.find(scoped);
  assert(p != _contentMap.end());
  ContainedList::iterator q;
  for (q = p->second.begin(); q != p->second.end(); ++q) {
    if (q->get() == contained.get()) {
      p->second.erase(q);
      return;
    }
  }
  assert(false);
}

ContainedList Slice::Unit::findContents(const string& scoped) const
{
  assert(!scoped.empty());
  assert(scoped[0] == ':');

  string name = scoped;
  if (!_unit->caseSensitive()) {
    toLower(name);
  }

  map<string, ContainedList>::const_iterator p = _contentMap.find(name);
  if (p != _contentMap.end()) {
    return p->second;
  } else {
    return ContainedList();
  }
}

ClassList Slice::Unit::findDerivedClasses(const ClassDefPtr& cl) const
{
  ClassList derived;
  for (map<string, ContainedList>::const_iterator p = _contentMap.begin(); p != _contentMap.end(); ++p) {
    for (ContainedList::const_iterator q = p->second.begin(); q != p->second.end(); ++q) {
      ClassDefPtr r = ClassDefPtr::dynamicCast(*q);
      if (r) {
        ClassList bases = r->bases();
        if (find(bases.begin(), bases.end(), cl) != bases.end()) {
          derived.push_back(r);
        }
      }
    }
  }
  derived.sort();
  derived.unique();
  return derived;
}

ExceptionList Slice::Unit::findDerivedExceptions(const ExceptionPtr& ex) const
{
  ExceptionList derived;
  for (map<string, ContainedList>::const_iterator p = _contentMap.begin(); p != _contentMap.end(); ++p) {
    for (ContainedList::const_iterator q = p->second.begin(); q != p->second.end(); ++q) {
      ExceptionPtr r = ExceptionPtr::dynamicCast(*q);
      if (r) {
        ExceptionPtr base = r->base();
        if (base && base == ex) {
          derived.push_back(r);
        }
      }
    }
  }
  derived.sort();
  derived.unique();
  return derived;
}

ContainedList Slice::Unit::findUsedBy(const ContainedPtr& contained) const
{
  ContainedList usedBy;
  for (map<string, ContainedList>::const_iterator p = _contentMap.begin(); p != _contentMap.end(); ++p) {
    for (ContainedList::const_iterator q = p->second.begin(); q != p->second.end(); ++q) {
      if ((*q)->uses(contained)) {
        usedBy.push_back(*q);
      }
    }
  }
  usedBy.sort();
  usedBy.unique();
  return usedBy;
}

bool Slice::Unit::usesProxies() const
{
  for (map<string, ContainedList>::const_iterator p = _contentMap.begin(); p != _contentMap.end(); ++p) {
    for (ContainedList::const_iterator q = p->second.begin(); q != p->second.end(); ++q) {
      ClassDeclPtr decl = ClassDeclPtr::dynamicCast(*q);
      if (decl && !decl->isLocal()) {
        return true;
      }
    }
  }

  if (_builtins.find(Builtin::KindObjectProxy) != _builtins.end()) {
    return true;
  }

  return false;
}

bool Slice::Unit::usesNonLocals() const
{
  for (map<string, ContainedList>::const_iterator p = _contentMap.begin(); p != _contentMap.end(); ++p) {
    for (ContainedList::const_iterator q = p->second.begin(); q != p->second.end(); ++q) {
      ConstructedPtr constr = ConstructedPtr::dynamicCast(*q);
      if (constr && !constr->isLocal()) {
        return true;
      }

      ExceptionPtr exc = ExceptionPtr::dynamicCast(*q);
      if (exc && !exc->isLocal()) {
        return true;
      }
    }
  }

  if (_builtins.find(Builtin::KindObject) != _builtins.end()) {
    return true;
  }

  if (_builtins.find(Builtin::KindObjectProxy) != _builtins.end()) {
    return true;
  }

  return false;
}

bool Slice::Unit::usesConsts() const
{
  for (map<string, ContainedList>::const_iterator p = _contentMap.begin(); p != _contentMap.end(); ++p) {
    for (ContainedList::const_iterator q = p->second.begin(); q != p->second.end(); ++q) {
      ConstPtr cd = ConstPtr::dynamicCast(*q);
      if (cd) {
        return true;
      }
    }
  }

  return false;
}

FeatureProfile Slice::Unit::profile() const
{
  return _featureProfile;
}

void 
Slice::Unit::addIncludeFile(const std::string& includeFile)
{
  this->_includeFiles.push_back(includeFile);
}

StringList Slice::Unit::includeFiles() const
{
  return _includeFiles;
}

int Slice::Unit::parse(FILE* file, bool debug, Slice::FeatureProfile profile)
{
  slice_debug = debug ? 1 : 0;

  assert(!Slice::unit);
  Slice::unit = this;

  _currentComment = "";
  _currentLine = 1;
  _currentIncludeLevel = 0;
  _featureProfile = profile;
  pushContainer(this);
  pushDefinitionContext();

  slice_in = file;
  int status = slice_parse();
  if (_errors) {
    status = EXIT_FAILURE;
  }

  if (status == EXIT_FAILURE) {
    while (!_containerStack.empty()) {
      popContainer();
    }
    while (!_definitionContextStack.empty()) {
      popDefinitionContext();
    }
  } else {
    assert(_containerStack.size() == 1);
    popContainer();
    assert(_definitionContextStack.size() == 1);
    popDefinitionContext();
  }

  Slice::unit = 0;
  return status;
}

void Slice::Unit::destroy()
{
  _contentMap.clear();
  _builtins.clear();
  Container::destroy();
}

void Slice::Unit::visit(ParserVisitor* visitor, bool all)
{
  if (visitor->visitUnitStart(this)) {
    Container::visit(visitor, all);
    visitor->visitUnitEnd(this);
  }
}

BuiltinPtr Slice::Unit::builtin(Builtin::Kind kind)
{
  map<Builtin::Kind, BuiltinPtr>::const_iterator p = _builtins.find(kind);
  if (p != _builtins.end()) {
    return p->second;
  }
  BuiltinPtr builtin = new Builtin(this, kind);
  _builtins.insert(make_pair(kind, builtin));
  return builtin;
}

Slice::Unit::Unit(bool ignRedefs, bool all, bool allowIcePrefix, bool caseSensitive) :
  SyntaxTreeBase(0), Container(0), _ignRedefs(ignRedefs), _all(all),
      _allowIcePrefix(allowIcePrefix), _caseSensitive(caseSensitive), _errors(0)
{
  _unit = this;
}

void Slice::Unit::eraseWhiteSpace(string& s)
{
  string::size_type idx = s.find_first_not_of(" \t\r");
  if (idx != string::npos) {
    s.erase(0, idx);
  }
  idx = s.find_last_not_of(" \t\r");
  if (idx != string::npos) {
    s.erase(++idx);
  }
}

// ----------------------------------------------------------------------
// CICompare
// ----------------------------------------------------------------------

bool Slice::CICompare::operator()(const string& s1, const string& s2) const
{
  string::const_iterator p1 = s1.begin();
  string::const_iterator p2 = s2.begin();
  while(p1 != s1.end() && p2 != s2.end() && ::tolower(*p1) == ::tolower(*p2))
  {
    ++p1;
    ++p2;
  }
  if(p1 == s1.end() && p2 == s2.end())
  {
    return false;
  }
  else if(p1 == s1.end())
  {
    return true;
  }
  else if(p2 == s2.end())
  {
    return false;
  }
  else
  {
    return ::tolower(*p1) < ::tolower(*p2);
  }
}

#if defined(__SUNPRO_CC)
bool
Slice::cICompare(const std::string& s1, const std::string& s2)
{
  CICompare c;
  return c(s1, s2);
}
#endif

// ----------------------------------------------------------------------
// DerivedToBaseCompare
// ----------------------------------------------------------------------

bool Slice::DerivedToBaseCompare::operator()(const ExceptionPtr& e1, const ExceptionPtr& e2) const
{
  return e2->isBaseOf(e1);
}

#if defined(__SUNPRO_CC)
bool
Slice::derivedToBaseCompare(const ExceptionPtr& e1, const ExceptionPtr& e2)
{
  return e2->isBaseOf(e1);
}
#endif
