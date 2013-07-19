/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS QtCoordFrameDialog
//
//=============================================================================


#ifndef ACG_QTCOORDFRAMEDIALOG_HH
#define ACG_QTCOORDFRAMEDIALOG_HH


//== INCLUDES =================================================================


#include "ui_QtCoordFrameDialogUi.hh"
#include "../Math/VectorT.hh"
#include <vector>

#include <QDialog>
#include <QWidget>


//== FORWARDDECLARATIONS ======================================================


namespace ACG {
  namespace SceneGraph {
    class CoordFrameNode;
    class BaseNode;
  }
}


//== NAMESPACE ================================================================


namespace ACG {
namespace QtWidgets {


//== CLASS DEFINITION =========================================================


class ACGDLLEXPORT QtCoordFrameDialog : public QDialog
{
  Q_OBJECT

public:

  QtCoordFrameDialog( QWidget* parent,
		      SceneGraph::CoordFrameNode* _node,
		      Qt::WindowFlags fl = Qt::Widget );

  ~QtCoordFrameDialog() {}


  void show();


signals:

  void signalNodeChanged(ACG::SceneGraph::BaseNode* _node);

private slots:

  void add_x_plane();
  void add_y_plane();
  void add_z_plane();
  void del_x_plane();
  void del_y_plane();
  void del_z_plane();
  void mod_x_plane();
  void mod_y_plane();
  void mod_z_plane();
  void apply_changes();
  void undo_changes();

private:

  void update_values();

  void combo2planes(const QComboBox* _combo,
		    std::vector<float>& _planes);
  void planes2combo(const std::vector<float>& _planes,
		    QComboBox* _combo);


private:

  SceneGraph::CoordFrameNode* node_;

  std::vector<float> x_planes_bak_;
  std::vector<float> y_planes_bak_;
  std::vector<float> z_planes_bak_;

  Ui::QtCoordFrameDialogUi ui_;
};


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
#endif // ACG_QTCOORDFRAMEDIALOG_HH defined
//=============================================================================

