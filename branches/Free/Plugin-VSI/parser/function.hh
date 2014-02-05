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

#ifndef VSI_FUNCTION_HH_
#define VSI_FUNCTION_HH_

//== INCLUDES =================================================================
#include <QString>
#include <QVector>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class Input;
class Output;
class InOut;
class Context;
class Element;

//== CLASS DEFINITION =========================================================

/** Class that represents a function in the visual script interface
  * A function represents a editable script sub element that will be executed
  * by its element (ex. loop/condition contents)
  */
class Function {

  public:

    /// Constructor
    Function (Element *_element, QString _name);

    /// Destructor
    ~Function ();

    /// Element that this function belongs to
    Element *element () const { return element_; };

    /// Name
    QString name () const { return name_; };

    /// Short description
    const QString &shortDescription () const { return shortDesc_; };

    /// Long description
    const QString &longDescription () const { return longDesc_; };

    /// Start element of this function
    Element *start () const { return start_; };

    /// End element of this function (can be NULL)
    Element *end () const { return end_; };

    friend class Context;

  private:
    Element *element_;

    QString name_;
    QString shortDesc_;
    QString longDesc_;

    Element *start_;
    Element *end_;
};

//=============================================================================
}
//=============================================================================

#endif
