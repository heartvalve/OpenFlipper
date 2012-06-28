/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
// C++ Interface: prototypeVec3d
//
// Description: 
//
//
// Author: Jan Möbius <moebius@cs.rwth-aachen.de>, (C) 2007
//

/** \page scripting_vector_type Vector data type for scripting
 *
 *   The vector data type is corresponding to the Vec3d type in C++. It does not support all
 *   of its operations yet. The implementation of the scripting type is done in prototypeVec3d.
 *
 *   You can use the following functions:
 *   \code
 *      var vec = Vector(1,2,3)
 *      var vec2 = Vector(10,20,30)
 *
 *      vec.multiply(0.5)  // Multiply vec with 0.5
 *      vec.add(vec2)      // Add vec2 to vec
 *      vec.sub(vec2)      // Subtract vec2 from vec
 *
 *      print(vec)         // Output the current value: 0.5,1,1.5
 *   \endcode
 *
 */

#ifndef PROTOTYPEVEC3D_HH 
#define PROTOTYPEVEC3D_HH 


#include <QtCore/QObject>
#include <QtScript/QtScript>
 
/** \class prototypeVec3d
 *
 * This class wraps the Vec3d to the scripting language
 *
 * \ref scripting_vector_type
 *
 */
class prototypeVec3d : public QObject , public QScriptable
{
     Q_OBJECT
 
 public:
     prototypeVec3d(QObject *parent = 0);

 public slots:
     /** \brief Multiplies the given vector with the scalar
      *
      * @param _scalar Scalar value that gets multiplied
      */
     void multiply(QScriptValue _scalar);

     /** \brief Adds another vector to this vector
      *
      * @param _vector Vector that should be added
      */
     void add(QScriptValue _vector);

     /** \brief Subtracts another vector from this vector
      *
      * @param _vector Vector that should be subtracted
      */
     void sub(QScriptValue _vector);

 public Q_SLOTS:
     QString toString() const;

};
 
      
#endif // PROTOTYPEVEC3D_HH

