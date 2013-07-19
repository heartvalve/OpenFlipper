//================================================================
//
/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision: 12588 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-10-13 12:41:04 +0200 (Do, 13 Okt 2011) $                     *
*                                                                            *
\*===========================================================================*/


//================================================================
//
//  CLASS SplatCloudRenderingControlPlugin - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "SplatCloudRenderingControlPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <ObjectTypes/SplatCloud/SplatCloud.hh>

#include <OpenFlipper/common/GlobalOptions.hh>


//== IMPLEMENTATION ==============================================


SplatCloudRenderingControlPlugin::SplatCloudRenderingControlPlugin() :
  toolboxPointsizeScale_  ( 0 ),
  toolboxDefaultColorR_   ( 0 ),
  toolboxDefaultColorG_   ( 0 ),
  toolboxDefaultColorB_   ( 0 ),
  toolboxDefaultNormalX_  ( 0 ),
  toolboxDefaultNormalY_  ( 0 ),
  toolboxDefaultNormalZ_  ( 0 ),
  toolboxDefaultPointsize_( 0 ),
  contextScaleAction_        ( 0 ),
  contextCullingAction_      ( 0 ),
  contextDefaultsAction_     ( 0 ),
  contextReloadShadersAction_( 0 ),
  contextRebuildVBOAction_   ( 0 ),
  contextScaleWidget_              ( 0 ),
  contextScaleWidgetPointsizeScale_( 0 ),
  contextDefaultsWidget_                ( 0 ),
  contextDefaultsWidgetDefaultColorR_   ( 0 ),
  contextDefaultsWidgetDefaultColorG_   ( 0 ),
  contextDefaultsWidgetDefaultColorB_   ( 0 ),
  contextDefaultsWidgetDefaultNormalX_  ( 0 ),
  contextDefaultsWidgetDefaultNormalY_  ( 0 ),
  contextDefaultsWidgetDefaultNormalZ_  ( 0 ),
  contextDefaultsWidgetDefaultPointsize_( 0 )
{
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::initializePlugin()
{
  // --------------------------------
  // -------- create toolbox --------
  // --------------------------------

  // ---- options tab ----

  // pointsize scale
  toolboxPointsizeScale_ = new QDoubleSpinBox();
  toolboxPointsizeScale_->setRange( 0.0, 100.0 );
  toolboxPointsizeScale_->setDecimals( 4 );
  toolboxPointsizeScale_->setSingleStep( 0.1 );
  toolboxPointsizeScale_->setValue( 1.0 );
  toolboxPointsizeScale_->setToolTip( "The scaling factor of the pointsizes used during the rendering of objects" );

  // pointsize scale layout
  QHBoxLayout *toolboxPointsizeScaleLayout = new QHBoxLayout();
  toolboxPointsizeScaleLayout->setSpacing( 6 );
  toolboxPointsizeScaleLayout->addWidget( new QLabel( "Pointsize Scaling Factor:" ) );
  toolboxPointsizeScaleLayout->addWidget( toolboxPointsizeScale_                    );

  // buttons
  QPushButton *toolboxEnableBackfaceCullingButton  = new QPushButton( "Enable Backface Culling"  );
  QPushButton *toolboxDisableBackfaceCullingButton = new QPushButton( "Disable Backface Culling" );
  QPushButton *toolboxReloadShadersButton          = new QPushButton( "Reload Shaders"           );
  QPushButton *toolboxRebuildVBOsButton            = new QPushButton( "Rebuild VBOs"             );
  toolboxEnableBackfaceCullingButton->setToolTip ( "Enable the culling of backfaces"       );
  toolboxDisableBackfaceCullingButton->setToolTip( "Disable the culling of backfaces"      );
  toolboxReloadShadersButton->setToolTip         ( "Reload all shader files"           );
  toolboxRebuildVBOsButton->setToolTip           ( "Rebuild all vertex-buffer-objects" );

  // buttonsA layout
  QHBoxLayout *toolboxButtonsALayout = new QHBoxLayout();
  toolboxButtonsALayout->setSpacing( 6 );
  toolboxButtonsALayout->addWidget( toolboxEnableBackfaceCullingButton  );
  toolboxButtonsALayout->addWidget( toolboxDisableBackfaceCullingButton );

  // separator frame
  QFrame *toolboxSeparatorFrame = new QFrame();
  toolboxSeparatorFrame->setFrameShape( QFrame::HLine );
  toolboxSeparatorFrame->setFrameShadow( QFrame::Sunken );

  // buttonsB layout
  QHBoxLayout *toolboxButtonsBLayout = new QHBoxLayout();
  toolboxButtonsBLayout->setSpacing( 6 );
  toolboxButtonsBLayout->addWidget( toolboxReloadShadersButton );
  toolboxButtonsBLayout->addWidget( toolboxRebuildVBOsButton   );

  // options layout
  QVBoxLayout *toolboxOptionsLayout = new QVBoxLayout();
  toolboxOptionsLayout->setAlignment( Qt::AlignTop );
  toolboxOptionsLayout->setSpacing( 6 );
  toolboxOptionsLayout->addItem  ( toolboxPointsizeScaleLayout );
  toolboxOptionsLayout->addItem  ( toolboxButtonsALayout       );
  toolboxOptionsLayout->addItem  ( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
  toolboxOptionsLayout->addWidget( toolboxSeparatorFrame       );
  toolboxOptionsLayout->addItem  ( toolboxButtonsBLayout       );

  // options widget
  QWidget *toolboxOptionsWidget = new QWidget();
  toolboxOptionsWidget->setLayout( toolboxOptionsLayout );
  toolboxOptionsWidget->setToolTip( "Rendering options" );

  // ---- defaults tab ----

  // default color
  toolboxDefaultColorR_ = new QSpinBox();
  toolboxDefaultColorG_ = new QSpinBox();
  toolboxDefaultColorB_ = new QSpinBox();
  toolboxDefaultColorR_->setRange( 0, 255 );
  toolboxDefaultColorG_->setRange( 0, 255 );
  toolboxDefaultColorB_->setRange( 0, 255 );
  toolboxDefaultColorR_->setSingleStep( 1 );
  toolboxDefaultColorG_->setSingleStep( 1 );
  toolboxDefaultColorB_->setSingleStep( 1 );
  toolboxDefaultColorR_->setValue( 255 );
  toolboxDefaultColorG_->setValue( 255 );
  toolboxDefaultColorB_->setValue( 255 );
  toolboxDefaultColorR_->setToolTip( "The default red-component of the color used when an object has no colors"   );
  toolboxDefaultColorG_->setToolTip( "The default green-component of the color used when an object has no colors" );
  toolboxDefaultColorB_->setToolTip( "The default blue-component of the color used when an object has no colors"  );

  // default normal
  toolboxDefaultNormalX_ = new QDoubleSpinBox();
  toolboxDefaultNormalY_ = new QDoubleSpinBox();
  toolboxDefaultNormalZ_ = new QDoubleSpinBox();
  toolboxDefaultNormalX_->setRange( -1.0, 1.0 );
  toolboxDefaultNormalY_->setRange( -1.0, 1.0 );
  toolboxDefaultNormalZ_->setRange( -1.0, 1.0 );
  toolboxDefaultNormalX_->setDecimals( 2 );
  toolboxDefaultNormalY_->setDecimals( 2 );
  toolboxDefaultNormalZ_->setDecimals( 2 );
  toolboxDefaultNormalX_->setSingleStep( 0.1 );
  toolboxDefaultNormalY_->setSingleStep( 0.1 );
  toolboxDefaultNormalZ_->setSingleStep( 0.1 );
  toolboxDefaultNormalX_->setValue( 0.0 );
  toolboxDefaultNormalY_->setValue( 0.0 );
  toolboxDefaultNormalZ_->setValue( 1.0 );
  toolboxDefaultNormalX_->setToolTip( "The default x-component of the normal used when an object has no normals" );
  toolboxDefaultNormalY_->setToolTip( "The default y-component of the normal used when an object has no normals" );
  toolboxDefaultNormalZ_->setToolTip( "The default z-component of the normal used when an object has no normals" );

  // default pointsize
  toolboxDefaultPointsize_ = new QDoubleSpinBox();
  toolboxDefaultPointsize_->setRange( 0.0, 1000.0 );
  toolboxDefaultPointsize_->setDecimals( 3 );
  toolboxDefaultPointsize_->setSingleStep( 0.01 );
  toolboxDefaultPointsize_->setValue( 0.1 );
  toolboxDefaultPointsize_->setToolTip( "The default pointsize used when an object has no pointsizes" );

  // default color layout
  QHBoxLayout *toolboxDefaultColorLayout = new QHBoxLayout();
  toolboxDefaultColorLayout->setSpacing( 6 );
  toolboxDefaultColorLayout->addWidget( toolboxDefaultColorR_ );
  toolboxDefaultColorLayout->addWidget( toolboxDefaultColorG_ );
  toolboxDefaultColorLayout->addWidget( toolboxDefaultColorB_ );

  // default normal layout
  QHBoxLayout *toolboxDefaultNormalLayout = new QHBoxLayout();
  toolboxDefaultNormalLayout->setSpacing( 6 );
  toolboxDefaultNormalLayout->addWidget( toolboxDefaultNormalX_ );
  toolboxDefaultNormalLayout->addWidget( toolboxDefaultNormalY_ );
  toolboxDefaultNormalLayout->addWidget( toolboxDefaultNormalZ_ );

  // default pointsize layout
  QHBoxLayout *toolboxDefaultPointsizeLayout = new QHBoxLayout();
  toolboxDefaultPointsizeLayout->setSpacing( 6 );
  toolboxDefaultPointsizeLayout->addWidget( toolboxDefaultPointsize_ );

  // default options layout
  QGridLayout *toolboxDefaultsLayout = new QGridLayout();
  toolboxDefaultsLayout->setSpacing( 6 );
  toolboxDefaultsLayout->addWidget( new QLabel( "Default Color:"     ), 0, 0 );
  toolboxDefaultsLayout->addWidget( new QLabel( "Default Normal:"    ), 1, 0 );
  toolboxDefaultsLayout->addWidget( new QLabel( "Default Pointsize:" ), 2, 0 );
  toolboxDefaultsLayout->addItem  ( toolboxDefaultColorLayout,          0, 1 );
  toolboxDefaultsLayout->addItem  ( toolboxDefaultNormalLayout,         1, 1 );
  toolboxDefaultsLayout->addItem  ( toolboxDefaultPointsizeLayout,      2, 1 );

  // defaults widget
  QWidget *toolboxDefaultsWidget = new QWidget();
  toolboxDefaultsWidget->setLayout( toolboxDefaultsLayout );
  toolboxDefaultsWidget->setToolTip( "Default values" );

  // ----

  // tab widget
  QTabWidget *toolboxTabWidget = new QTabWidget();
  toolboxTabWidget->addTab( toolboxOptionsWidget,  "Options"  );
  toolboxTabWidget->addTab( toolboxDefaultsWidget, "Defaults" );

  // toolbox layout
  QVBoxLayout *toolboxLayout = new QVBoxLayout();
  toolboxLayout->setSpacing( 6 );
  toolboxLayout->addWidget( toolboxTabWidget );

  // toolbox
  QWidget *toolbox = new QWidget();
  toolbox->setLayout( toolboxLayout );

  // toolbox icon
  QIcon *toolboxIcon = new QIcon( OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "SplatCloudRenderingControl.png" );

  // ----

  // connect events to slots
  connect( toolboxPointsizeScale_,              SIGNAL( valueChanged(double) ), this, SLOT( slotToolboxPointsizeScaleValueChanged()          ) );
  connect( toolboxDefaultColorR_,               SIGNAL( valueChanged(int)    ), this, SLOT( slotToolboxDefaultColorValueChanged()            ) );
  connect( toolboxDefaultColorG_,               SIGNAL( valueChanged(int)    ), this, SLOT( slotToolboxDefaultColorValueChanged()            ) );
  connect( toolboxDefaultColorB_,               SIGNAL( valueChanged(int)    ), this, SLOT( slotToolboxDefaultColorValueChanged()            ) );
  connect( toolboxDefaultNormalX_,              SIGNAL( valueChanged(double) ), this, SLOT( slotToolboxDefaultNormalValueChanged()           ) );
  connect( toolboxDefaultNormalY_,              SIGNAL( valueChanged(double) ), this, SLOT( slotToolboxDefaultNormalValueChanged()           ) );
  connect( toolboxDefaultNormalZ_,              SIGNAL( valueChanged(double) ), this, SLOT( slotToolboxDefaultNormalValueChanged()           ) );
  connect( toolboxDefaultPointsize_,            SIGNAL( valueChanged(double) ), this, SLOT( slotToolboxDefaultPointsizeValueChanged()        ) );

  connect( toolboxEnableBackfaceCullingButton,  SIGNAL( clicked()            ), this, SLOT( slotToolboxEnableBackfaceCullingButtonClicked()  ) );
  connect( toolboxDisableBackfaceCullingButton, SIGNAL( clicked()            ), this, SLOT( slotToolboxDisableBackfaceCullingButtonClicked() ) );
  connect( toolboxReloadShadersButton,          SIGNAL( clicked()            ), this, SLOT( slotToolboxReloadShadersButtonClicked()          ) );
  connect( toolboxRebuildVBOsButton,            SIGNAL( clicked()            ), this, SLOT( slotToolboxRebuildVBOsButtonClicked()            ) );

  // emit signal to add the new toolbox
  emit addToolbox( tr("SplatCloud Rendering Control") , toolbox , toolboxIcon );

  // -------------------------------------
  // -------- create context menu --------
  // -------------------------------------

  // actions
  contextScaleAction_         = new QAction( tr( "Pointsize Scaling Factor..."), this );
  contextCullingAction_       = new QAction( tr( "Backface Culling"           ), this );
  contextDefaultsAction_      = new QAction( tr( "Defaults..."                ), this );
  contextReloadShadersAction_ = new QAction( tr( "Reload Shaders"             ), this );
  contextRebuildVBOAction_    = new QAction( tr( "Rebuild VBO"                ), this );
  contextCullingAction_->setCheckable( true );

  // context menu
  QMenu *contextMenu = new QMenu( "SplatCloud Rendering Control" );
  contextMenu->setIcon( QIcon( OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "SplatCloudRenderingControl.png" ) );
  contextMenu->addAction( contextScaleAction_         );
  contextMenu->addAction( contextCullingAction_       );
  contextMenu->addSeparator();
  contextMenu->addAction( contextDefaultsAction_      );
  contextMenu->addSeparator();
  contextMenu->addAction( contextReloadShadersAction_ );
  contextMenu->addAction( contextRebuildVBOAction_    );

  // ----

  // connect events to slots
  connect( contextScaleAction_,         SIGNAL( triggered() ), this, SLOT( slotContextScaleActionTriggered()         ) );
  connect( contextCullingAction_,       SIGNAL( triggered() ), this, SLOT( slotContextCullingActionTriggered()       ) );
  connect( contextDefaultsAction_,      SIGNAL( triggered() ), this, SLOT( slotContextDefaultsActionTriggered()      ) );
  connect( contextReloadShadersAction_, SIGNAL( triggered() ), this, SLOT( slotContextReloadShadersActionTriggered() ) );
  connect( contextRebuildVBOAction_,    SIGNAL( triggered() ), this, SLOT( slotContextRebuildVBOActionTriggered()    ) );

  // emit signal to add the new context menu
  emit addContextMenuItem( contextMenu->menuAction(), DATA_SPLATCLOUD, CONTEXTOBJECTMENU );

  // --------------------------------------------------
  // -------- create context menu scale widget --------
  // --------------------------------------------------

  // pointsize scale
  contextScaleWidgetPointsizeScale_ = new QDoubleSpinBox();
  contextScaleWidgetPointsizeScale_->setRange( 0.0, 100.0 );
  contextScaleWidgetPointsizeScale_->setDecimals( 4 );
  contextScaleWidgetPointsizeScale_->setSingleStep( 0.1 );
  contextScaleWidgetPointsizeScale_->setToolTip( "The scaling factor of the pointsizes used during the rendering of the picked object" );

  // pointsize scale layout
  QHBoxLayout *contextScaleWidgetPointsizeScaleLayout = new QHBoxLayout();
  contextScaleWidgetPointsizeScaleLayout->setSpacing( 6 );
  contextScaleWidgetPointsizeScaleLayout->addWidget( new QLabel( "Pointsize Scaling Factor:" ) );
  contextScaleWidgetPointsizeScaleLayout->addWidget( contextScaleWidgetPointsizeScale_         );

  // widget layout
  QVBoxLayout *contextScaleWidgetLayout = new QVBoxLayout();
  contextScaleWidgetLayout->setAlignment( Qt::AlignTop );
  contextScaleWidgetLayout->setSpacing( 6 );
  contextScaleWidgetLayout->addItem( contextScaleWidgetPointsizeScaleLayout );

  // widget
  contextScaleWidget_ = new QWidget();
  contextScaleWidget_->setWindowTitle( "Pointsize Scaling Factor" );
  contextScaleWidget_->setLayout( contextScaleWidgetLayout );

  // ----

  // connect events to slots
  connect( contextScaleWidgetPointsizeScale_, SIGNAL( valueChanged(double) ), this, SLOT( slotContextScaleWidgetPointsizeScaleValueChanged() ) );

  // -----------------------------------------------------
  // -------- create context menu defaults widget --------
  // -----------------------------------------------------

  // default color
  contextDefaultsWidgetDefaultColorR_ = new QSpinBox();
  contextDefaultsWidgetDefaultColorG_ = new QSpinBox();
  contextDefaultsWidgetDefaultColorB_ = new QSpinBox();
  contextDefaultsWidgetDefaultColorR_->setRange( 0, 255 );
  contextDefaultsWidgetDefaultColorG_->setRange( 0, 255 );
  contextDefaultsWidgetDefaultColorB_->setRange( 0, 255 );
  contextDefaultsWidgetDefaultColorR_->setSingleStep( 1 );
  contextDefaultsWidgetDefaultColorG_->setSingleStep( 1 );
  contextDefaultsWidgetDefaultColorB_->setSingleStep( 1 );
  contextDefaultsWidgetDefaultColorR_->setToolTip( "The default red-component of the color used when the picked object has no colors"   );
  contextDefaultsWidgetDefaultColorG_->setToolTip( "The default green-component of the color used when the picked object has no colors" );
  contextDefaultsWidgetDefaultColorB_->setToolTip( "The default blue-component of the color used when the picked object has no colors"  );

  // default normal
  contextDefaultsWidgetDefaultNormalX_ = new QDoubleSpinBox();
  contextDefaultsWidgetDefaultNormalY_ = new QDoubleSpinBox();
  contextDefaultsWidgetDefaultNormalZ_ = new QDoubleSpinBox();
  contextDefaultsWidgetDefaultNormalX_->setRange( -1.0, 1.0 );
  contextDefaultsWidgetDefaultNormalY_->setRange( -1.0, 1.0 );
  contextDefaultsWidgetDefaultNormalZ_->setRange( -1.0, 1.0 );
  contextDefaultsWidgetDefaultNormalX_->setDecimals( 2 );
  contextDefaultsWidgetDefaultNormalY_->setDecimals( 2 );
  contextDefaultsWidgetDefaultNormalZ_->setDecimals( 2 );
  contextDefaultsWidgetDefaultNormalX_->setSingleStep( 0.1 );
  contextDefaultsWidgetDefaultNormalY_->setSingleStep( 0.1 );
  contextDefaultsWidgetDefaultNormalZ_->setSingleStep( 0.1 );
  contextDefaultsWidgetDefaultNormalX_->setToolTip( "The default x-component of the normal used when the picked object has no normals" );
  contextDefaultsWidgetDefaultNormalY_->setToolTip( "The default y-component of the normal used when the picked object has no normals" );
  contextDefaultsWidgetDefaultNormalZ_->setToolTip( "The default z-component of the normal used when the picked object has no normals" );

  // default pointsize
  contextDefaultsWidgetDefaultPointsize_ = new QDoubleSpinBox();
  contextDefaultsWidgetDefaultPointsize_->setRange( 0.0, 1000.0 );
  contextDefaultsWidgetDefaultPointsize_->setDecimals( 3 );
  contextDefaultsWidgetDefaultPointsize_->setSingleStep( 0.01 );
  contextDefaultsWidgetDefaultPointsize_->setToolTip( "The default pointsize used when the picked object has no pointsizes" );

  // default color layout
  QHBoxLayout *contextDefaultsWidgetDefaultColorLayout = new QHBoxLayout();
  contextDefaultsWidgetDefaultColorLayout->setSpacing( 6 );
  contextDefaultsWidgetDefaultColorLayout->addWidget( contextDefaultsWidgetDefaultColorR_ );
  contextDefaultsWidgetDefaultColorLayout->addWidget( contextDefaultsWidgetDefaultColorG_ );
  contextDefaultsWidgetDefaultColorLayout->addWidget( contextDefaultsWidgetDefaultColorB_ );

  // default normal layout
  QHBoxLayout *contextDefaultsWidgetDefaultNormalLayout = new QHBoxLayout();
  contextDefaultsWidgetDefaultNormalLayout->setSpacing( 6 );
  contextDefaultsWidgetDefaultNormalLayout->addWidget( contextDefaultsWidgetDefaultNormalX_ );
  contextDefaultsWidgetDefaultNormalLayout->addWidget( contextDefaultsWidgetDefaultNormalY_ );
  contextDefaultsWidgetDefaultNormalLayout->addWidget( contextDefaultsWidgetDefaultNormalZ_ );

  // default pointsize layout
  QHBoxLayout *contextDefaultsWidgetDefaultPointsizeLayout = new QHBoxLayout();
  contextDefaultsWidgetDefaultPointsizeLayout->setSpacing( 6 );
  contextDefaultsWidgetDefaultPointsizeLayout->addWidget( contextDefaultsWidgetDefaultPointsize_ );

  // default options layout
  QGridLayout *contextDefaultsWidgetDefaultOptionsLayout = new QGridLayout();
  contextDefaultsWidgetDefaultOptionsLayout->setSpacing( 6 );
  contextDefaultsWidgetDefaultOptionsLayout->addWidget( new QLabel( "Default Color:"     ),          0, 0 );
  contextDefaultsWidgetDefaultOptionsLayout->addWidget( new QLabel( "Default Normal:"    ),          1, 0 );
  contextDefaultsWidgetDefaultOptionsLayout->addWidget( new QLabel( "Default Pointsize:" ),          2, 0 );
  contextDefaultsWidgetDefaultOptionsLayout->addItem  ( contextDefaultsWidgetDefaultColorLayout,     0, 1 );
  contextDefaultsWidgetDefaultOptionsLayout->addItem  ( contextDefaultsWidgetDefaultNormalLayout,    1, 1 );
  contextDefaultsWidgetDefaultOptionsLayout->addItem  ( contextDefaultsWidgetDefaultPointsizeLayout, 2, 1 );

  // widget layout
  QVBoxLayout *contextDefaultsWidgetLayout = new QVBoxLayout();
  contextDefaultsWidgetLayout->setAlignment( Qt::AlignTop );
  contextDefaultsWidgetLayout->setSpacing( 6 );
  contextDefaultsWidgetLayout->addItem( contextDefaultsWidgetDefaultOptionsLayout );

  // widget
  contextDefaultsWidget_ = new QWidget();
  contextDefaultsWidget_->setWindowTitle( "Defaults" );
  contextDefaultsWidget_->setLayout( contextDefaultsWidgetLayout );

  // ----

  // connect events to slots
  connect( contextDefaultsWidgetDefaultColorR_,    SIGNAL( valueChanged(int)    ), this, SLOT( slotContextDefaultsWidgetDefaultColorValueChanged()     ) );
  connect( contextDefaultsWidgetDefaultColorG_,    SIGNAL( valueChanged(int)    ), this, SLOT( slotContextDefaultsWidgetDefaultColorValueChanged()     ) );
  connect( contextDefaultsWidgetDefaultColorB_,    SIGNAL( valueChanged(int)    ), this, SLOT( slotContextDefaultsWidgetDefaultColorValueChanged()     ) );
  connect( contextDefaultsWidgetDefaultNormalX_,   SIGNAL( valueChanged(double) ), this, SLOT( slotContextDefaultsWidgetDefaultNormalValueChanged()    ) );
  connect( contextDefaultsWidgetDefaultNormalY_,   SIGNAL( valueChanged(double) ), this, SLOT( slotContextDefaultsWidgetDefaultNormalValueChanged()    ) );
  connect( contextDefaultsWidgetDefaultNormalZ_,   SIGNAL( valueChanged(double) ), this, SLOT( slotContextDefaultsWidgetDefaultNormalValueChanged()    ) );
  connect( contextDefaultsWidgetDefaultPointsize_, SIGNAL( valueChanged(double) ), this, SLOT( slotContextDefaultsWidgetDefaultPointsizeValueChanged() ) );

  // ----------------------------------
  // -------- create view mode --------
  // ----------------------------------

  // toolboxes
  QStringList viewModeToolboxes;
  viewModeToolboxes += "Data Control";
  viewModeToolboxes += "Selections";
  viewModeToolboxes += "SplatCloud Estimator";
  viewModeToolboxes += "SplatCloud Rendering Control";

  // toolbars
  QStringList viewModeToolbars;
  viewModeToolbars += "Main Toolbar";
  viewModeToolbars += "Viewer Toolbar";
  viewModeToolbars += "Color Toolbar";
  viewModeToolbars += "Viewing Directions";
  viewModeToolbars += "Selection Base";
  viewModeToolbars += "Backup Toolbar";

  // context menus
  QStringList viewModeContextMenus;
  viewModeContextMenus += "DataControl->Material Properties";
  viewModeContextMenus += "DataControl->Object";
  viewModeContextMenus += "SplatCloudEstimatorPlugin->SplatCloud Estimator";
  viewModeContextMenus += "SplatCloudRenderingControlPlugin->SplatCloud Rendering Control";
  viewModeContextMenus += "ViewControl->Visualization";
  viewModeContextMenus += "Backup->&Redo";
  viewModeContextMenus += "Backup->&Undo";

  // emit signals to add new viewmode
  emit defineViewModeToolboxes   ( "SplatCloud", viewModeToolboxes    );
  emit defineViewModeToolbars    ( "SplatCloud", viewModeToolbars     );
  emit defineViewModeContextMenus( "SplatCloud", viewModeContextMenus );
  emit defineViewModeIcon        ( "SplatCloud", "SplatCloudType.png" );
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::addedEmptyObject( int _objectId )
{
  if( _objectId == -1 )
    return;

  // get object by id
  BaseObjectData *object;
  if( !PluginFunctions::getObject( _objectId, object ) )
    return;

  // get splatcloud-object
  SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( object );

  // if object is a SplatCloud...
  if( splatCloudObject )
  {
    // set current rendering options
    splatCloudObject->enableBackfaceCulling( false                           );
    splatCloudObject->setPointsizeScale    ( toolboxPointsizeScale_->value() );

    // set default values
    SplatCloudNode *splatCloudNode = splatCloudObject->splatCloudNode();
    if( splatCloudNode )
    {
      splatCloudNode->setDefaultColor    ( SplatCloudNode::Color    ( toolboxDefaultColorR_->   value(), 
                                                                      toolboxDefaultColorG_->   value(), 
                                                                      toolboxDefaultColorB_->   value() ) );
      splatCloudNode->setDefaultNormal   ( SplatCloudNode::Normal   ( toolboxDefaultNormalX_->  value(), 
                                                                      toolboxDefaultNormalY_->  value(), 
                                                                      toolboxDefaultNormalZ_->  value() ) );
      splatCloudNode->setDefaultPointsize( SplatCloudNode::Pointsize( toolboxDefaultPointsize_->value() ) );
    }
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotUpdateContextMenu( int _objectId )
{
  if( _objectId == -1 )
    return;

  // get object by id
  BaseObjectData *object;
  if( !PluginFunctions::getObject( _objectId, object ) )
    return;

  // get splatcloud-object
  SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( object );

  // if object is a SplatCloud...
  if( splatCloudObject )
  {
    // update context menu value
    contextCullingAction_->setChecked( splatCloudObject->isBackfaceCullingEnabled() );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotToolboxPointsizeScaleValueChanged()
{
  // get toolbox option value
  double scale = toolboxPointsizeScale_->value();

  // for all splatcloud-objects...
  PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
  for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
  {
    // get splatcloud-object
    SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( *objIter );

    // apply update
    splatCloudObject->setPointsizeScale( scale );

    // emit signal that the object has to be updated
    emit updatedObject( objIter->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotToolboxDefaultColorValueChanged()
{
  // for all splatcloud-objects...
  PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
  for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
  {
    // get scenegraph splatcloud-node
    SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( *objIter );

    // apply update (set default color)
    splatCloudNode->setDefaultColor( SplatCloudNode::Color( toolboxDefaultColorR_->value(), 
                                                            toolboxDefaultColorG_->value(), 
                                                            toolboxDefaultColorB_->value() ) );

    // emit signal that object has to be updated
    emit updatedObject( objIter->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotToolboxDefaultNormalValueChanged()
{
  // for all splatcloud-objects...
  PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
  for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
  {
    // get scenegraph splatcloud-node
    SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( *objIter );

    // apply update (set default normal)
    splatCloudNode->setDefaultNormal( SplatCloudNode::Normal( toolboxDefaultNormalX_->value(), 
                                                              toolboxDefaultNormalY_->value(), 
                                                              toolboxDefaultNormalZ_->value() ) );

    // emit signal that object has to be updated
    emit updatedObject( objIter->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotToolboxDefaultPointsizeValueChanged()
{
  // for all splatcloud-objects...
  PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
  for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
  {
    // get scenegraph splatcloud-node
    SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( *objIter );

    // apply update (set default pointsize)
    splatCloudNode->setDefaultPointsize( SplatCloudNode::Pointsize( toolboxDefaultPointsize_->value() ) );

    // emit signal that object has to be updated
    emit updatedObject( objIter->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotToolboxEnableBackfaceCullingButtonClicked()
{
  // for all splatcloud-objects...
  PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
  for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
  {
    // get splatcloud-object
    SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( *objIter );

    // apply update
    splatCloudObject->enableBackfaceCulling( true );

    // emit signal that the object has to be updated
    emit updatedObject( objIter->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotToolboxDisableBackfaceCullingButtonClicked()
{
  // for all splatcloud-objects...
  PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
  for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
  {
    // get splatcloud-object
    SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( *objIter );

    // apply update
    splatCloudObject->enableBackfaceCulling( false );

    // emit signal that the object has to be updated
    emit updatedObject( objIter->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotToolboxReloadShadersButtonClicked()
{
  // for all splatcloud-objects...
  PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
  for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
  {
    // get scenegraph splatcloud-object
    SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( *objIter );

    // apply update (reload standard and picking shaders and re-set uniforms)
    splatCloudObject->reloadShaders();
    splatCloudObject->setPointsizeScale    ( splatCloudObject->pointsizeScale()           );
    splatCloudObject->enableBackfaceCulling( splatCloudObject->isBackfaceCullingEnabled() );

    // emit signal that object has to be updated
    emit updatedObject( objIter->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotToolboxRebuildVBOsButtonClicked()
{
  // for all splatcloud-objects...
  PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
  for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
  {
    // get scenegraph splatcloud-node
    SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( *objIter );

    // rebuild the vertex-buffer-object the next time the node is drawn (or picked)
    splatCloudNode->modifiedAll();

    // emit signal that object has to be updated
    emit updatedObject( objIter->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotContextScaleActionTriggered()
{
  // get picked object by id
  int objectId = contextScaleAction_->data().toInt();
  if( objectId == -1 )
    return;
  //
  BaseObjectData *object;
  if( ! PluginFunctions::getObject( objectId, object ) )
    return;

  // get splatcloud-object
  SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( object );

  // if object is a SplatCloud...
  if( splatCloudObject )
  {
    // get value from SplatCloud
    double ps = splatCloudObject->pointsizeScale();

    // update widget option value
    contextScaleWidgetPointsizeScale_->setValue( ps );

    // move widget to position of context menu entry
    contextScaleWidget_->move( contextScaleAction_->associatedWidgets()[0]->mapToGlobal( QPoint() ) );

    // show widget
    contextScaleWidget_->show();
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotContextScaleWidgetPointsizeScaleValueChanged()
{
  // get picked object by id
  int objectId = contextScaleAction_->data().toInt();
  if( objectId == -1 )
    return;
  //
  BaseObjectData *object;
  if( ! PluginFunctions::getObject( objectId, object ) )
    return;

  // get splatcloud-object
  SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( object );

  // if object is a SplatCloud...
  if( splatCloudObject )
  {
    // get widget option value
    double scale = contextScaleWidgetPointsizeScale_->value();

    // apply update (set pointsize scale)
    splatCloudObject->setPointsizeScale( scale );

    // emit signal that object has to be updated
    emit updatedObject( object->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotContextDefaultsActionTriggered()
{
  // get picked object by id
  int objectId = contextDefaultsAction_->data().toInt();
  if( objectId == -1 )
    return;
  //
  BaseObjectData *object;
  if( ! PluginFunctions::getObject( objectId, object ) )
    return;

  // get splatcloud-node
  SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( object );

  // if object is a SplatCloud...
  if( splatCloudNode )
  {
    // get values from SplatCloud
    int    cr = splatCloudNode->defaultColor()[0];
    int    cg = splatCloudNode->defaultColor()[1];
    int    cb = splatCloudNode->defaultColor()[2];
    double nx = splatCloudNode->defaultNormal()[0];
    double ny = splatCloudNode->defaultNormal()[1];
    double nz = splatCloudNode->defaultNormal()[2];
    double ps = splatCloudNode->defaultPointsize();

    // update widget option values
    contextDefaultsWidgetDefaultColorR_->   setValue( cr );
    contextDefaultsWidgetDefaultColorG_->   setValue( cg );
    contextDefaultsWidgetDefaultColorB_->   setValue( cb );
    contextDefaultsWidgetDefaultNormalX_->  setValue( nx );
    contextDefaultsWidgetDefaultNormalY_->  setValue( ny );
    contextDefaultsWidgetDefaultNormalZ_->  setValue( nz );
    contextDefaultsWidgetDefaultPointsize_->setValue( ps );

    // move defaults widget to position of context menu entry
    contextDefaultsWidget_->move( contextDefaultsAction_->associatedWidgets()[0]->mapToGlobal( QPoint() ) );

    // show widget
    contextDefaultsWidget_->show();
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotContextDefaultsWidgetDefaultColorValueChanged()
{
  // get picked object by id
  int objectId = contextDefaultsAction_->data().toInt();
  if( objectId == -1 )
    return;
  //
  BaseObjectData *object;
  if( ! PluginFunctions::getObject( objectId, object ) )
    return;

  // get splatcloud-node
  SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( object );

  // if object is a SplatCloud...
  if( splatCloudNode )
  {
    // apply update (set default color)
    splatCloudNode->setDefaultColor( SplatCloudNode::Color( contextDefaultsWidgetDefaultColorR_->value(),
                                                            contextDefaultsWidgetDefaultColorG_->value(),
                                                            contextDefaultsWidgetDefaultColorB_->value() ) );

    // emit signal that the object has to be updated
    emit updatedObject( object->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotContextDefaultsWidgetDefaultNormalValueChanged()
{
  // get picked object by id
  int objectId = contextDefaultsAction_->data().toInt();
  if( objectId == -1 )
    return;
  //
  BaseObjectData *object;
  if( ! PluginFunctions::getObject( objectId, object ) )
    return;

  // get splatcloud-node
  SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( object );

  // if object is a SplatCloud...
  if( splatCloudNode )
  {
    // apply update (set default normal)
    splatCloudNode->setDefaultNormal( SplatCloudNode::Normal( contextDefaultsWidgetDefaultNormalX_->value(),
                                                              contextDefaultsWidgetDefaultNormalY_->value(),
                                                              contextDefaultsWidgetDefaultNormalZ_->value() ) );

    // emit signal that the object has to be updated
    emit updatedObject( object->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotContextDefaultsWidgetDefaultPointsizeValueChanged()
{
  // get picked object by id
  int objectId = contextDefaultsAction_->data().toInt();
  if( objectId == -1 )
    return;
  //
  BaseObjectData *object;
  if( ! PluginFunctions::getObject( objectId, object ) )
    return;

  // get splatcloud-node
  SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( object );

  // if object is a SplatCloud...
  if( splatCloudNode )
  {
    // apply update (set default pointsize)
    splatCloudNode->setDefaultPointsize( SplatCloudNode::Pointsize( contextDefaultsWidgetDefaultPointsize_->value() ) );

    // emit signal that the object has to be updated
    emit updatedObject( object->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotContextCullingActionTriggered()
{
  // get picked object by id
  int objectId = contextCullingAction_->data().toInt();
  if( objectId == -1 )
    return;
  //
  BaseObjectData *object;
  if( ! PluginFunctions::getObject( objectId, object ) )
    return;

  // get splatcloud-object
  SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( object );

  // if object is a SplatCloud...
  if( splatCloudObject )
  {
    // get context menu option value
    bool enable = contextCullingAction_->isChecked();

    // apply update
    splatCloudObject->enableBackfaceCulling( enable );

    // emit signal that the object has to be updated
    emit updatedObject( object->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotContextReloadShadersActionTriggered()
{
  // get picked object by id
  int objectId = contextReloadShadersAction_->data().toInt();
  if( objectId == -1 )
    return;
  //
  BaseObjectData *object;
  if( ! PluginFunctions::getObject( objectId, object ) )
    return;

  // get splatcloud-object
  SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( object );

  // if object is a SplatCloud...
  if( splatCloudObject )
  {
    // apply update (reload standard and picking shaders and re-set uniforms)
    splatCloudObject->reloadShaders();
    splatCloudObject->setPointsizeScale    ( splatCloudObject->pointsizeScale()           );
    splatCloudObject->enableBackfaceCulling( splatCloudObject->isBackfaceCullingEnabled() );

    // emit signal that object has to be updated
    emit updatedObject( object->id(), UPDATE_ALL );
  }
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotContextRebuildVBOActionTriggered()
{
  // get picked object by id
  int objectId = contextRebuildVBOAction_->data().toInt();
  if( objectId == -1 )
    return;
  //
  BaseObjectData *object;
  if( ! PluginFunctions::getObject( objectId, object ) )
    return;

  // get splatcloud-node
  SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( object );

  // if object is a SplatCloud...
  if( splatCloudNode )
  {
    // rebuild the vertex-buffer-object the next time the node is drawn (or picked)
        splatCloudNode->modifiedAll();

    // emit signal that object has to be updated
    emit updatedObject( object->id(), UPDATE_ALL );
  }
}


//================================================================


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( splatcloudrenderingcontrolplugin, SplatCloudRenderingControlPlugin );
#endif


