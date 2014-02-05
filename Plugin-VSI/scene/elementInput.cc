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
#include "elementInput.hh"
#include "elementOutput.hh"
#include "sceneElement.hh"
#include "connectionPoint.hh"
#include "connection.hh"
#include "graphicsScene.hh"
#include "text.hh"
#include "../parser/input.hh"
#include "../parser/element.hh"
#include "../parser/context.hh"
#include "../parser/type.hh"
#include "../parser/typeWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::ElementInput - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ElementInput::ElementInput (Input *_in, SceneElement *_parent) :
  ElementInOut (_in, _parent),
  in_ (_in),
  isSet_ (false),
  value_ (""),
  forceAsk_ (false)
{
  if (state () & Input::Optional)
    connectionPointItem ()->setState (ConnectionPoint::Optional);
  else if (state () & Input::NoUserInput)
    connectionPointItem ()->setState (ConnectionPoint::NoValue);
  else
    connectionPointItem ()->setState (ConnectionPoint::RuntimeInput);

  VSI::Type *t = inOut ()->element ()->context ()->getType (inOut ()->typeString ());

  if (t)
  {
    TypeWidget *tW = t->widget (inOut ()->hints (), inOut ()->typeString ());
    if (tW)
    {
      value_ = tW->toValue ();
      delete tW;
    }
  }

  typeTextItem ()->setBackground (false, true);
  typeTextItem ()->setBackgroundBrush (Qt::gray);
  typeTextItem ()->setBrush (Qt::black);
  descriptionTextItem ()->setBackground (false, true);
  descriptionTextItem ()->setBackgroundBrush (Qt::white);
  descriptionTextItem ()->setBrush (Qt::black);

  connectionPointItem ()->setWidth (qMax (typeTextItem ()->preferredHeight (),
                                          descriptionTextItem ()->preferredHeight ()));
}

//------------------------------------------------------------------------------

/// Destructor
ElementInput::~ElementInput ()
{
}

//------------------------------------------------------------------------------

/// Returns state of valid flag (needed during code generation)
bool ElementInput::valid()
{
  if (!connections ().isEmpty ())
    return valid_;

  if (isSet () || !(state () & Input::NoRuntimeUserInput) || state () & Input::Optional)
    return true;

  return false;
}

//------------------------------------------------------------------------------

/// VSI::Input state passthrough
unsigned int VSI::ElementInput::state()
{
  return in_->state ();
}

//------------------------------------------------------------------------------

/// Add connection
void ElementInput::addConnection(Connection * _conn)
{
  ElementInOut::addConnection (_conn);

  // update state
  connectionPointItem ()->setState (ConnectionPoint::Connected);
}

//------------------------------------------------------------------------------

/// Remove connection
void ElementInput::removeConnection (Connection * _conn)
{
  ElementInOut::removeConnection (_conn);

  // update state
  if (connections().isEmpty())
  {
    if (isSet ())
      connectionPointItem ()->setState (ConnectionPoint::UserInput);
    else if (state () & Input::Optional && !forceAsk_)
    {
      connectionPointItem ()->setState (ConnectionPoint::Optional);
    }
    else if (state () & Input::NoUserInput)
    {
      connectionPointItem ()->setState (ConnectionPoint::NoValue);
    }
    else
      connectionPointItem ()->setState (ConnectionPoint::RuntimeInput);
  }
}

//------------------------------------------------------------------------------

/// "Set" flag to mark input as set by user
void ElementInput::set(bool _set)
{
  if (isSet_ != _set)
    element ()->graphicsScene ()->contentChange ();
  isSet_ = _set;

  // update state
  if (connections().isEmpty())
  {
    if (isSet ())
      connectionPointItem ()->setState (ConnectionPoint::UserInput);
    else if (state () & Input::Optional && !forceAsk_)
    {
      connectionPointItem ()->setState (ConnectionPoint::Optional);
    }
    else if (state () & Input::NoUserInput)
    {
      connectionPointItem ()->setState (ConnectionPoint::NoValue);
    }
    else
      connectionPointItem ()->setState (ConnectionPoint::RuntimeInput);
  }
}

//------------------------------------------------------------------------------

/// Return "set" flag
bool ElementInput::isSet ()
{
  // Only configured input
  if ((state () & Input::NoRuntimeUserInput) && (state () & Input::NoExternalInput))
    return true;
  return isSet_;
}

//------------------------------------------------------------------------------

/// Save to xml
void ElementInput::saveToXml(QDomDocument & _doc, QDomElement & _root)
{
  QDomText t;

  QDomElement main = _doc.createElement("input");
  _root.appendChild(main);
  main.setAttribute ("name",in_->name ());


  QDomElement set = _doc.createElement("is_set");
  main.appendChild(set);
  t = _doc.createTextNode(isSet_ ? "true" : "false");
  set.appendChild(t);

  if (state () & Input::Optional && forceAsk_)
  {
    QDomElement fA = _doc.createElement("force_ask");
    main.appendChild(fA);
    t = _doc.createTextNode(isSet_ ? "true" : "false");
    fA.appendChild(t);
  }

  if (isSet_)
  {
    QDomElement val = _doc.createElement("value");
    main.appendChild(val);
    t = _doc.createTextNode(value_);
    val.appendChild(t);
  }

  foreach (Connection *c, connections ())
  {
    QDomElement conn = _doc.createElement("connection");
    main.appendChild(conn);
    conn.setAttribute ("element",c->output ()->element ()->element ()->name ());
    conn.setAttribute ("element_id",QString::number (c->output ()->element ()->id ()));
    conn.setAttribute ("output",c->output ()->inOut ()->name ());
  }


}

//------------------------------------------------------------------------------

/// Load from xml
void ElementInput::loadFromXml(QXmlQuery & _xml)
{
  QString val = Context::getXmlString (_xml, "is_set/string(text())");

  isSet_ = Context::strToBool (val);

  if (state () & Input::Optional)
  {
    val = Context::getXmlString (_xml, "force_ask/string(text())");
    forceAsk_ = Context::strToBool (val);
  }

  if (isSet_)
  {
    value_ = Context::getXmlString (_xml, "value/string(text())");
  }

  if (isSet ())
    connectionPointItem ()->setState (ConnectionPoint::UserInput);
  else if (state () & Input::Optional && !forceAsk_)
  {
    connectionPointItem ()->setState (ConnectionPoint::Optional);
  }
  else if (state () & Input::NoUserInput)
  {
    connectionPointItem ()->setState (ConnectionPoint::NoValue);
  }
  else
    connectionPointItem ()->setState (ConnectionPoint::RuntimeInput);

}

//------------------------------------------------------------------------------

/// Set to a user value
void ElementInput::setValue(QString _value)
{
  if (value_ != _value)
    element ()->graphicsScene ()->contentChange ();
  value_ = _value;
}

//------------------------------------------------------------------------------
}

