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

#ifndef VSI_CONTEXT_HH
#define VSI_CONTEXT_HH

//== INCLUDES =================================================================
#include <QVector>
#include <QStringList>
#include <QXmlQuery>

#include "element.hh"

//== FORWARDDECLARATIONS ======================================================
class QScriptEngine;

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class Input;
class Output;
class InOut;
class Type;

//== CLASS DEFINITION =========================================================

/** Class holding all the informations / parsed xml metadata for the visual scripting interface
 */
class Context {

  public:
    /// Constructor
    Context (QScriptEngine *_engine);

    /// Destructor
    ~Context ();

    /// Parse xml content
    void parse (QByteArray _xml);

    /// Returns all available elements
    const QVector<Element *>& elements () const { return elements_; };

    /// Returns all available elements for a given category
    QVector<Element *> elements (QString _category);

    /// Returns the element with a given name
    Element *element (QString _name);

    /// List of categories
    QStringList categories ();

    /// Registers a supported datatype
    void registerType (Type *_type);

    /// Is the given type supported
    bool typeSupported (QString _type);

    /// Get type object for given type name
    Type *getType (QString _type);

    /// Can the given types be converted to each other
    bool canConvert (QString _type1, QString _type2);

    /// Return script engine pointer
    QScriptEngine *scriptEngine () { return scriptEngine_; };

    /// Converts the given string to bool
    static bool strToBool (QString _str);

    /// Gets the string of a xml query
    static QString getXmlString (QXmlQuery &_xml, QString _expr, QString _default = "");

  private:

    // parse element from xml
    void parseElement (QXmlQuery &_xml);

    // parse element input from xml
    Input *parseInput (QXmlQuery &_xml, Element *_e);

    // parse element output from xml
    Output *parseOutput (QXmlQuery &_xml, Element *_e);

    // parse element function from xml
    Function *parseFunction (QXmlQuery &_xml, Element *_e);

    // parse common input and output parts from xml
    bool parseInOutBase (QXmlQuery &_xml, InOut *_io);

  private:
    QVector <Element *> elements_;

    QMap <QString, Type*> supportedTypes_;

    QList <Type *> types_;

    QScriptEngine *scriptEngine_;
};

//=============================================================================
}
//=============================================================================

#endif
