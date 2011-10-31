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
//  CLASS SplatCloudRenderingControlPlugin
//
//    This class represents an OpenFlipper plugin controlling the rendering of SplatClouds.
//
//================================================================


#ifndef SPLATCLOUDRENDERINGCONTROLPLUGIN_HH
#define SPLATCLOUDRENDERINGCONTROLPLUGIN_HH


//== INCLUDES ====================================================


#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/ViewModeInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#include <OpenFlipper/common/Types.hh>


//== CLASS DEFINITION ============================================


class SplatCloudRenderingControlPlugin : public QObject, BaseInterface, ToolboxInterface, ContextMenuInterface, ViewModeInterface, LoggingInterface
{
	Q_OBJECT
	Q_INTERFACES( BaseInterface        )
	Q_INTERFACES( ToolboxInterface     )
	Q_INTERFACES( ContextMenuInterface )
	Q_INTERFACES( ViewModeInterface    )
	Q_INTERFACES( LoggingInterface     )

signals:

	//-- Base Interface --
	void updatedObject( int _identifier, const UpdateType _type);

	//-- Toolbox Interface --
	void addToolbox( QString  _name, QWidget *_widget, QIcon *_icon );

	//-- ContextMenu Interface--
	void addContextMenuItem( QAction *_action, DataType _objectType, ContextMenuType _type );

	//-- ViewMode Interface--
	void defineViewModeToolboxes   ( QString _mode, QStringList _usedWidgets      );
	void defineViewModeToolbars    ( QString _mode, QStringList _usedToolbars     );
	void defineViewModeContextMenus( QString _mode, QStringList _usedContextMenus );
	void defineViewModeIcon        ( QString _mode, QString _iconName             );

	//-- Logging Interface --
	void log( Logtype _type, QString _message );
	void log( QString _message );

public:

	//-- Base Interface --
	QString name() { return QString( "SplatCloud Rendering Control Plugin" ); }
	QString description() { return QString( "Control over rendering of SplatClouds" ); }

private:

	// toolbox options
	QDoubleSpinBox *toolboxPointsizeScale_;
	QDoubleSpinBox *toolboxDefaultNormalX_;
	QDoubleSpinBox *toolboxDefaultNormalY_;
	QDoubleSpinBox *toolboxDefaultNormalZ_;
	QDoubleSpinBox *toolboxDefaultPointsize_;
	QSpinBox       *toolboxDefaultColorR_;
	QSpinBox       *toolboxDefaultColorG_;
	QSpinBox       *toolboxDefaultColorB_;

	// context menu options
	QAction *contextScaleAction_;
	QAction *contextCullingAction_;
	QAction *contextDefaultsAction_;
	QAction *contextReloadShadersAction_;
	QAction *contextRebuildVBOAction_;

	// scale widget options
	QWidget        *scaleWidget_;
	QDoubleSpinBox *scaleWidgetPointsizeScale_;

	// defaults widget options
	QWidget        *defaultsWidget_;
	QDoubleSpinBox *defaultsWidgetDefaultNormalX_;
	QDoubleSpinBox *defaultsWidgetDefaultNormalY_;
	QDoubleSpinBox *defaultsWidgetDefaultNormalZ_;
	QDoubleSpinBox *defaultsWidgetDefaultPointsize_;
	QSpinBox       *defaultsWidgetDefaultColorR_;
	QSpinBox       *defaultsWidgetDefaultColorG_;
	QSpinBox       *defaultsWidgetDefaultColorB_;

private slots:

	//-- Base Interface --
	void initializePlugin();

	//-- ContextMenu Interface --
	void slotUpdateContextMenu( int _objectId );

	// slot called when a toolbox option changed value
	void slotToolboxPointsizeScaleValueChanged();

	// slots called when a toolbox button was clicked
	void slotToolboxEnableBackfaceCullingButtonClicked();
	void slotToolboxDisableBackfaceCullingButtonClicked();
	void slotToolboxReloadShadersButtonClicked();
	void slotToolboxRebuildVBOsButtonClicked();
	void slotToolboxApplyDefaultsButtonClicked();

	// slots called when a context menu action was triggered
	void slotContextScaleActionTriggered();
	void slotContextCullingActionTriggered();
	void slotContextReloadShadersActionTriggered();
	void slotContextRebuildVBOActionTriggered();
	void slotContextDefaultsActionTriggered();

	// slots called when a scale widget option changed value
	void slotScaleWidgetPointsizeScaleValueChanged();

	// slots called when a defaults widget button was clicked
	void slotDefaultsWidgetCancelButtonClicked();
	void slotDefaultsWidgetApplyButtonClicked();

public slots:

	// -- Base Interface --
	QString version() { return QString( "1.0" ); }
};


//================================================================


#endif // SPLATCLOUDRENDERINGCONTROLPLUGIN_HH
