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
//   $Revision: 1720 $
//   $Author: moebius $
//   $Date: 2008-05-09 14:15:53 +0200 (Fr, 09 Mai 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS QtShaderDialog
//
//=============================================================================


#ifndef ACG_QTSHADERDIALOG_HH
#define ACG_QTSHADERDIALOG_HH


//== INCLUDES =================================================================


#include "ui_QtShaderDialogUi.hh"
#include "../Config/ACGDefines.hh"

#include <QColor>
#include <QDialog>

//== FORWARDDECLARATIONS ======================================================


namespace ACG {
  namespace SceneGraph {
    class ShaderNode;
    class BaseNode;
  }
}


//== NAMESPACE ================================================================


namespace ACG {
namespace QtWidgets {


//== CLASS DEFINITION =========================================================


class ACGDLLEXPORT QtShaderDialog : public QDialog
{
  Q_OBJECT

public:

  QtShaderDialog( QWidget                  * _parent,
		    SceneGraph::ShaderNode * _node );

  ~QtShaderDialog() {}


signals:

  void signalNodeChanged( ACG::SceneGraph::BaseNode * _node );


private:

private slots:

  void applyChanges();
  void undoChanges();

  void reject();

  void comboChanged (  int index );

private:
  SceneGraph::ShaderNode * node_;

  Ui::QtShaderDialogUi ui_;

};


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
#endif // ACG_QTSHADERDIALOG_HH defined
//=============================================================================

