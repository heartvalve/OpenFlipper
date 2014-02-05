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
//  CLASS QtShaderDialog - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtShaderDialog.hh"
#include "../Scenegraph/ShaderNode.hh"
#include "../Scenegraph/DrawModes.hh"
#include <QFileInfo>
#include <QDir>

//== NAMESPACES ============================================================== 


namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ========================================================== 


QtShaderDialog::QtShaderDialog( QWidget                * _parent,
				SceneGraph::ShaderNode * _node )
  : QDialog( _parent ),
    node_(_node)
{
  ui_.setupUi( this );
  
  ACG::SceneGraph::DrawModes::DrawMode drawmode = ACG::SceneGraph::DrawModes::DEFAULT;
  
  while ( drawmode != ACG::SceneGraph::DrawModes::UNUSED )
  {
    ui_.drawModeBox->addItem( QString( drawmode.description().c_str() ) );
    ++drawmode ;
  }
  
  ui_.shaderDir->setText( QString(_node->shaderDir().c_str()) );
  ui_.vertexShader->setText( QString(_node->vertexShaderName(ACG::SceneGraph::DrawModes::DEFAULT).c_str()) );
  ui_.fragmentShader->setText( QString(_node->fragmentShaderName(ACG::SceneGraph::DrawModes::DEFAULT).c_str()) ); 
  
  connect( ui_.okButton, SIGNAL( clicked() ),
           this, SLOT( applyChanges() ) );
  connect( ui_.cancelButton, SIGNAL( clicked() ),
           this, SLOT( reject() ) );
  
  connect( ui_.drawModeBox, SIGNAL( currentIndexChanged( int ) ),
           this, SLOT( comboChanged( int ) ) );
}


//-----------------------------------------------------------------------------


void QtShaderDialog::reject()
{
  std::cerr << "reject" << std::endl;
  undoChanges();
  QDialog::reject();
}


//-----------------------------------------------------------------------------


void QtShaderDialog::applyChanges()
{
  // Get and test shader directory
  std::string shaderDirectory("");
  
  QString shaderDir = ui_.shaderDir->text();
  QDir dir(shaderDir);
  
  if ( dir.exists() ) {
    if ( ! shaderDir.endsWith('/' ) && ! shaderDir.endsWith( '\\' ) ) {
      shaderDir += "/"; 
    }
    
    shaderDirectory = std::string( shaderDir.toUtf8() );
    node_->setShaderDir( shaderDirectory );
    
  } else {
    std::cerr << "Shader directory does not exist" << std::string( shaderDir.toUtf8() ) << std::endl; 
    return;
  }
  
  unsigned int drawMode = 1;
  
  for ( int i = 0 ; i < ui_.drawModeBox->currentIndex() ; ++i )
    drawMode *= 2; 
  
  node_->setShader(drawMode, 
                   std::string(  ui_.vertexShader->text().toUtf8() ),
                   std::string(  ui_.fragmentShader->text().toUtf8() ) );
          
  emit signalNodeChanged(node_);
  
  accept();
}


//-----------------------------------------------------------------------------


void QtShaderDialog::undoChanges()
{
  std::cerr << "undo" << std::endl;
  emit signalNodeChanged(node_);
}

void QtShaderDialog::comboChanged ( int index ) {
  unsigned int drawMode = 1;
  
  for ( int i = 0 ; i < index; ++i )
    drawMode *= 2;  
  
  QString vertexShader(node_->vertexShaderName(drawMode).c_str());
  QString fragmentShader(node_->fragmentShaderName(drawMode).c_str());
  
  
  QString shaderDir( node_->shaderDir().c_str() );
  vertexShader   = vertexShader.remove( shaderDir );
  fragmentShader = fragmentShader.remove( shaderDir );
  
  ui_.vertexShader->setText( vertexShader );
  ui_.fragmentShader->setText( fragmentShader );
}


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
