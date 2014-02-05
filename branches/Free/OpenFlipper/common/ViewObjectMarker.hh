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

#ifndef VIEWOBJECTMARKER_HH
#define VIEWOBJECTMARKER_HH

//== INCLUDES =================================================================
#include <ACG/Math/VectorT.hh>

//== FORWARDDECLARATIONS ======================================================

class BaseObject;

//== CLASS DEFINITION =========================================================


/** \class ViewObjectMarker ViewObjectMarker.hh <OpenFlipper/BasePlugin/ViewObjectMarker.hh>

    Abstract base class to mark objects with help of the stencil buffer

    This system allows to blend the rendered object with a color,
    to visualize different object properties/states.

    The ViewObjectMarker class can be activated for a examiner. The examiner will
    then call stencilRefForObject for each object. If stencilRefForObject returns
    "true" then the object _obj will be painted with the stencil buffer
    reference _reference (has to be != 0).

    There are 2 operation modes

    PerNumber:
      The examiner will call blendForStencilRefNumber for each of the
      returned references of stencilRefForObject to ask for the blend values.

    PerBit:
      The examiner will call blendForStencilRefNumberBit for each of the bits
      in the returned references of stencilRefForObject to ask for the blend values.

      PerBit handling allows to blend the object multiple times with different blend
      values, to visualize multiple properties at once, but is limited to the number
      of bits available in the stencil buffer (usually 8).

 **/

class DLLEXPORT ViewObjectMarker
{
//-------------------------------------------------------------- public methods
public:

  //--------------------------------------------------- destructor

  /// Blending type for this marker
  enum Type {
    /// Mark per returned reference
    PerNumber,
    /// Mark per returned reference bits
    PerBit
  };

  /// Destructor.
  virtual ~ViewObjectMarker () {};

  virtual Type type () { return PerNumber; };

  /** Get stencil reference for object
    @param _obj Object
    @param _reference stencil reference for object painting
    @return should the reference be used
  */
  virtual bool stencilRefForObject (BaseObjectData *_obj, GLuint &_reference) = 0;

  /** Per reference number blending values
    @param _reference stencil reference for blending
    @param _src sfactor parameter for ACG::GLState::blendFunc function
    @param _dst dfactor parameter for ACG::GLState::blendFunc function
    @param _color color used for blending
    @return should blending be berformed for this reference
  */
  virtual bool blendForStencilRefNumber (GLuint _reference, GLenum & _src, GLenum & _dst, ACG::Vec4f & _color) { return false; };

  /** Per reference bit blending values
    @param _refbit stencil reference bit for blending
    @param _src sfactor parameter for ACG::GLState::blendFunc function
    @param _dst dfactor parameter for ACG::GLState::blendFunc function
    @param _color color used for blending
    @return should blending be performed for this reference
  */
  virtual bool blendForStencilRefBit (GLuint _refbit , GLenum & _src , GLenum & _dst , ACG::Vec4f & _color ) { return false; };

};


#endif

