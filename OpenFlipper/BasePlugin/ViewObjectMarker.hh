//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================

#ifndef VIEWOBJECTMARKER_HH
#define VIEWOBJECTMARKER_HH

//== INCLUDES =================================================================
#include <ACG/Math/VectorT.hh>

//== FORWARDDECLARATIONS ======================================================

class BaseObject;

//== CLASS DEFINITION =========================================================


/** \class ViewObjectMarker ViewObjectMarker.hh <OpenFlipper/BasePlugin/ViewObjectMarker.hh>

    Abstract base class to mark objects with help of the stencil buffer
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
  virtual bool stencilRefForObject (BaseObject *_obj, GLuint &_reference) = 0;

  /** Per reference number blending values
    @param _reference stencil reference for blending
    @param _src sfactor parameter for glBlendFunc function
    @param _dst dfactor parameter for glBlendFunc function
    @param _color color used for blending
    @return should blending be berformed for this reference
  */
  virtual bool blendForStencilRefNumber (GLuint _reference, GLenum &_src, GLenum &_dst, ACG::Vec4f &_color) { return false; };

  /** Per reference bit blending values
    @param _refbit stencil reference bit for blending
    @param _src sfactor parameter for glBlendFunc function
    @param _dst dfactor parameter for glBlendFunc function
    @param _color color used for blending
    @return should blending be berformed for this reference
  */
  virtual bool blendForStencilRefBit (GLuint _refbit, GLenum &_src, GLenum &_dst, ACG::Vec4f &_color) { return false; };

};


#endif