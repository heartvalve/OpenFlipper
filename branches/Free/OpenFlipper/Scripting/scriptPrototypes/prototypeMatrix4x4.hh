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




//
// C++ Interface: prototypeMatrix4x4
//
// Description: 
//
//
// Author: Jan Möbius <moebius@cs.rwth-aachen.de>, (C) 2007
//

#ifndef PROTOTYPEVMATRIX4X4_HH 
#define PROTOTYPEVMATRIX4X4_HH 

/** \page scripting_matrix_type Matrix data type used for scripting
 *
 *   The matrix type Matrix4x4 is used in the scripting language to handle matrix operations.
 *   The implementation of the type for scripting can be found in prototypeMatrix4x4 .
 *
 *   You can use the following functions:
 *   \code
 *   var matrix = Matrix4x4(1,0,0,1,
 *                          0,1,0,2,
 *                          0,0,1,3,
 *                          0,0,0,1 )  // Create a matrix that contains only a translation (1,2,3).
 *
 *   var vec = Vector(1,2,3)  // Create a vector
 *
 *   var vec2 = matrix.transform_point(vec);  // Transforms a point by the given matrix (translation is included)
 *   var vec3 = matrix.transform_vector(vec); // Transforms a vector by the given matrix (translation is omitted)
 *
 *   \endcode
 *
 */

#include <QtCore/QObject>
#include <QtScript/QtScript>
#include "OpenFlipper/common/Types.hh"
 
class prototypeMatrix4x4 : public QObject , public QScriptable
{

  Q_OBJECT

  public:
    prototypeMatrix4x4(QObject *parent = 0);

  public slots:

    /** \brief Matrix vector product
     *
     * Transforms a vector by the given matrix (translation is omitted)
     */
    QScriptValue transform_vector(QScriptValue _vector);

    /** \brief Matrix vector product
     *
     * Transforms a point by the given matrix (translation is included)
     */
    QScriptValue transform_point(QScriptValue _vector);

  public Q_SLOTS:
    QString toString() const;

};
 
      
#endif // PROTOTYPEVMATRIX4X4_HH
