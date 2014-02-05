/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS ColorStack
//
//=============================================================================

#ifndef ACG_COLORSTACK_HH
#define ACG_COLORSTACK_HH


//== INCLUDES =================================================================

#include <vector>

#include "ACG/GL/gl.hh"
#include "../Math/VectorT.hh"
#include "ColorTranslator.hh"


//== NAMESPACES ===============================================================


namespace ACG {


//== CLASS DEFINITION =========================================================


/** This class can be used to implement a gl picking stack based on colors
*/
class ACGDLLEXPORT ColorStack
{
public:
   
  /// Default constructor.
  ColorStack();
   /// Destructor.
  ~ColorStack();

  
  /// init (takes current GL context/ like glInitNames (); glPushName (0))
  void initialize();
  /// has it been initialized?
  bool initialized() const { return initialized_; }

  /// sets the maximum index number used in current node 
  bool setMaximumIndex (unsigned int _idx);

  /// sets the current color the given index (like glLoadName)
  void setIndex (unsigned int _idx);

  /// gets the color instead of setting it directly
  Vec4uc getIndexColor (unsigned int _idx);

  /// creates a new node the stack (like glPushName)
  void pushIndex (unsigned int _idx);

  /// pops the current node from the stack (like glPopName)
  void popIndex ();

  /// converts the given color to index values on the stack
  std::vector<unsigned int> colorToStack (Vec4uc _rgba) const;

  /// returns maximal available index count
  unsigned int freeIndicies () const;

  /// Did an error occur during picking
  bool error () const { return error_ && initialized_; };

  /// returns the current color index
  unsigned int currentIndex () const;

private:

  // Internal class used realize the color stack

  class Node {
    public:
      Node (unsigned int _idx, Node *_parent, ColorTranslator *_ct);
      ~Node ();

      /// sets the maximum index number used in current node 
      bool setMaximumIndex (unsigned int _idx);

      /// sets the current color the given index (like glLoadName)
      bool setIndex (unsigned int _idx) const;

      /// gets the color instead of setting it directly
      bool getIndexColor (unsigned int _idx, Vec4uc &_rgba) const;

      /// creates a new node the stack (like glPushName)
      Node * pushIndex (unsigned int _idx);

      /// pops the current node from the stack (like glPopName)
      Node * popIndex ();

      void colorToStack (std::vector<unsigned int> &_stack, unsigned int _index);

      unsigned int startIndex () { return startIdx_; };
      unsigned int endIndex () { return endIdx_; };
      unsigned int colorIndex () { return colorStartIdx_; };

    private:
      Node                *parent_;
      unsigned int        index_;
      ColorTranslator     *translator_;
      std::vector<Node *> nodes_;

      unsigned int startIdx_;
      unsigned int endIdx_;
      unsigned int colorStartIdx_;
      unsigned int colorEndIdx_;

  };

  bool            initialized_;
  ColorTranslator translator_;
  Node            *root_;
  Node            *currentNode_;
  bool            error_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_COLORSTACK_HH defined
//=============================================================================

