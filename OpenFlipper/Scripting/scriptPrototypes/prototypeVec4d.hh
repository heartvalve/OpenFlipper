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
*   $Revision: 14819 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2012-06-29 13:42:49 +0200 (Fr, 29 Jun 2012) $                     *
*                                                                            *
\*===========================================================================*/




//
// C++ Interface: prototypeVec4d
//
// Description: 
//
//
// Author: Jan Möbius <moebius@cs.rwth-aachen.de>, (C) 2013
//

/** \page scripting_vector_type_4d Vector4d data type for scripting
 *
 *   The vector data type is corresponding to the Vec4d type in C++. The implementation of the
 *   scripting type is done in prototypeVec3d.
 *
 *   You can use the following functions:
 *   \code
 *      var vec = Vector(1,2,3,4)
 *      var vec2 = Vector(10,20,30,4)
 *
 *      vec.multiply(0.5)           // Multiply vec with 0.5
 *      vec.add(vec2)               // Add vec2 to vec
 *      vec.sub(vec2)               // Subtract vec2 from vec
 *
 *      var norm    = vec.norm()    // Calculate the length of the vector
 *      var sqrnorm = vec.sqrnorm() // Calculate the squared length of the vector
 *
 *      var sprod = vec.sprod(vec)  //Compute scalar product with the vector and itself
 *
 *      vec.normalize()             // Normalize vector
 *
 *      vec.zero()                  // Set all components of the vector to zero
 *
 *      print(vec)                  // Output vector to the console
 *
 *   \endcode
 *
 */

#ifndef PROTOTYPEVEC4D_HH
#define PROTOTYPEVEC4D_HH


#include <QtCore/QObject>
#include <QtScript/QtScript>
 
/** \class prototypeVec4d
 *
 * This class wraps the Vec4d to the scripting language
 *
 * \ref scripting_vector_type
 *
 */
class prototypeVec4d : public QObject , public QScriptable
{
     Q_OBJECT
 
 public:
     prototypeVec4d(QObject *parent = 0);

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

     /** \brief Resets all components of the vector to zero
      *
      */
     void zero();


     /** \brief Calculate scalar product
      *
      * @param _vector Second vector for scalar product (Can be the vector itself)
      * @return Scalar product value
      */
     QScriptValue sprod(QScriptValue _vector);

     /** \brief Calculate the Euclidean norm of the vector
      *
      * @return Norm of the vector
      */
     QScriptValue norm();

     /** \brief Calculate the squared Euclidean norm of the vector
      *
      * @return Squared norm of the vector
      */
     QScriptValue sqrnorm();


     /** \brief Normalize the vector
      */
     void normalize();

 public Q_SLOTS:
     QString toString() const;

};
 
      
#endif // PROTOTYPEVEC4D_HH

