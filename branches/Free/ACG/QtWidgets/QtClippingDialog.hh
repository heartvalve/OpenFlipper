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
//  CLASS QtClippingDialog
//
//=============================================================================


#ifndef ACG_QTCLIPPINGDIALOG_HH
#define ACG_QTCLIPPINGDIALOG_HH


//== INCLUDES =================================================================


#include "ui_QtClippingDialogUi.hh"
#include "../Config/ACGDefines.hh"

#include <QDialog>


//== FORWARDDECLARATIONS ======================================================


namespace ACG {
  namespace SceneGraph {
    class ClippingNode;
    class BaseNode;
  }
}


//== NAMESPACES ===============================================================


namespace ACG {
namespace QtWidgets {


//== CLASS DEFINITION =========================================================


class ACGDLLEXPORT QtClippingDialog : public QDialog
{
  Q_OBJECT

public:

  /// Default constructor
  QtClippingDialog( QWidget                  * _parent,
		    SceneGraph::ClippingNode * _node );

  /// Destructor
  ~QtClippingDialog() {}


  void update_values();
  void show();
  void set_sweep_range(float _radius);


signals:

  void signalNodeChanged(ACG::SceneGraph::BaseNode* _node);

private slots:

  void set_plane();
  void sweep_plane(int _i);

private:

  SceneGraph::ClippingNode * node_;
  Ui::QtClippingDialogUi ui_;
};


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
#endif // ACG_QTCLIPPINGDIALOG_HH defined
//=============================================================================

