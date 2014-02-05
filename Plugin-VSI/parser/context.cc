/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

//== INCLUDES =================================================================
#include <QBuffer>
#include <QStringList>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QRegExp>

#include <iostream>

#include "context.hh"
#include "input.hh"
#include "output.hh"
#include "type.hh"
#include "function.hh"

#include "types/number.hh"
#include "types/string.hh"
#include "types/bool.hh"
#include "types/filename.hh"
#include "types/selection.hh"
#include "types/vec3d.hh"
#include "types/vec4d.hh"
#include "types/matrix4x4.hh"
#include "types/objectId/objectId.hh"
#include "types/any.hh"

#define DATA_NAME "DataFlow"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Context - IMPLEMENTATION
//
//=============================================================================

/// Constructor
Context::Context (QScriptEngine *_engine) :
  scriptEngine_ (_engine)
{
  // add start element
  Element *e = new Element (this, "start");
  elements_.append (e);

  e->category_ = "Undefined";

  e->shortDesc_ = "Start";
  e->longDesc_ = "Start";

  e->dataOut_ = new Output (e);
  e->dataOut_->name_ = "data";
  e->dataOut_->type_ = "data";

  e->dataOut_->shortDesc_ = DATA_NAME;
  e->dataOut_->longDesc_ = DATA_NAME;

  e->flags_ = ELEMENT_FLAG_NO_DELETE | ELEMENT_FLAG_SKIP_TOOLBOX;

  // add end element
  e = new Element (this, "end");
  elements_.append (e);

  e->category_ = "Undefined";

  e->shortDesc_ = "End";
  e->longDesc_ = "End";

  e->dataIn_ = new Input (e);
  e->dataIn_->name_ = "data";
  e->dataIn_->type_ = "data";

  e->dataIn_->shortDesc_ = DATA_NAME;
  e->dataIn_->longDesc_ = DATA_NAME;

  e->flags_ = ELEMENT_FLAG_NO_DELETE | ELEMENT_FLAG_SKIP_TOOLBOX;


  // Register default types
  registerType (new TypeNumber ());
  registerType (new TypeBool ());
  registerType (new TypeString ());
  registerType (new TypeFilename ());
  registerType (new TypeSelection ());
  registerType (new TypeVec3D ());
  registerType (new TypeVec4D ());
  registerType (new TypeMatrix4x4 ());
  registerType (new TypeObjectId ());
  registerType (new TypeAny ());
}

//------------------------------------------------------------------------------

/// Destructor
Context::~Context ()
{
  foreach (Element *e, elements_)
    delete e;
  foreach (Type *t, types_)
    delete t;
}

//------------------------------------------------------------------------------

/// Returns all available elements for a given category
QVector <Element *> Context::elements (QString _category)
{
  QVector <Element *> rv;
  foreach (Element *e, elements_)
    if (e->category () == _category)
      rv.append (e);
  return rv;
}

//------------------------------------------------------------------------------

/// Returns the element with a given name
Element * Context::element (QString _name)
{
  foreach (Element *e, elements_)
    if (e->name () == _name)
      return e;
  return NULL;
}

//------------------------------------------------------------------------------

/// List of categories
QStringList Context::categories ()
{
  QStringList sl;

  foreach (Element *e, elements_)
    if (!sl.contains (e->category ()) && !(e->flags () & ELEMENT_FLAG_SKIP_TOOLBOX))
      sl << e->category ();

  return sl;
}

//------------------------------------------------------------------------------

/// Parse xml content
void Context::parse (QByteArray _xml)
{
  QBuffer device (&_xml);
  device.open(QIODevice::ReadOnly);

  QXmlQuery query;
  query.setFocus (&device);

  // for each OpenFlipper/element
  query.setQuery ("OpenFlipper//element");

  QXmlResultItems elements;

  if (query.isValid ())
  {
    query.evaluateTo (&elements);
    QXmlItem item (elements.next ());
    while (!item.isNull ())
    {
      QXmlQuery q (query);
      q.setFocus (item);
      parseElement (q);
      item = elements.next ();
    }

  }

}

//------------------------------------------------------------------------------

// parse element from xml
void Context::parseElement (QXmlQuery &_xml)
{
  if (getXmlString (_xml, "string(@name)").isEmpty ())
    return;

  Element *e = new Element (this, getXmlString (_xml, "string(@name)"));

  elements_.append (e);

  e->category_ = getXmlString (_xml, "category/string(text())", "Undefined");

  e->shortDesc_ = getXmlString (_xml, "short/string(text())", e->name ());
  e->longDesc_ = getXmlString (_xml, "long/string(text())", e->shortDesc_);

  // scene graph in/output for scenegraph elements
  if (strToBool (getXmlString (_xml, "dataflow/string(text())")))
  {

    e->dataIn_ = new Input (e);
    e->dataIn_->name_ = "data";
    e->dataIn_->type_ = QVariant::Invalid;

    e->dataIn_->shortDesc_ = DATA_NAME;
    e->dataIn_->longDesc_ = DATA_NAME;
    e->dataIn_->setState (Input::NoRuntimeUserInput | Input::NoUserInput);

    e->dataOut_ = new Output (e);
    e->dataOut_->name_ = "data";
    e->dataOut_->type_ = QVariant::Invalid;

    e->dataOut_->shortDesc_ = DATA_NAME;
    e->dataOut_->longDesc_ = DATA_NAME;
  }

  // parse all inputs
  _xml.setQuery ("inputs/input");

  QXmlResultItems inouts;

  if (_xml.isValid ())
  {
    _xml.evaluateTo (&inouts);
    QXmlItem item (inouts.next ());
    while (!item.isNull ())
    {
      QXmlQuery q (_xml);
      q.setFocus (item);
      Input *i = parseInput (q, e);
      if (i)
      {
        e->inputs_.append (i);
      }       
      item = inouts.next ();
    }

  }

  // parse all outputs
  _xml.setQuery ("outputs/output");

  if (_xml.isValid ())
  {
    _xml.evaluateTo (&inouts);
    QXmlItem item (inouts.next ());
    while (!item.isNull ())
    {
      QXmlQuery q (_xml);
      q.setFocus (item);
      Output *o = parseOutput (q, e);
      if (o)
      {
        o->element_ = e;
        e->outputs_.append (o);
      }
      item = inouts.next ();
    }

  }

  // parse all functions
  _xml.setQuery ("functions/function");

  if (_xml.isValid ())
  {
    _xml.evaluateTo (&inouts);
    QXmlItem item (inouts.next ());
    while (!item.isNull ())
    {
      QXmlQuery q (_xml);
      q.setFocus (item);
      Function *f = parseFunction (q, e);
      if (f)
      {
        e->functions_.append (f);
      }
      item = inouts.next ();
    }

  }

  // get code & precode segment
  e->precode_ = getXmlString (_xml, "precode/string(text())", "");
  e->code_ = getXmlString (_xml, "code/string(text())", "");

  // remove spaces at line begin
  e->precode_ = e->precode_.replace (QRegExp ("\\n\\s*"),"\n");
  e->code_ = e->code_.replace (QRegExp ("\\n\\s*"),"\n");

}

//------------------------------------------------------------------------------

// parse element input from xml
Input* Context::parseInput (QXmlQuery &_xml, Element *_e)
{
  Input *i = new Input (_e);

  // common part
  if (!parseInOutBase (_xml, i))
  {
    delete i;
    return NULL;
  }

  // input states
  QString stateStr = getXmlString (_xml, "string(@external)");
  unsigned int state = 0;
  
  if (!stateStr.isEmpty () && !strToBool (stateStr))
    state |= Input::NoExternalInput;

  stateStr = getXmlString (_xml, "string(@user)");

  if (!stateStr.isEmpty () && !strToBool (stateStr))
    state |= Input::NoUserInput;

  stateStr = getXmlString (_xml, "string(@runtime)");

  if (!stateStr.isEmpty () && !strToBool (stateStr))
    state |= Input::NoRuntimeUserInput;

  stateStr = getXmlString (_xml, "string(@optional)");

  if (!stateStr.isEmpty () && strToBool (stateStr))
    state |= Input::Optional;

  i->state_ = state;

  return i;
}

//------------------------------------------------------------------------------

// parse element output from xml
Output* Context::parseOutput (QXmlQuery &_xml, Element *_e)
{
  Output *o = new Output (_e);

  // common part
  if (!parseInOutBase (_xml, o))
  {
    delete o;
    return NULL;
  }

  return o;
}

//------------------------------------------------------------------------------

// parse common input and output parts from xml
bool Context::parseInOutBase (QXmlQuery &_xml, InOut *_io)
{
  QString type = getXmlString (_xml, "string(@type)");

  if (getXmlString (_xml, "string(@name)").isEmpty () ||
      type.isEmpty ())
    return false;  

  _io->name_ = getXmlString (_xml, "string(@name)");
  _io->type_ = type;

  _io->shortDesc_ = getXmlString (_xml, "short/string(text())", _io->name ());
  _io->longDesc_ = getXmlString (_xml, "long/string(text())", _io->shortDesc_);

  // get type hints for supported types
  if (typeSupported (type))
  {
    foreach (QString hint, supportedTypes_[type]->supportedHints ())
    {
      QString value = getXmlString (_xml, hint + "/string(text())", "");
      if (!value.isEmpty ())
        _io->hints_[hint] = value;
    }
  }

  return true;
}

//------------------------------------------------------------------------------

// parse element function from xml
Function* Context::parseFunction (QXmlQuery &_xml, Element *_e)
{
  QString name = getXmlString (_xml, "string(@name)");
  if (name.isEmpty ())
    return NULL;

  Function *f = new Function (_e, name);

  f->shortDesc_ = getXmlString (_xml, "short/string(text())", f->name ());
  f->longDesc_ = getXmlString (_xml, "long/string(text())", f->shortDesc_);

  // add start element
  f->start_ = new Element (_e->context (), "start_" + _e->name () + "_" + name);

  f->start_->category_ = "Undefined";

  f->start_->shortDesc_ = "Start";
  f->start_->longDesc_ = "Start";

  f->start_->dataOut_ = new Output (f->start_);
  f->start_->dataOut_->name_ = "data";
  f->start_->dataOut_->type_ = "data";

  f->start_->dataOut_->shortDesc_ = DATA_NAME;
  f->start_->dataOut_->longDesc_ = DATA_NAME;

  f->start_->flags_ = ELEMENT_FLAG_NO_DELETE | ELEMENT_FLAG_SKIP_TOOLBOX;

  elements_.append (f->start_);

  // add end element
  f->end_ = new Element (_e->context (), "end_" + _e->name () + "_" + name);

  f->end_->category_ = "Undefined";

  f->end_->shortDesc_ = "End";
  f->end_->longDesc_ = "End";

  f->end_->dataIn_ = new Input (f->end_);
  f->end_->dataIn_->name_ = "data";
  f->end_->dataIn_->type_ = "data";

  f->end_->dataIn_->shortDesc_ = DATA_NAME;
  f->end_->dataIn_->longDesc_ = DATA_NAME;

  f->end_->flags_ = ELEMENT_FLAG_NO_DELETE | ELEMENT_FLAG_SKIP_TOOLBOX;

  elements_.append (f->end_);

  // inputs
  _xml.setQuery ("inputs/input");

  QXmlResultItems inouts;

  if (_xml.isValid ())
  {
    _xml.evaluateTo (&inouts);
    QXmlItem item (inouts.next ());
    while (!item.isNull ())
    {
      QXmlQuery q (_xml);
      q.setFocus (item);
      Output *o = new Output (f->start_);
      if (parseInOutBase(q, o))
      {
        f->start_->outputs_.append (o);
      }
      else
        delete o;
      item = inouts.next ();
    }

  }

  // outputs
  _xml.setQuery ("outputs/output");

  if (_xml.isValid ())
  {
    _xml.evaluateTo (&inouts);
    QXmlItem item (inouts.next ());
    while (!item.isNull ())
    {
      QXmlQuery q (_xml);
      q.setFocus (item);
      Input *i = new Input (f->end_);
      if (parseInOutBase(q, i))
      {
        i->state_ = Input::NoUserInput | Input::NoRuntimeUserInput;
        f->end_->inputs_.append (i);
      }
      else
        delete i;
      item = inouts.next ();
    }

  }

  // add end node only if we have outputs
  if (!f->end_->inputs ().isEmpty ())
  {
    // Generate end node return code

    QString endCode = "return { ";
    foreach (Input *i, f->end_->inputs ())
      endCode += i->name () + " : [input=\"" + i->name () + "\"], ";
    endCode.remove (endCode.length () - 2, 2);
    endCode += " };\n";

    f->end_->code_ = endCode;
  }

  return f;
}

//------------------------------------------------------------------------------

/// Gets the string of a xml query
QString Context::getXmlString (QXmlQuery &_xml, QString _expr, QString _default)
{
  QStringList sl;

  _xml.setQuery (_expr);

  if (_xml.isValid () && _xml.evaluateTo (&sl) && sl.length () == 1)
    return sl[0];

  return _default;
}

//------------------------------------------------------------------------------

/// Converts the given string to bool
bool Context::strToBool (QString _str)
{
  if (_str == "1" || _str == "true"  || _str == "True" || _str == "TRUE"  ||
      _str == "yes"  || _str == "Yes"  || _str == "YES")
    return true;
  return false;
}

//------------------------------------------------------------------------------

/// Registers a supported datatype
void Context::registerType(Type * _type)
{
  types_.append (_type);

  foreach (const QString &s, _type->supportedTypes())
    supportedTypes_.insert (s, _type);
}

//------------------------------------------------------------------------------

/// Is the given type supported
bool Context::typeSupported(QString _type)
{
  return supportedTypes_.contains (_type);
}

//------------------------------------------------------------------------------

/// Can the given types be converted to each other
bool Context::canConvert(QString _type1, QString _type2)
{
  if (!typeSupported (_type1) || !typeSupported (_type2))
    return false;
  return supportedTypes_[_type1]->canConvertTo (_type2) ||
         supportedTypes_[_type2]->canConvertTo (_type1);
}

//------------------------------------------------------------------------------

/// Get type object for given type name
Type * Context::getType(QString _type)
{
  if (typeSupported (_type))
    return supportedTypes_[_type];
  return NULL;
}

//------------------------------------------------------------------------------

}
