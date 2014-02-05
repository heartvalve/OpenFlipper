/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

//=============================================================================
//
//  CLASS PropertyVisPlugin - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <Qt>

#include <QSpacerItem>
#include <QFileDialog>

#include "PropertyVisPlugin.hh"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ACG/Utils/ColorCoder.hh>

#include "PropertyModelFactory.hh"

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
    #include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
    #include <ObjectTypes/HexahedralMesh/HexahedralMesh.hh>
#endif

//== IMPLEMENTATION ==========================================================

#define PROP_VIS "PropertyVisualization"

PropertyVisPlugin::PropertyVisPlugin() :
tool_(0),
propertyModel_(0)
{
}

void PropertyVisPlugin::initializePlugin()
{
    if ( OpenFlipper::Options::gui() ) {
      tool_ = new PropertyVisToolbar();

      QSize size(300,300);
      tool_->resize(size);

      tool_->meshNames->setModel(&objectListItemModel_);

      emit addHiddenPickMode( PROP_VIS );

      QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"PropertyVisIcon.png");

      emit addToolbox( tr("Property Visualization") , tool_, toolIcon );
    }
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::pluginsInitialized() 
{
    if ( OpenFlipper::Options::gui() ) {

        // connect toolbox elements
        connect(tool_->meshNames,       SIGNAL( currentIndexChanged(int) ), this, SLOT( slotMeshChanged(int) ) );

        connect(tool_->visualizeButton, SIGNAL( clicked() ), this, SLOT( slotVisualize() ) );
        connect(tool_->clearButton,     SIGNAL( clicked() ), this, SLOT( slotAllCleared() ) );

        connect(tool_->refresh_property_names_tb, SIGNAL( clicked() ), this, SLOT( slotMeshChanged() ) );
        connect(tool_->duplicate_property_tb, SIGNAL( clicked() ), this, SLOT( slotDuplicateProperty() ) );
        connect(tool_->remove_property_tb, SIGNAL( clicked() ), this, SLOT( slotRemoveProperty() ) );

        connect(tool_, SIGNAL( widgetShown() ), this, SLOT( updateGUI() ) );

        setNewPropertyModel(-1);
    }
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotPickModeChanged( const std::string& _mode)
{
    if (propertyModel_ != 0)
        propertyModel_->pickModeChanged(_mode);
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotAllCleared()
{
    if (propertyModel_  != 0)
    {
        QModelIndexList selectedIndices = tool_->propertyName_lv->selectionModel()->selectedIndexes();
        propertyModel_->clear(selectedIndices);
        propertyModel_->objectUpdated();

        emit updateView();
    }
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::objectDeleted(int _id)
{
    if( OpenFlipper::Options::gui() )
        objectListItemModel_.removeObject(_id);
    PropertyModelFactory::Instance().deleteModel(_id);
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotObjectUpdated( int _identifier, const UpdateType& _type )
{
    if( OpenFlipper::Options::gui() )
    {
        if ( tool_->isVisible() )
            updateGUI();
        PropertyModel* propertyModel = PropertyModelFactory::Instance().getModel(_identifier);
        if (propertyModel)
        {
            if (_type == UPDATE_ALL)
                propertyModel->gatherProperties();
            if (_type == (UPDATE_ALL | UPDATE_GEOMETRY))
                propertyModel->objectUpdated();
        }
    }
}

void PropertyVisPlugin::updateGUI()
{
    DataType datatype = DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH);
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
    datatype |= DataType(DATA_POLYHEDRAL_MESH);
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
    datatype |= DataType(DATA_HEXAHEDRAL_MESH);
#endif
    objectListItemModel_.refresh(datatype);
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::propertySelectionChanged()
{
    if (propertyModel_ != 0)
    {
        QModelIndexList selectedIndices = tool_->propertyName_lv->selectionModel()->selectedIndexes();
        propertyModel_->updateWidget(selectedIndices);
    }
}

//-----------------------------------------------------------------------------


/*! \brief Exchanges the PropertyModel after the user selected a different object.
 *
 * This method exchanges the PropertyModel after the user selects a different object
 * for visualization. The widget of the old model needs to be hidden and the one of
 * the new model is shown. Also some signals need to be disconnected and connected.
 *
 * \param id The id of the selected object
 */
void PropertyVisPlugin::setNewPropertyModel(int id)
{
    if (propertyModel_)
    {
        propertyModel_->hideWidget();
        disconnect(propertyModel_, SIGNAL(log(Logtype,QString)), this, SLOT(slotLog(Logtype,QString)));
        disconnect(propertyModel_, SIGNAL(log(QString)), this, SLOT(slotLog(QString)));
    }
    propertyModel_ = PropertyModelFactory::Instance().getModel(id);
    if (propertyModel_ != 0)
    {

        tool_->propertyName_lv->setModel(propertyModel_);
        connect(propertyModel_, SIGNAL( modelReset() ), this, SLOT( propertySelectionChanged() ));
        connect(tool_->propertyName_lv->selectionModel(),
                SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
                this,
                SLOT( propertySelectionChanged() ));
        QWidget* widget = propertyModel_->getWidget();
        tool_->propertyWidgets->addWidget(widget);
        widget->show();
        propertyModel_->gatherProperties();
        connect(propertyModel_, SIGNAL(log(Logtype,QString)), this, SLOT(slotLog(Logtype,QString)));
        connect(propertyModel_, SIGNAL(log(QString)), this, SLOT(slotLog(QString)));
    }
    else
    {
        tool_->propertyName_lv->setModel(0);
    }
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotMeshChanged(int /*_index*/)
{
  int id = tool_->meshNames->itemData( tool_->meshNames->currentIndex() ).toInt();
  BaseObjectData* object = 0;

  PluginFunctions::getObject( id, object );
  setNewPropertyModel(id);
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotVisualize()
{
    if (propertyModel_ != 0)
    {
        QModelIndexList selectedIndices = tool_->propertyName_lv->selectionModel()->selectedIndexes();
        propertyModel_->visualize(selectedIndices);

        emit updateView();
        int id = tool_->meshNames->itemData( tool_->meshNames->currentIndex() ).toInt();
        emit updatedObject( id, UPDATE_COLOR );
    }
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotMouseEvent( QMouseEvent* _event ) {
    if (propertyModel_ != 0)
        propertyModel_->mouseEvent(_event);
}

//-----------------------------------------------------------------------------

void PropertyVisPlugin::slotDuplicateProperty() {
    if (propertyModel_ != 0)
    {
        QModelIndexList selectedIndices = tool_->propertyName_lv->selectionModel()->selectedIndexes();
        propertyModel_->duplicateProperty(selectedIndices);

        emit updateView();
        int id = tool_->meshNames->itemData( tool_->meshNames->currentIndex() ).toInt();
        slotMeshChanged();
        emit updatedObject( id, UPDATE_ALL );
    }
}

void PropertyVisPlugin::slotRemoveProperty()
{
    if (propertyModel_ != 0)
    {
        QModelIndexList selectedIndices = tool_->propertyName_lv->selectionModel()->selectedIndexes();
        propertyModel_->removeProperty(selectedIndices);

        emit updateView();
        int id = tool_->meshNames->itemData( tool_->meshNames->currentIndex() ).toInt();
        emit updatedObject( id, UPDATE_ALL );
    }
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( propertyvisplugin , PropertyVisPlugin );
#endif

