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

	// ---- options ----

	// pointsizeScale
	toolboxPointsizeScale_ = new QDoubleSpinBox();
	toolboxPointsizeScale_->setRange( 0.0, 100.0 );
	toolboxPointsizeScale_->setDecimals( 4 );
	toolboxPointsizeScale_->setSingleStep( 0.1 );
	toolboxPointsizeScale_->setValue( 1.0 );

	// pointsizeScale layout
	QHBoxLayout *toolboxPointsizeScaleLayout = new QHBoxLayout();
	toolboxPointsizeScaleLayout->addWidget( new QLabel( "Pointsize Scale:" ) );
	toolboxPointsizeScaleLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	toolboxPointsizeScaleLayout->addWidget( toolboxPointsizeScale_           );

	// backfaceCulling
	toolboxBackfaceCulling_ = new QCheckBox( "Backface Culling" );
	toolboxBackfaceCulling_->setChecked( false );

	// reloadShaders and rebuildVBOs buttons
	QPushButton *toolboxReloadShadersButton = new QPushButton( "Reload Shaders" );
	QPushButton *toolboxRebuildVBOsButton   = new QPushButton( "Rebuild VBOs"   );

	// buttons layout
	QHBoxLayout *toolboxButtonsLayout = new QHBoxLayout();
	toolboxButtonsLayout->addWidget( toolboxReloadShadersButton );
	toolboxButtonsLayout->addItem  ( new QSpacerItem( 8, 8 )    );
	toolboxButtonsLayout->addWidget( toolboxRebuildVBOsButton   );

	// options layout
	QVBoxLayout *toolboxOptionsLayout = new QVBoxLayout();
	toolboxOptionsLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	toolboxOptionsLayout->addItem  ( toolboxPointsizeScaleLayout );
	toolboxOptionsLayout->addWidget( toolboxBackfaceCulling_     );
	toolboxOptionsLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	toolboxOptionsLayout->addItem  ( toolboxButtonsLayout        );

	// ---- defaults ----

	// defaultNormal
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

	// defaultNormal layout
	QHBoxLayout *toolboxDefaultNormalLayout = new QHBoxLayout();
	toolboxDefaultNormalLayout->addWidget( new QLabel( "Default Normal:" ) );
	toolboxDefaultNormalLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	toolboxDefaultNormalLayout->addWidget( toolboxDefaultNormalX_          );
	toolboxDefaultNormalLayout->addItem  ( new QSpacerItem( 4, 4 )         );
	toolboxDefaultNormalLayout->addWidget( toolboxDefaultNormalY_          );
	toolboxDefaultNormalLayout->addItem  ( new QSpacerItem( 4, 4 )         );
	toolboxDefaultNormalLayout->addWidget( toolboxDefaultNormalZ_          );

	// defaultPointsize
	toolboxDefaultPointsize_ = new QDoubleSpinBox();
	toolboxDefaultPointsize_->setRange( 0.0, 1000.0 );
	toolboxDefaultPointsize_->setDecimals( 4 );
	toolboxDefaultPointsize_->setSingleStep( 0.001 );
	toolboxDefaultPointsize_->setValue( 0.01 );

	// defaultPointsize layout
	QHBoxLayout *toolboxDefaultPointsizeLayout = new QHBoxLayout();
	toolboxDefaultPointsizeLayout->addWidget( new QLabel( "Default Pointsize:" ) );
	toolboxDefaultPointsizeLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	toolboxDefaultPointsizeLayout->addWidget( toolboxDefaultPointsize_           );

	// defaultColor
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

	// defaultColor layout
	QHBoxLayout *toolboxDefaultColorLayout = new QHBoxLayout();
	toolboxDefaultColorLayout->addWidget( new QLabel( "Default Color:" ) );
	toolboxDefaultColorLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	toolboxDefaultColorLayout->addWidget( toolboxDefaultColorR_          );
	toolboxDefaultColorLayout->addItem  ( new QSpacerItem( 4, 4 )        );
	toolboxDefaultColorLayout->addWidget( toolboxDefaultColorG_          );
	toolboxDefaultColorLayout->addItem  ( new QSpacerItem( 4, 4 )        );
	toolboxDefaultColorLayout->addWidget( toolboxDefaultColorB_          );

	// applyDefaults button
	QPushButton *toolboxApplyDefaultsButton = new QPushButton( "Apply Defaults" );

	// defaults layout
	QVBoxLayout *toolboxDefaultsLayout = new QVBoxLayout();
	toolboxDefaultsLayout->addItem  ( toolboxDefaultNormalLayout    );
	toolboxDefaultsLayout->addItem  ( toolboxDefaultPointsizeLayout );
	toolboxDefaultsLayout->addItem  ( toolboxDefaultColorLayout     );
	toolboxDefaultsLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	toolboxDefaultsLayout->addWidget( toolboxApplyDefaultsButton    );

	// ----

	// options widget
	QWidget *toolboxOptionsWidget = new QWidget();
	toolboxOptionsWidget->setLayout( toolboxOptionsLayout );

	// defaults widget
	QWidget *toolboxDefaultsWidget = new QWidget();
	toolboxDefaultsWidget->setLayout( toolboxDefaultsLayout );

	// tab widget
	QTabWidget *toolboxTabWidget = new QTabWidget();
	toolboxTabWidget->addTab( toolboxOptionsWidget,  "Options"  );
	toolboxTabWidget->addTab( toolboxDefaultsWidget, "Defaults" );

	// toolbox layout
	QVBoxLayout *toolboxLayout = new QVBoxLayout();
	toolboxLayout->addWidget( toolboxTabWidget );

	// toolbox
	QWidget *toolbox = new QWidget();
	toolbox->setLayout( toolboxLayout );

	// ----

	// connect events to slots
	connect( toolboxPointsizeScale_,     SIGNAL( valueChanged(double) ), this, SLOT( slotToolboxPointsizeScaleValueChanged()  ) );
	connect( toolboxBackfaceCulling_,    SIGNAL( stateChanged(int)    ), this, SLOT( slotToolboxBackfaceCullingStateChanged() ) );
	connect( toolboxReloadShadersButton, SIGNAL( clicked()            ), this, SLOT( slotToolboxReloadShadersButtonClicked()  ) );
	connect( toolboxRebuildVBOsButton,   SIGNAL( clicked()            ), this, SLOT( slotToolboxRebuildVBOsButtonClicked()    ) );
	connect( toolboxApplyDefaultsButton, SIGNAL( clicked()            ), this, SLOT( slotToolboxApplyDefaultsButtonClicked()  ) );

	// emit signal to add the new toolbox
	toolBoxIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"SplatCloudType.png");
	emit addToolbox( tr("SplatCloud Rendering Control") , toolbox , toolBoxIcon_);

	// -------------------------------------
	// -------- create context menu --------
	// -------------------------------------

	// scale action
	menuScaleAction_ = new QAction( tr("Pointsize Scale..."), this );

	// culling action
	menuCullingAction_ = new QAction( tr("Backface Culling"), this );
	menuCullingAction_->setCheckable( true );

	// defaults action
	menuDefaultsAction_ = new QAction( tr("Defaults..."), this );

	// reloadShaders action
	menuReloadShadersAction_ = new QAction( tr("Reload Shaders"), this );

	// rebuildVBO action
	menuRebuildVBOAction_ = new QAction( tr("Rebuild VBO"), this );

	// menu
	QMenu *menu = new QMenu( "SplatCloud Rendering Control" );
	menu->addAction( menuScaleAction_         );
	menu->addAction( menuCullingAction_       );
	menu->addSeparator();
	menu->addAction( menuDefaultsAction_      );
	menu->addSeparator();
	menu->addAction( menuReloadShadersAction_ );
	menu->addAction( menuRebuildVBOAction_    );

	// ----

	// connect events to slots
	connect( menuScaleAction_,         SIGNAL( triggered() ), this, SLOT( slotMenuScaleActionTriggered()         ) );
	connect( menuCullingAction_,       SIGNAL( triggered() ), this, SLOT( slotMenuCullingActionTriggered()       ) );
	connect( menuDefaultsAction_,      SIGNAL( triggered() ), this, SLOT( slotMenuDefaultsActionTriggered()      ) );
	connect( menuReloadShadersAction_, SIGNAL( triggered() ), this, SLOT( slotMenuReloadShadersActionTriggered() ) );
	connect( menuRebuildVBOAction_,    SIGNAL( triggered() ), this, SLOT( slotMenuRebuildVBOActionTriggered()    ) );

	// emit signal to add the new contest menu
	emit addContextMenuItem( menu->menuAction(), DATA_SPLATCLOUD, CONTEXTOBJECTMENU );

	// -------------------------------------
	// -------- create scale widget --------
	// -------------------------------------

	// pointsizeScale
	scaleWidgetPointsizeScale_ = new QDoubleSpinBox();
	scaleWidgetPointsizeScale_->setRange( 0.0, 100.0 );
	scaleWidgetPointsizeScale_->setDecimals( 4 );
	scaleWidgetPointsizeScale_->setSingleStep( 0.1 );

	// pointsizeScale layout
	QHBoxLayout *scaleWidgetPointsizeScaleLayout = new QHBoxLayout();
	scaleWidgetPointsizeScaleLayout->addWidget( new QLabel( "Pointsize Scale:" ) );
	scaleWidgetPointsizeScaleLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	scaleWidgetPointsizeScaleLayout->addWidget( scaleWidgetPointsizeScale_       );

	// widget layout
	QVBoxLayout *scaleWidgetLayout = new QVBoxLayout();
	scaleWidgetLayout->addItem( scaleWidgetPointsizeScaleLayout );

	// widget
	scaleWidget_ = new QWidget();
	scaleWidget_->setWindowTitle( "Pointsize Scale" );
	scaleWidget_->setLayout( scaleWidgetLayout );

	// ----

	// connect events to slots
	connect( scaleWidgetPointsizeScale_, SIGNAL( valueChanged(double) ), this, SLOT( slotScaleWidgetPointsizeScaleValueChanged() ) );

	// ----------------------------------------
	// -------- create defaults widget --------
	// ----------------------------------------

	// defaultNormal
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

	// defaultNormal layout
	QHBoxLayout *defaultsWidgetDefaultNormalLayout = new QHBoxLayout();
	defaultsWidgetDefaultNormalLayout->addWidget( new QLabel( "Default Normal:" ) );
	defaultsWidgetDefaultNormalLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	defaultsWidgetDefaultNormalLayout->addWidget( defaultsWidgetDefaultNormalX_   );
	defaultsWidgetDefaultNormalLayout->addItem  ( new QSpacerItem( 4, 4 )         );
	defaultsWidgetDefaultNormalLayout->addWidget( defaultsWidgetDefaultNormalY_   );
	defaultsWidgetDefaultNormalLayout->addItem  ( new QSpacerItem( 4, 4 )         );
	defaultsWidgetDefaultNormalLayout->addWidget( defaultsWidgetDefaultNormalZ_   );

	// defaultPointsize
	defaultsWidgetDefaultPointsize_ = new QDoubleSpinBox();
	defaultsWidgetDefaultPointsize_->setRange( 0.0, 1000.0 );
	defaultsWidgetDefaultPointsize_->setDecimals( 4 );
	defaultsWidgetDefaultPointsize_->setSingleStep( 0.001 );

	// defaultPointsize layout
	QHBoxLayout *defaultsWidgetDefaultPointsizeLayout = new QHBoxLayout();
	defaultsWidgetDefaultPointsizeLayout->addWidget( new QLabel( "Default Pointsize:" ) );
	defaultsWidgetDefaultPointsizeLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	defaultsWidgetDefaultPointsizeLayout->addWidget( defaultsWidgetDefaultPointsize_    );

	// defaultColor
	defaultsWidgetDefaultColorR_ = new QSpinBox();
	defaultsWidgetDefaultColorG_ = new QSpinBox();
	defaultsWidgetDefaultColorB_ = new QSpinBox();
	defaultsWidgetDefaultColorR_->setRange( 0, 255 );
	defaultsWidgetDefaultColorG_->setRange( 0, 255 );
	defaultsWidgetDefaultColorB_->setRange( 0, 255 );
	defaultsWidgetDefaultColorR_->setSingleStep( 1 );
	defaultsWidgetDefaultColorG_->setSingleStep( 1 );
	defaultsWidgetDefaultColorB_->setSingleStep( 1 );

	// defaultColor layout
	QHBoxLayout *defaultsWidgetDefaultColorLayout = new QHBoxLayout();
	defaultsWidgetDefaultColorLayout->addWidget( new QLabel( "Default Color:" ) );
	defaultsWidgetDefaultColorLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	defaultsWidgetDefaultColorLayout->addWidget( defaultsWidgetDefaultColorR_   );
	defaultsWidgetDefaultColorLayout->addItem  ( new QSpacerItem( 4, 4 )        );
	defaultsWidgetDefaultColorLayout->addWidget( defaultsWidgetDefaultColorG_   );
	defaultsWidgetDefaultColorLayout->addItem  ( new QSpacerItem( 4, 4 )        );
	defaultsWidgetDefaultColorLayout->addWidget( defaultsWidgetDefaultColorB_   );

	// cancel and apply buttons
	QPushButton *defaultsWidgetCancelButton = new QPushButton( "Cancel" );
	QPushButton *defaultsWidgetApplyButton  = new QPushButton( "Apply"  );

	// buttons layout
	QHBoxLayout *defaultsWidgetButtonsLayout = new QHBoxLayout();
	defaultsWidgetButtonsLayout->addWidget( defaultsWidgetCancelButton );
	defaultsWidgetButtonsLayout->addItem  ( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	defaultsWidgetButtonsLayout->addWidget( defaultsWidgetApplyButton  );

	// widget layout
	QVBoxLayout *defaultsWidgetLayout = new QVBoxLayout();
	defaultsWidgetLayout->addItem( defaultsWidgetDefaultNormalLayout    );
	defaultsWidgetLayout->addItem( defaultsWidgetDefaultPointsizeLayout );
	defaultsWidgetLayout->addItem( defaultsWidgetDefaultColorLayout     );
	defaultsWidgetLayout->addItem( new QSpacerItem( 8, 8, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	defaultsWidgetLayout->addItem( defaultsWidgetButtonsLayout          );

	// widget
	defaultsWidget_ = new QWidget();
	defaultsWidget_->setWindowTitle( "Defaults" );
	defaultsWidget_->setLayout( defaultsWidgetLayout );

	// ----

	// connect events to slots
	connect( defaultsWidgetCancelButton, SIGNAL( clicked() ), this, SLOT( slotDefaultsWidgetCancelButtonClicked() ) );
	connect( defaultsWidgetApplyButton,  SIGNAL( clicked() ), this, SLOT( slotDefaultsWidgetApplyButtonClicked()  ) );
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotUpdateContextMenu( int _objectId )
{
	// get object by id

	if( _objectId == -1 )
		return;

	BaseObjectData *object;
	if( ! PluginFunctions::getObject( _objectId, object ) )
		return;

	// get splatcloud-object
	SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( object );

	// If it is not a splat object, we stop here
	if ( splatCloud != 0 ) {
	  // update context-menu value
	  menuCullingAction_->setChecked( splatCloud->isBackfaceCullingEnabled() );
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


void SplatCloudRenderingControlPlugin::slotToolboxBackfaceCullingStateChanged()
{
	// get toolbox option value
	bool enable = toolboxBackfaceCulling_->isChecked();

	// for all splatcloud-objects...
	PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
	for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
	{
		// get splatcloud-object
		SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( *objIter );

		// apply update
		splatCloud->enableBackfaceCulling( enable );

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
		// get scenegraph splatcloud-node
		SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( *objIter );

		// apply update (reload standard and picking shaders)
		splatCloud->reloadShaders();

		// emit signal that object has to be updated
		emit updatedObject( objIter->id(), UPDATE_ALL );
	}

	// after reloading a shader we have to re-set all uniforms for this shader
	slotToolboxPointsizeScaleValueChanged();
	slotToolboxBackfaceCullingStateChanged();
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

		if ( splatCloudNode != 0) {
		  // apply update (make vertex-buffer-object invalid so it will be rebuilt the next time the node is drawn (or picked))
		  splatCloudNode->invalidateVBO();

		  // emit signal that object has to be updated
		  emit updatedObject( objIter->id(), UPDATE_ALL );
		}
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

		if ( splatCloudNode ) {
		  // apply update (this may trigger the VBO to become invalid so it will be rebuilt)
		  splatCloudNode->setDefaultNormal( SplatCloudNode::Normal(	toolboxDefaultNormalX_->value(),
			                               														toolboxDefaultNormalY_->value(),
			                               														toolboxDefaultNormalZ_->value() ) );
		  splatCloudNode->setDefaultPointsize( toolboxDefaultPointsize_->value() );
		  splatCloudNode->setDefaultColor( SplatCloudNode::Color(	toolboxDefaultColorR_->value(),
																                              toolboxDefaultColorG_->value(),
																                              toolboxDefaultColorB_->value() ) );

		  // emit signal that object has to be updated
		  emit updatedObject( objIter->id(), UPDATE_ALL );
		}
	}
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotMenuScaleActionTriggered()
{
	// get picked object

	QVariant contextObject = menuScaleAction_->data();

	int objectId = contextObject.toInt();
	if( objectId == -1 )
		return;

	BaseObjectData *object;
	if( ! PluginFunctions::getObject( objectId, object ) )
		return;

	// get splatcloud-object
	SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( object );

	// update scale-widget option value
	scaleWidgetPointsizeScale_->setValue( splatCloud->pointsizeScale() );

	// move scale-widget to position of context menu entry
	QWidget *menuWidget = menuScaleAction_->associatedWidgets()[0];
	scaleWidget_->move( menuWidget->x(), menuWidget->y() );

	// show scale-widget
	scaleWidget_->show();
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotScaleWidgetPointsizeScaleValueChanged()
{
	// get picked object

	QVariant contextObject = menuScaleAction_->data();

	int objectId = contextObject.toInt();
	if( objectId == -1 )
		return;

	BaseObjectData *object;
	if( ! PluginFunctions::getObject( objectId, object ) )
		return;

	// get scale-widget option value
	float scale = (float) scaleWidgetPointsizeScale_->value();

	// get splatcloud-object
	SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( object );

	// apply update
	splatCloud->setPointsizeScale( scale );

	// emit signal that object has to be updated
	emit updatedObject( object->id(), UPDATE_ALL );
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotMenuCullingActionTriggered()
{
	// get picked object

	QVariant contextObject = menuCullingAction_->data();

	int objectId = contextObject.toInt();
	if( objectId == -1 )
		return;

	BaseObjectData *object;
	if( ! PluginFunctions::getObject( objectId, object ) )
		return;

	// get context-menu option value
	bool enable = menuCullingAction_->isChecked();

	// get splatcloud-object
	SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( object );

	// apply update
	splatCloud->enableBackfaceCulling( enable );

	// emit signal that the object has to be updated
	emit updatedObject( object->id(), UPDATE_ALL );
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotMenuReloadShadersActionTriggered()
{
	// get picked object

	QVariant contextObject = menuReloadShadersAction_->data();

	int objectId = contextObject.toInt();
	if( objectId == -1 )
		return;

	BaseObjectData *object;
	if( ! PluginFunctions::getObject( objectId, object ) )
		return;

	// get splatcloud-object
	SplatCloudObject *splatCloud = PluginFunctions::splatCloudObject( object );

	// apply update (reload standard and picking shaders)
	splatCloud->reloadShaders();

	// emit signal that object has to be updated
	emit updatedObject( object->id(), UPDATE_ALL );
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotMenuRebuildVBOActionTriggered()
{
	// get picked object

	QVariant contextObject = menuRebuildVBOAction_->data();

	int objectId = contextObject.toInt();
	if( objectId == -1 )
		return;

	BaseObjectData *object;
	if( ! PluginFunctions::getObject( objectId, object ) )
		return;

	// get splatcloud-node
	SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( object );

	if ( splatCloudNode != 0) {
	  // apply update (make vertex-buffer-object invalid so it will be rebuilt the next time the node is drawn (or picked))
	  splatCloudNode->invalidateVBO();

	  // emit signal that object has to be updated
	  emit updatedObject( object->id(), UPDATE_ALL );
	}
}


//----------------------------------------------------------------


void SplatCloudRenderingControlPlugin::slotMenuDefaultsActionTriggered()
{
	// get picked object

	QVariant contextObject = menuDefaultsAction_->data();

	int objectId = contextObject.toInt();
	if( objectId == -1 )
		return;

	BaseObjectData *object;
	if( ! PluginFunctions::getObject( objectId, object ) )
		return;

	// get splatcloud-node
	SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( object );

	if ( splatCloudNode ) {
	  // update defaults-widget option values
	  defaultsWidgetDefaultNormalX_->setValue( splatCloudNode->defaultNormal()[0] );
	  defaultsWidgetDefaultNormalY_->setValue( splatCloudNode->defaultNormal()[1] );
	  defaultsWidgetDefaultNormalZ_->setValue( splatCloudNode->defaultNormal()[2] );
	  defaultsWidgetDefaultPointsize_->setValue( splatCloudNode->defaultPointsize() );
	  defaultsWidgetDefaultColorR_->setValue( splatCloudNode->defaultColor()[0] );
	  defaultsWidgetDefaultColorG_->setValue( splatCloudNode->defaultColor()[1] );
	  defaultsWidgetDefaultColorB_->setValue( splatCloudNode->defaultColor()[2] );

	  // move defaults widget to position of context menu entry
	  QWidget *menuWidget = menuDefaultsAction_->associatedWidgets()[0];
	  defaultsWidget_->move( menuWidget->x(), menuWidget->y() );

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
	// get picked object

	QVariant contextObject = menuDefaultsAction_->data();

	int objectId = contextObject.toInt();
	if( objectId == -1 )
		return;

	BaseObjectData *object;
	if( ! PluginFunctions::getObject( objectId, object ) )
		return;

	// get splatcloud-node
	SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( object );

	if ( splatCloudNode != 0) {

	  // apply update (this may trigger the VBO to become invalid so it will be rebuilt)
	  splatCloudNode->setDefaultNormal( SplatCloudNode::Normal(	defaultsWidgetDefaultNormalX_->value(),
	                                                            defaultsWidgetDefaultNormalY_->value(),
	                                                            defaultsWidgetDefaultNormalZ_->value() ) );
	  splatCloudNode->setDefaultPointsize( defaultsWidgetDefaultPointsize_->value() );
	  splatCloudNode->setDefaultColor( SplatCloudNode::Color(	defaultsWidgetDefaultColorR_->value(),
	                                                          defaultsWidgetDefaultColorG_->value(),
	                                                          defaultsWidgetDefaultColorB_->value() ) );

	  // emit signal that the object has to be updated
	  emit updatedObject( object->id(), UPDATE_ALL );
	}

	// close widget
	defaultsWidget_->close();
}


//================================================================


Q_EXPORT_PLUGIN2( splatcloudrenderingcontrolplugin, SplatCloudRenderingControlPlugin );
