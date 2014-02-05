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

#ifndef VSI_ELEMENT_HH_
#define VSI_ELEMENT_HH_

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
class Function;

#define ELEMENT_FLAG_NO_DELETE 0x1
#define ELEMENT_FLAG_SKIP_TOOLBOX 0x2

//== CLASS DEFINITION =========================================================

/** Class that represents an element of the visual scripting interface.
  */
class Element {

  public:

    /// Constructor
    Element (Context *_context, QString _name);

    /// Destructor
    ~Element ();

    /// Context of element
    Context *context () const { return context_; };

    /// Element name
    QString name () const { return name_; };

    /// Element category
    const QString &category () const { return category_; };

    /// Short description
    const QString &shortDescription () const { return shortDesc_; };

    /// Long description
    const QString &longDescription () const { return longDesc_; };

    /// Inputs
    const QVector<Input *> &inputs () const { return inputs_; };

    /// Outputs
    const QVector<Output *> &outputs () const { return outputs_; };

    /// Functions
    const QVector <Function*> &functions () const { return functions_; };

    /// Scenegraph input
    Input *dataIn () { return dataIn_; };

    /// Scenegraph output
    Output *dataOut () { return dataOut_; };

    /// Flags
    unsigned int flags () const { return flags_; };

    /// Precode segment
    QString precode () const { return precode_; };

    /// Code segment
    QString code () const { return code_; };

    /// Returns an unused id number
    unsigned int getNewId ();

    /// sets the minimum for an unused id
    void setMinId (unsigned int _id);

    friend class Context;

  private:
    Context *context_;

    QString name_;
    QString category_;
    QString shortDesc_;
    QString longDesc_;

    QString precode_;
    QString code_;

    QVector <Input*> inputs_;
    QVector <Output*> outputs_;
    QVector <Function*> functions_;

    Input *dataIn_;
    Output *dataOut_;

    unsigned int flags_;

    unsigned int id_;
};

//=============================================================================
}
//=============================================================================

#endif
