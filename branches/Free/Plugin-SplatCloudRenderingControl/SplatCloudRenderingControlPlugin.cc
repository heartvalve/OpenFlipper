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
	toolboxOptionsLayout->setSpacing( 6 );
	toolboxOptionsLayout->addItem  ( toolboxPointsizeScaleLayout );
	toolboxOptionsLayout->addItem  ( toolboxButtonsALayout       );
	toolboxOptionsLayout->addWidget( toolboxSeparatorFrame       );
	toolboxOptionsLayout->addItem  ( toolboxButtonsBLayout       );

	// options widget
	QWidget *toolboxOptionsWidget = new QWidget();
	toolboxOptionsWidget->setLayout( toolboxOptionsLayout );
	toolboxOptionsWidget->setToolTip( "Rendering options" );

	// ---- defaults tab ----

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
	toolboxDefaultPointsize_->setDecimals( 4 );
	toolboxDefaultPointsize_->setSingleStep( 0.001 );
	toolboxDefaultPointsize_->setValue( 0.01 );
	toolboxDefaultPointsize_->setToolTip( "The default pointsize used when an object has no pointsizes" );

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

	// default color layout
	QHBoxLayout *toolboxDefaultColorLayout = new QHBoxLayout();
	toolboxDefaultColorLayout->setSpacing( 6 );
	toolboxDefaultColorLayout->addWidget( toolboxDefaultColorR_ );
	toolboxDefaultColorLayout->addWidget( toolboxDefaultColorG_ );
	toolboxDefaultColorLayout->addWidget( toolboxDefaultColorB_ );

	// default options layout
	QGridLayout *toolboxDefaultOptionsLayout = new QGridLayout();
	toolboxDefaultOptionsLayout->setSpacing( 6 );
	toolboxDefaultOptionsLayout->addWidget( new QLabel( "Default Normal:"    ), 0, 0 );
	toolboxDefaultOptionsLayout->addWidget( new QLabel( "Default Pointsize:" ), 1, 0 );
	toolboxDefaultOptionsLayout->addWidget( new QLabel( "Default Color:"     ), 2, 0 );
	toolboxDefaultOptionsLayout->addItem  ( toolboxDefaultNormalLayout,         0, 1 );
	toolboxDefaultOptionsLayout->addItem  ( toolboxDefaultPointsizeLayout,      1, 1 );
	toolboxDefaultOptionsLayout->addItem  ( toolboxDefaultColorLayout,          2, 1 );

	// apply defaults button
	QPushButton *toolboxApplyDefaultsButton = new QPushButton( "Apply Defaults" );
	toolboxApplyDefaultsButton->setToolTip( "Apply the changes made" );

	// defaults layout
	QVBoxLayout *toolboxDefaultsLayout = new QVBoxLayout();
	toolboxDefaultsLayout->setSpacing( 6 );
	toolboxDefaultsLayout->addItem  ( toolboxDefaultOptionsLayout );
	toolboxDefaultsLayout->addWidget( toolboxApplyDefaultsButton  );

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
	QIcon *toolboxIcon = new QIcon( OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "SplatCloudType.png" );

	// ----

	// connect events to slots
	connect( toolboxPointsizeScale_,              SIGNAL( valueChanged(double) ), this, SLOT( slotToolboxPointsizeScaleValueChanged()          ) );
	connect( toolboxEnableBackfaceCullingButton,  SIGNAL( clicked()            ), this, SLOT( slotToolboxEnableBackfaceCullingButtonClicked()  ) );
	connect( toolboxDisableBackfaceCullingButton, SIGNAL( clicked()            ), this, SLOT( slotToolboxDisableBackfaceCullingButtonClicked() ) );
	connect( toolboxReloadShadersButton,          SIGNAL( clicked()            ), this, SLOT( slotToolboxReloadShadersButtonClicked()          ) );
	connect( toolboxRebuildVBOsButton,            SIGNAL( clicked()            ), this, SLOT( slotToolboxRebuildVBOsButtonClicked()            ) );
	connect( toolboxApplyDefaultsButton,          SIGNAL( clicked()            ), this, SLOT( slotToolboxApplyDefaultsButtonClicked()          ) );

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

	// -------------------------------------
	// -------- create scale widget --------
	// -------------------------------------

	// pointsize scale
	scaleWidgetPointsizeScale_ = new QDoubleSpinBox();
	scaleWidgetPointsizeScale_->setRange( 0.0, 100.0 );
	scaleWidgetPointsizeScale_->setDecimals( 4 );
	scaleWidgetPointsizeScale_->setSingleStep( 0.1 );
	scaleWidgetPointsizeScale_->setToolTip( "The scaling factor of the pointsizes used during the rendering of the picked object" );

	// pointsize scale layout
	QHBoxLayout *scaleWidgetPointsizeScaleLayout = new QHBoxLayout();
	scaleWidgetPointsizeScaleLayout->setSpacing( 6 );
	scaleWidgetPointsizeScaleLayout->addWidget( new QLabel( "Pointsize Scaling Factor:" ) );
	scaleWidgetPointsizeScaleLayout->addWidget( scaleWidgetPointsizeScale_                );

	// widget layout
	QVBoxLayout *scaleWidgetLayout = new QVBoxLayout();
	scaleWidgetLayout->setSpacing( 6 );
	scaleWidgetLayout->addItem( scaleWidgetPointsizeScaleLayout );

	// widget
	scaleWidget_ = new QWidget();
	scaleWidget_->setWindowTitle( "Pointsize Scaling Factor" );
	scaleWidget_->setLayout( scaleWidgetLayout );

	// ----

	// connect events to slots
	connect( scaleWidgetPointsizeScale_, SIGNAL( valueChanged(double) ), this, SLOT( slotScaleWidgetPointsizeScaleValueChanged() ) );

	// ----------------------------------------
	// -------- create defaults widget --------
	// ----------------------------------------

	// default normal
	defaultsWidgetDefaultNormalX_ = new QDoubleSpinBox();
	defaultsWidgetDefaultNormalY_ = new QDoubleSpinBox();
	defaultsWidgetDefaultNormalZ_ = new QDoubleSpinBox();
	defaultsWidgetDefaultNormalX_->setRange( -1.0, 1.0 );
	defaultsWidgetDefaultNormalY_->setRange( -1.0, 1.0 );
	defaultsWidgetDefaultNormalZ_->setRange( -1.0, 1.0 );
	defaultsWidgetDefaultNormalX_->setDecimals( 2 );
	defaultsWidgetDefaultNormalY_->setDecimals( 2 );
	defaultsWidgetDefaultNormalZ_->setDecimals( 2 );
	defaultsWidgetDefaultNormalX_->setSingleStep( 0.1 );
	defaultsWidgetDefaultNormalY_->setSingleStep( 0.1 );
	defaultsWidgetDefaultNormalZ_->setSingleStep( 0.1 );
	defaultsWidgetDefaultNormalX_->setToolTip( "The default x-component of the normal used when the picked object has no normals" );
	defaultsWidgetDefaultNormalY_->setToolTip( "The default y-component of the normal used when the picked object has no normals" );
	defaultsWidgetDefaultNormalZ_->setToolTip( "The default z-component of the normal used when the picked object has no normals" );

	// default pointsize
	defaultsWidgetDefaultPointsize_ = new QDoubleSpinBox();
	defaultsWidgetDefaultPointsize_->setRange( 0.0, 1000.0 );
	defaultsWidgetDefaultPointsize_->setDecimals( 4 );
	defaultsWidgetDefaultPointsize_->setSingleStep( 0.001 );
	defaultsWidgetDefaultPointsize_->setToolTip( "The default pointsize used when the picked object has no pointsizes" );

	// default color
	defaultsWidgetDefaultColorR_ = new QSpinBox();
	defaultsWidgetDefaultColorG_ = new QSpinBox();
	defaultsWidgetDefaultColorB_ = new QSpinBox();
	defaultsWidgetDefaultColorR_->setRange( 0, 255 );
	defaultsWidgetDefaultColorG_->setRange( 0, 255 );
	defaultsWidgetDefaultColorB_->setRange( 0, 255 );
	defaultsWidgetDefaultColorR_->setSingleStep( 1 );
	defaultsWidgetDefaultColorG_->setSingleStep( 1 );
	defaultsWidgetDefaultColorB_->setSingleStep( 1 );
	defaultsWidgetDefaultColorR_->setToolTip( "The default red-component of the color used when the picked object has no colors"   );
	defaultsWidgetDefaultColorG_->setToolTip( "The default green-component of the color used when the picked object has no colors" );
	defaultsWidgetDefaultColorB_->setToolTip( "The default blue-component of the color used when the picked object has no colors"  );

	// default normal layout
	QHBoxLayout *defaultsWidgetDefaultNormalLayout = new QHBoxLayout();
	defaultsWidgetDefaultNormalLayout->setSpacing( 6 );
	defaultsWidgetDefaultNormalLayout->addWidget( defaultsWidgetDefaultNormalX_ );
	defaultsWidgetDefaultNormalLayout->addWidget( defaultsWidgetDefaultNormalY_ );
	defaultsWidgetDefaultNormalLayout->addWidget( defaultsWidgetDefaultNormalZ_ );

	// default pointsize layout
	QHBoxLayout *defaultsWidgetDefaultPointsizeLayout = new QHBoxLayout();
	defaultsWidgetDefaultPointsizeLayout->setSpacing( 6 );
	defaultsWidgetDefaultPointsizeLayout->addWidget( defaultsWidgetDefaultPointsize_ );

	// default color layout
	QHBoxLayout *defaultsWidgetDefaultColorLayout = new QHBoxLayout();
	defaultsWidgetDefaultColorLayout->setSpacing( 6 );
	defaultsWidgetDefaultColorLayout->addWidget( defaultsWidgetDefaultColorR_ );
	defaultsWidgetDefaultColorLayout->addWidget( defaultsWidgetDefaultColorG_ );
	defaultsWidgetDefaultColorLayout->addWidget( defaultsWidgetDefaultColorB_ );

	// default options layout
	QGridLayout *defaultsWidgetDefaultOptionsLayout = new QGridLayout();
	defaultsWidgetDefaultOptionsLayout->setSpacing( 6 );
	defaultsWidgetDefaultOptionsLayout->addWidget( new QLabel( "Default Normal:"    ),   0, 0 );
	defaultsWidgetDefaultOptionsLayout->addWidget( new QLabel( "Default Pointsize:" ),   1, 0 );
	defaultsWidgetDefaultOptionsLayout->addWidget( new QLabel( "Default Color:"     ),   2, 0 );
	defaultsWidgetDefaultOptionsLayout->addItem  ( defaultsWidgetDefaultNormalLayout,    0, 1 );
	defaultsWidgetDefaultOptionsLayout->addItem  ( defaultsWidgetDefaultPointsizeLayout, 1, 1 );
	defaultsWidgetDefaultOptionsLayout->addItem  ( defaultsWidgetDefaultColorLayout,     2, 1 );

	// cancel and apply buttons
	QPushButton *defaultsWidgetCancelButton = new QPushButton( "Cancel" );
	QPushButton *defaultsWidgetApplyButton  = new QPushButton( "Apply"  );
	defaultsWidgetCancelButton->setToolTip( "Return without applying any changes" );
	defaultsWidgetApplyButton->setToolTip ( "Apply the changes made and return" );

	// buttons layout
	QHBoxLayout *defaultsWidgetButtonsLayout = new QHBoxLayout();
	defaultsWidgetButtonsLayout->setSpacing( 6 );
	defaultsWidgetButtonsLayout->addWidget( defaultsWidgetCancelButton );
	defaultsWidgetButtonsLayout->addWidget( defaultsWidgetApplyButton  );

	// widget layout
	QVBoxLayout *defaultsWidgetLayout = new QVBoxLayout();
	defaultsWidgetLayout->setSpacing( 6 );
	defaultsWidgetLayout->addItem( defaultsWidgetDefaultOptionsLayout );
	defaultsWidgetLayout->addItem( defaultsWidgetButtonsLayout        );

	// widget
	defaultsWidget_ = new QWidget();
	defaultsWidget_->setWindowTitle( "Defaults" );
	defaultsWidget_->setLayout( defaultsWidgetLayout );

	// ----

	// connect events to slots
	connect( defaultsWidgetCancelButton, SIGNAL( clicked() ), this, SLOT( slotDefaultsWidgetCancelButtonClicked() ) );
	connect( defaultsWidgetApplyButton,  SIGNAL( clicked() ), this, SLOT( slotDefaultsWidgetApplyButtonClicked()  ) );

	// ----------------------------------
	// -------- create view mode --------
	// ----------------------------------

	// toolboxes
	QStringList viewModeToolboxes;
	viewModeToolboxes += "Data Control";
	viewModeToolboxes += "SplatCloud Estimator";
	viewModeToolboxes += "SplatCloud Rendering Control";

	// toolbars
	QStringList viewModeToolbars;
	viewModeToolbars += "Main Toolbar";
	viewModeToolbars += "Viewer Toolbar";
	viewModeToolbars += "Color Toolbar";
	viewModeToolbars += "Viewing Directions";

	// context menus
	QStringList viewModeContextMenus;
	viewModeContextMenus += "DataControl->Material Properties";
	viewModeContextMenus += "DataControl->Object";
	viewModeContextMenus += "SplatCloudEstimatorPlugin->SplatCloud Estimator";
	viewModeContextMenus += "SplatCloudRenderingControlPlugin->SplatCloud Rendering Control";
	viewModeContextMenus += "ViewControl->Visualization";

	// emit signals to add new viewmode
	emit defineViewModeToolboxes   ( "SplatCloud", viewModeToolboxes    );
	emit defineViewModeToolbars    ( "SplatCloud", viewModeToolbars     );
	emit defineViewModeContextMenus( "SplatCloud", viewModeContextMenus );
	emit defineViewModeIcon        ( "SplatCloud", "SplatCloudType.png" );
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotUpdateContextMenu( int _objectId )
{
	// get object by id
	if( _objectId == -1 )
		return;
	//
	BaseObjectData *object;
	if( ! PluginFunctions::getObject( _objectId, object ) )
		return;

	// get splatcloud-object
	SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( object );

	// if object is a SplatCloud...
	if( splatCloud )
	{
		// update context menu value
		contextCullingAction_->setChecked( splatCloud->isBackfaceCullingEnabled() );
	}
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotToolboxPointsizeScaleValueChanged()
{
	// get toolbox option value
	float scale = (float) toolboxPointsizeScale_->value();

	// for all splatcloud-objects...
	PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
	for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
	{
		// get splatcloud-object
		SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( *objIter );

		// apply update
		splatCloud->setPointsizeScale( scale );

		// emit signal that the object has to be updated
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
		SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( *objIter );

		// apply update
		splatCloud->enableBackfaceCulling( true );

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
		SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( *objIter );

		// apply update
		splatCloud->enableBackfaceCulling( false );

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
		SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( *objIter );

		// apply update (reload standard and picking shaders and re-set uniforms)
		splatCloud->reloadShaders();
		splatCloud->setPointsizeScale    ( splatCloud->pointsizeScale()           );
		splatCloud->enableBackfaceCulling( splatCloud->isBackfaceCullingEnabled() );

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

		// apply update (make vertex-buffer-object invalid so it will be rebuilt the next time the node is drawn (or picked))
		splatCloudNode->invalidateVBO();

		// emit signal that object has to be updated
		emit updatedObject( objIter->id(), UPDATE_ALL );
	}
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotToolboxApplyDefaultsButtonClicked()
{
	// for all splatcloud-objects...
	PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
	for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
	{
		// get scenegraph splatcloud-node
		SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( *objIter );

		// apply update (this may trigger the VBO to become invalid so it will be rebuilt)
		splatCloudNode->setDefaultNormal( SplatCloudNode::Normal( toolboxDefaultNormalX_->value(), 
		                                                          toolboxDefaultNormalY_->value(), 
		                                                          toolboxDefaultNormalZ_->value() ) );
		splatCloudNode->setDefaultPointsize( toolboxDefaultPointsize_->value() );
		splatCloudNode->setDefaultColor( SplatCloudNode::Color( toolboxDefaultColorR_->value(), 
		                                                        toolboxDefaultColorG_->value(), 
		                                                        toolboxDefaultColorB_->value() ) );

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
	SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( object );

	// if object is a SplatCloud...
	if( splatCloud )
	{
		// update scale-widget option value
		scaleWidgetPointsizeScale_->setValue( splatCloud->pointsizeScale() );

		// move scale-widget to position of context menu entry
		scaleWidget_->move( contextScaleAction_->associatedWidgets()[0]->mapToGlobal( QPoint() ) );

		// show scale-widget
		scaleWidget_->show();
	}
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotScaleWidgetPointsizeScaleValueChanged()
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
	SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( object );

	// if object is a SplatCloud...
	if( splatCloud )
	{
		// get scale-widget option value
		float scale = (float) scaleWidgetPointsizeScale_->value();

		// apply update
		splatCloud->setPointsizeScale( scale );

		// emit signal that object has to be updated
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
	SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( object );

	// if object is a SplatCloud...
	if( splatCloud )
	{
		// get context menu option value
		bool enable = contextCullingAction_->isChecked();

		// apply update
		splatCloud->enableBackfaceCulling( enable );

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
	SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( object );

	// if object is a SplatCloud...
	if( splatCloud )
	{
		// apply update (reload standard and picking shaders and re-set uniforms)
		splatCloud->reloadShaders();
		splatCloud->setPointsizeScale    ( splatCloud->pointsizeScale()           );
		splatCloud->enableBackfaceCulling( splatCloud->isBackfaceCullingEnabled() );

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
		// apply update (make vertex-buffer-object invalid so it will be rebuilt the next time the node is drawn (or picked))
		splatCloudNode->invalidateVBO();

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
		// update defaults-widget option values
		defaultsWidgetDefaultNormalX_->setValue( splatCloudNode->defaultNormal()[0] );
		defaultsWidgetDefaultNormalY_->setValue( splatCloudNode->defaultNormal()[1] );
		defaultsWidgetDefaultNormalZ_->setValue( splatCloudNode->defaultNormal()[2] );
		defaultsWidgetDefaultPointsize_->setValue( splatCloudNode->defaultPointsize() );
		defaultsWidgetDefaultColorR_->setValue( splatCloudNode->defaultColor()[0] );
		defaultsWidgetDefaultColorG_->setValue( splatCloudNode->defaultColor()[1] );
		defaultsWidgetDefaultColorB_->setValue( splatCloudNode->defaultColor()[2] );

		// move defaults widget to position of context menu entry
		defaultsWidget_->move( contextDefaultsAction_->associatedWidgets()[0]->mapToGlobal( QPoint() ) );

		// show defaults widget
		defaultsWidget_->show();
	}
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotDefaultsWidgetCancelButtonClicked()
{
	// close widget
	defaultsWidget_->close();
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotDefaultsWidgetApplyButtonClicked()
{
	// close widget
	defaultsWidget_->close();

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
		// apply update (this may trigger the VBO to become invalid so it will be rebuilt)
		splatCloudNode->setDefaultNormal( SplatCloudNode::Normal( defaultsWidgetDefaultNormalX_->value(), 
		                                                          defaultsWidgetDefaultNormalY_->value(), 
		                                                          defaultsWidgetDefaultNormalZ_->value() ) );
		splatCloudNode->setDefaultPointsize( defaultsWidgetDefaultPointsize_->value() );
		splatCloudNode->setDefaultColor( SplatCloudNode::Color( defaultsWidgetDefaultColorR_->value(), 
		                                                        defaultsWidgetDefaultColorG_->value(), 
		                                                        defaultsWidgetDefaultColorB_->value() ) );

		// emit signal that the object has to be updated
		emit updatedObject( object->id(), UPDATE_ALL );
	}
}


//================================================================


Q_EXPORT_PLUGIN2( splatcloudrenderingcontrolplugin, SplatCloudRenderingControlPlugin );
