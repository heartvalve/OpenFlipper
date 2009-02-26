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

