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

