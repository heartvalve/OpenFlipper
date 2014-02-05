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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS QtLasso
//
//=============================================================================


#ifndef ACG_QTLASSO_HH
#define ACG_QTLASSO_HH


//== INCLUDES =================================================================

// ACG
#include "../GL/GLState.hh"
#include "../Math/VectorT.hh"

// Qt
//#include <QGl>
//#include <QNameSpace>
#include <QMouseEvent>


//== FORWARDDECLARATIONS ======================================================


//== NAMESPACES ===============================================================

namespace ACG {


//== CLASS DEFINITION =========================================================


class ACGDLLEXPORT QtLasso : public QObject
{
  Q_OBJECT

public:

  QtLasso(GLState& _glstate);
  ~QtLasso();

  void reset() { is_active_ = false; free_mask(); }
  bool is_active() const { return is_active_; }

  bool is_vertex_selected(const Vec3d& _v);

  enum SelectionMode {
    NewSelection,
    AddToSelection,
    DelFromSelection
  };


public slots:

  void slotMouseEvent(QMouseEvent* _event);


signals:

  void signalLassoSelection(ACG::QtLasso::SelectionMode);


private:

  // hide copy & assignement
  QtLasso(const QtLasso& _rhs);
  QtLasso& operator=(const QtLasso& _rhs);

  void create_mask();
  void free_mask();


  GLState&                glstate_;
  Vec2i                   first_point_, last_point_, rubberband_point_;
  unsigned char          *mask_;
  unsigned int            mask_width_, mask_height_;
  bool                    is_active_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_QTLASSO_HH defined
//=============================================================================

