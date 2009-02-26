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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




// ============================================================================
//
//   Class Noncopyable
//
// ----------------------------------------------------------------------------
// $Id: Noncopyable.hh,v 1.1 2002-12-19 13:14:01 acgadm Exp $
// ============================================================================

#ifndef ACG_NONCOPYABLE_HH
#define ACG_NONCOPYABLE_HH


//== INCLUDES =================================================================


//== NAMESPACE ================================================================

namespace ACG {


//== CLASS DEFINITION =========================================================
   
/** This class demonstrates the non copyable idiom. In some cases it is
    important an object can't be copied. Deriving from Noncopyable makes sure
    all relevant constructor and operators are made inaccessable, for public
    AND derived classes.
**/
class Noncopyable
{
protected:
   Noncopyable(){}
   virtual ~Noncopyable(){}
   
private:
   /// Prevent access to copy constructor
   Noncopyable( const Noncopyable& );
   
   /// Prevent access to assignment operator
   const Noncopyable& operator=( const Noncopyable& );
};

// ============================================================================
} // end namespace ACG
// ============================================================================
#endif // ACG_NONCOPYABLE_HH
// ============================================================================
