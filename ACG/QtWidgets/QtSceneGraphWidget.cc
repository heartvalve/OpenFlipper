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
//  CLASS QtSceneGraphWidget - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


#include "QtSceneGraphWidget.hh"
#include "QtMaterialDialog.hh"
#include "QtTextureDialog.hh"
#include "QtClippingDialog.hh"
#include "QtCoordFrameDialog.hh"
#include "QtShaderDialog.hh"

#include "../Scenegraph/BaseNode.hh"
#include "../Scenegraph/DrawModes.hh"
#include "../Scenegraph/MaterialNode.hh"
#include "../Scenegraph/TextureNode.hh"
#include "../Scenegraph/ClippingNode.hh"
#include "../Scenegraph/ShaderNode.hh"
#include "../Scenegraph/CoordFrameNode.hh"

#include <QMenu>
#include <QCursor>
#include <QLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QStringList>


//== FORWARDDECLARATIONS ======================================================

using ACG::SceneGraph::BaseNode;


//== NAMESPACES ===============================================================

namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ========================================================== 

SceneGraphWidgetGenerator::SceneGraphWidgetGenerator() {
}

QWidget* SceneGraphWidgetGenerator::getWidget(SceneGraph::BaseNode* /*_node*/ ) {
  // Warn as this you have to derive from this class before it can be used!
  std::cerr << "SceneGraphWidgetGenerator::getWidget not implemented!"  << std::endl;
  return 0;
}

bool SceneGraphWidgetGenerator::canHandle(std::string _className) {
  // Warn as this you have to derive from this class before it can be used!
  std::cerr << "SceneGraphWidgetGenerator::canHandle  not implemented!" << _className << std::endl;
  return false;
}

std::string SceneGraphWidgetGenerator::handles() {
  // Warn as this you have to derive from this class before it can be used!
  std::cerr << "SceneGraphWidgetGenerator::canHandle  not implemented!" << std::endl;
  return std::string("Unset Type");
}

QString SceneGraphWidgetGenerator::contextMenuName() {
  // Warn as this you have to derive from this class before it can be used!
  std::cerr << "SceneGraphWidgetGenerator::contextMenuName  not implemented!" << std::endl;
  return QString("Context Menu name unset");
}

QtSceneGraphWidget::
QtSceneGraphWidget( QWidget              * _parent, 
		    SceneGraph::BaseNode * _rootNode ) :
  QTreeWidget( _parent ),
  rootNode_(0),
  curItem_(0),
  shiftPressed_(false)

{
  setRootIsDecorated(true);

  setSortingEnabled( false );

  setSelectionMode( QAbstractItemView::SingleSelection );


  setColumnCount( 4 );

  QStringList column_names;
  column_names.append( "Node" );
  column_names.append( "Type" );
  column_names.append( "Status" );
  column_names.append( "Mode" );

  setHeaderLabels( column_names );

  modeMenu_ = new QMenu( this );


  //
  // Setup the 'status' actions
  //

  statusActions_.menu_ = new QMenu( this );

  QActionGroup * status_ag = new QActionGroup( statusActions_.menu_ );
  status_ag->setExclusive( true );

  statusActions_.actionActive_ = new QAction( "Active", status_ag );
  statusActions_.actionActive_->setCheckable( true );
  statusActions_.actionActive_->setData( QVariant( BaseNode::Active ) );

  statusActions_.actionHideNode_ = new QAction( "Hide Node", status_ag );
  statusActions_.actionHideNode_->setCheckable( true );
  statusActions_.actionHideNode_->setData( QVariant( BaseNode::HideNode ) );

  statusActions_.actionHideChildren_ = new QAction( "Hide Children", status_ag );
  statusActions_.actionHideChildren_->setCheckable( true );
  statusActions_.actionHideChildren_->setData( QVariant( BaseNode::HideChildren ) );

  statusActions_.actionHideSubtree_ = new QAction( "Hide Subtree", status_ag );
  statusActions_.actionHideSubtree_->setCheckable( true );
  statusActions_.actionHideSubtree_->setData( QVariant( BaseNode::HideSubtree ) );

  statusActions_.menu_->addActions( status_ag->actions() );

  connect( status_ag, SIGNAL( triggered( QAction * ) ),
	   this, SLOT( slotStatusMenu( QAction * ) ) );



  connect( this, SIGNAL(itemPressed(QTreeWidgetItem*,int) ),
 	   this, SLOT(slotItemPressed(QTreeWidgetItem*,int)) );

  connect( this, SIGNAL(itemExpanded(QTreeWidgetItem*) ),
 	   this, SLOT(slotItemExpandedOrCollapsed(QTreeWidgetItem*)) );
  connect( this, SIGNAL(itemCollapsed(QTreeWidgetItem*) ),
 	   this, SLOT(slotItemExpandedOrCollapsed(QTreeWidgetItem*)) );

  update(_rootNode);

  setMinimumWidth( 600 );
  setMinimumHeight( 400 );

}


//-----------------------------------------------------------------------------


void 
QtSceneGraphWidget::
update(ACG::SceneGraph::BaseNode* _node)
{
  rootNode_ = _node;

  clear();

  Item * item = new Item( this, _node );

  SceneGraph::BaseNode::ChildRIter  cRIt(_node->childrenRBegin()),
                                    cREnd(_node->childrenREnd());
  for (; cRIt != cREnd; ++cRIt)
    update(*cRIt, item);
  
  expandToDepth ( 0 ); 

  resizeColumnToContents( 0 );
  resizeColumnToContents( 1 );
  resizeColumnToContents( 2 );
  resizeColumnToContents( 3 );

  setMinimumWidth( columnWidth(0) + columnWidth(1) + columnWidth(2) + columnWidth(3) );
}


//-----------------------------------------------------------------------------


void 
QtSceneGraphWidget::
update(SceneGraph::BaseNode* _node, Item* _parent)
{
  // create new item
  Item* item = new Item( _parent, _node );

  // process children
  SceneGraph::BaseNode::ChildRIter  cRIt(_node->childrenRBegin()),
                                    cREnd(_node->childrenREnd());
  for (; cRIt != cREnd; ++cRIt)
    update(*cRIt, item);
}


//-----------------------------------------------------------------------------


void 
QtSceneGraphWidget::
slotItemExpandedOrCollapsed( QTreeWidgetItem* /*_item*/ )
{
  resizeColumnToContents( 0 );
}


//-----------------------------------------------------------------------------


void 
QtSceneGraphWidget::
slotItemPressed( QTreeWidgetItem * _item,
		 int _col)
{

  if ( _item )
  {
    curItem_ = static_cast<Item* >(_item);
    BaseNode * node = curItem_->node();

    switch ( _col )
    {
      case 2:
      {
        statusActions_.actionActive_      ->setChecked( false );
        statusActions_.actionHideNode_    ->setChecked( false );
        statusActions_.actionHideChildren_->setChecked( false );
        statusActions_.actionHideSubtree_ ->setChecked( false );

        switch ( node->status() )
        {
          case BaseNode::Active:
            statusActions_.actionActive_->setChecked( true );
            break;
          case BaseNode::HideNode:
            statusActions_.actionHideNode_->setChecked( true );
            break;
          case BaseNode::HideChildren:
            statusActions_.actionHideChildren_->setChecked( true );
            break;
          case BaseNode::HideSubtree:
            statusActions_.actionHideSubtree_->setChecked( true );
            break;
        }
        statusActions_.menu_->popup( QCursor::pos() ); 
        break;
      }
      case 0: break;
      case 1: break;
      case 3:
      {
        modeMenu_->clear();

        QActionGroup * modeGroup = new QActionGroup( modeMenu_ );
        modeGroup->setExclusive( true );
        connect( modeGroup, SIGNAL( triggered( QAction * ) ),
                  this, SLOT( slotModeMenu( QAction * ) ) );

        ACG::SceneGraph::DrawModes::DrawMode availDrawModes( node->availableDrawModes() );
        availDrawModes |= SceneGraph::DrawModes::DEFAULT;
        
        ACG::SceneGraph::DrawModes::DrawMode currentDrawMode( node->drawMode() );

        std::vector< ACG::SceneGraph::DrawModes::DrawMode > available_modes( availDrawModes.getAtomicDrawModes() );

        
        for ( unsigned int i = 0; i < available_modes.size(); ++i )
        {
          ACG::SceneGraph::DrawModes::DrawMode id    = available_modes[i];
          std::string  descr = id.description();

          QAction * action = new QAction( descr.c_str(), modeGroup );
          action->setCheckable( true );
          action->setChecked ( currentDrawMode.containsAtomicDrawMode(id ) ) ;
          action->setData( QVariant( quint64(id.getIndex()) ) );
        }

        modeMenu_->addActions( modeGroup->actions() );

        if ( dynamic_cast<SceneGraph::MaterialNode*>( node ) )
        {
          modeMenu_->addSeparator();
          QAction * action = modeMenu_->addAction( "Edit material" );
          connect( action, SIGNAL( triggered() ),
                    this, SLOT( slotEditMaterial() ) );
        }
        
        if ( dynamic_cast<SceneGraph::TextureNode*>( node ) )
        {
          modeMenu_->addSeparator();
          QAction * action = modeMenu_->addAction( "Edit texture" );
          connect( action, SIGNAL( triggered() ),
                    this, SLOT( slotEditTexture() ) );
        }
        
        if ( dynamic_cast<SceneGraph::ShaderNode*>( node ) )
        {
          modeMenu_->addSeparator();
          QAction * action = modeMenu_->addAction( "Edit shaders" );
          connect( action, SIGNAL( triggered() ),
                    this, SLOT( slotEditShader() ) );
        }        

        if ( dynamic_cast<SceneGraph::ClippingNode*>( node ) )
        {
          modeMenu_->addSeparator();
          QAction * action = modeMenu_->addAction( "Edit clip planes" );
          connect( action, SIGNAL( triggered() ),
                    this, SLOT( slotEditClipPlanes() ) );
        }

        if ( dynamic_cast<SceneGraph::CoordFrameNode*>( node ) )
        {
          modeMenu_->addSeparator();
          QAction * action = modeMenu_->addAction( "Edit coord frame" );
          connect( action, SIGNAL( triggered() ),
                    this, SLOT( slotEditCoordinateFrame() ) );
        }

        // Add widgets through generator
        if ( generatorMap_.contains( node->className() ) ) {
          QWidget* widget = generatorMap_[node->className()]->getWidget( node );
          modeMenu_->addAction( generatorMap_[node->className()]->contextMenuName() , widget, SLOT(show()) );
        } 

        modeMenu_->popup( QCursor::pos() );

        break;
      }
      default: break;
    }
  }
}


//-----------------------------------------------------------------------------


void QtSceneGraphWidget::slotEditMaterial()
{
  if ( curItem_ )
  {
    SceneGraph::MaterialNode * node =
      dynamic_cast< SceneGraph::MaterialNode * >( curItem_->node() );

    QtMaterialDialog* dialog = new QtMaterialDialog( this, node );

    connect(dialog, 
	    SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
	    this, 
	    SLOT(slotNodeChanged(ACG::SceneGraph::BaseNode*)));

    dialog->show();
  }
}

//-----------------------------------------------------------------------------


void QtSceneGraphWidget::slotEditTexture()
{
  if ( curItem_ )
  {
    SceneGraph::TextureNode * node =
      dynamic_cast< SceneGraph::TextureNode * >( curItem_->node() );

    QtTextureDialog* dialog = new QtTextureDialog( this, node );

    connect(dialog, 
        SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
        this, 
        SLOT(slotNodeChanged(ACG::SceneGraph::BaseNode*)));

    dialog->show();
  }
}

//-----------------------------------------------------------------------------


void QtSceneGraphWidget::slotEditShader()
{
  if ( curItem_ )
  {
    SceneGraph::ShaderNode * node =
        dynamic_cast< SceneGraph::ShaderNode * >( curItem_->node() );

    QtShaderDialog* dialog = new QtShaderDialog( this, node );

    connect(dialog, 
            SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
                   this, 
                   SLOT(slotNodeChanged(ACG::SceneGraph::BaseNode*)));

    dialog->show();
  }
}


//-----------------------------------------------------------------------------


void QtSceneGraphWidget::slotEditClipPlanes()
{
  if ( curItem_ )
  {
    SceneGraph::ClippingNode * node =
      dynamic_cast< SceneGraph::ClippingNode * >( curItem_->node() );

    QtClippingDialog * dialog = new QtClippingDialog( this, node );

    connect(dialog, 
	    SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
	    this, 
	    SLOT(slotNodeChanged(ACG::SceneGraph::BaseNode*)));

    dialog->show();
  }
}


//-----------------------------------------------------------------------------


void QtSceneGraphWidget::slotEditCoordinateFrame()
{
  if ( curItem_ )
  {
    SceneGraph::CoordFrameNode * node =
      dynamic_cast< SceneGraph::CoordFrameNode * >( curItem_->node() );

    QtCoordFrameDialog * dialog = new QtCoordFrameDialog( this, node );

    connect(dialog, 
	    SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
 	    this, 
 	    SLOT(slotNodeChanged(ACG::SceneGraph::BaseNode*)));

    dialog->show();
  }
}


//-----------------------------------------------------------------------------


void QtSceneGraphWidget::slotModeMenu( QAction * _action )
{
  SceneGraph::DrawModes::DrawMode new_drawmode( _action->data().toUInt());

  curItem_->node()->drawMode( new_drawmode );
  curItem_->update();
  emit signalNodeChanged( curItem_->node() );
}


//-----------------------------------------------------------------------------


void QtSceneGraphWidget::slotStatusMenu( QAction * _action )
{
  if ( curItem_ )
  {
    unsigned int status = _action->data().toUInt();
    BaseNode * node = curItem_->node();

    node->set_status( (ACG::SceneGraph::BaseNode::StatusMode) status );
    curItem_->update();
    emit signalNodeChanged( node );
  }
}


//-----------------------------------------------------------------------------


void QtSceneGraphWidget::keyPressEvent(QKeyEvent* _event)
{
  switch(_event->key())
  {
    case Qt::Key_Shift:  shiftPressed_ = true; break;
    default : _event->ignore();
  }
}


void QtSceneGraphWidget::keyReleaseEvent(QKeyEvent *_event)
{
  switch(_event->key())
  {
    case Qt::Key_Shift: shiftPressed_ = false; break;
    default : _event->ignore();
  }
}


//-----------------------------------------------------------------------------

void QtSceneGraphWidget::expandAll() {
    
    QTreeWidget::expandAll();
    
    resizeColumnToContents(0);
}

void QtSceneGraphWidget::updateAll() {

    if(rootNode_) {
        update(rootNode_);
        QTreeWidget::expandAll();
    }

    resizeColumnToContents(0);
}

//-----------------------------------------------------------------------------

void 
QtSceneGraphWidget::
slotNodeChanged(ACG::SceneGraph::BaseNode* _node)
{
  emit signalNodeChanged(_node);
}

//-----------------------------------------------------------------------------

bool QtSceneGraphWidget::addWidgetGenerator( SceneGraphWidgetGenerator* _generator )  {
  // Check if we already have a generator for this type.
  if ( generatorMap_.contains( _generator->handles() ) ) {
    std::cerr << "Already handled" << std::endl;
    return false;
  }

  // Store the generator
  generatorMap_[_generator->handles() ] = _generator;

  return true;
}


//=============================================================================




QtSceneGraphWidget::Item::Item( QTreeWidget * _parent,
				SceneGraph::BaseNode* _node )
  : QTreeWidgetItem(_parent), node_(_node)
{ 
  update(); 
}


//-----------------------------------------------------------------------------


QtSceneGraphWidget::Item::Item( Item * _parent,
				SceneGraph::BaseNode* _node )
  : QTreeWidgetItem(_parent), node_(_node)
{ 
  update(); 
}


//-----------------------------------------------------------------------------


void
QtSceneGraphWidget::Item::update()
{
  setText( 0, node_->name().c_str());
  setText( 1, node_->className().c_str());

  switch (node_->status())
  {
    case BaseNode::Active:        setText( 2, "Active");       break;
    case BaseNode::HideNode:      setText( 2, "HideNode");     break;
    case BaseNode::HideChildren:  setText( 2, "HideChildren"); break;
    case BaseNode::HideSubtree:   setText( 2, "HideSubtree");  break;
  }

  setText( 3, node_->drawMode().description().c_str());
}


//-----------------------------------------------------------------------------


QtSceneGraphDialog::
QtSceneGraphDialog( QWidget* _parent,
		    SceneGraph::BaseNode* _rootNode )
  : QDialog(_parent)
{
  setModal( false );
  
  QVBoxLayout* l = new QVBoxLayout( this );
  
  QWidget* buttons = new QWidget(this);
  QHBoxLayout* butLayout = new QHBoxLayout(buttons);
  
  sgw_ = 
    new QtSceneGraphWidget( this, _rootNode );

    // Add buttons to hbox layout
    QPushButton* expAll = new QPushButton("Expand all");
    QPushButton* collAll = new QPushButton("Collapse all");
    QPushButton* updAll = new QPushButton("Update all");
    
    butLayout->addWidget(expAll);
    butLayout->addWidget(collAll);
    butLayout->addWidget(updAll);
    
  l->addWidget(buttons);
  l->addWidget(sgw_);

  connect(sgw_, 
        SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
        this, 
        SLOT(slotNodeChanged(ACG::SceneGraph::BaseNode*)) );
       
  connect(expAll, SIGNAL(pressed()), sgw_, SLOT(expandAll()));
  connect(collAll, SIGNAL(pressed()), sgw_, SLOT(collapseAll()));
  connect(updAll, SIGNAL(pressed()), sgw_, SLOT(updateAll()));
}


//-----------------------------------------------------------------------------


void
QtSceneGraphDialog::
slotNodeChanged(ACG::SceneGraph::BaseNode* _node)
{
  emit(signalNodeChanged(_node));
}


//-----------------------------------------------------------------------------


void
QtSceneGraphDialog::
update(ACG::SceneGraph::BaseNode* _rootNode)
{
  sgw_->update(_rootNode);
}


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
