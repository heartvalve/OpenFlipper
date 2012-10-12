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
*   $Revision: 14395 $                                                       *
*   $LastChangedBy: moeller $                                                *
*   $Date: 2012-04-16 14:46:02 +0200 (Mon, 16 Apr 2012) $                     *
*                                                                            *
\*===========================================================================*/


#include "MaterialPicker.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//------------------------------------------------------------------------------
MaterialPicker::MaterialPicker()
  :
  pickModeName_("MaterialPicker"),
  pickMaterialButton_(0),
  fillMaterialButton_(0),
  materialListWidget_(0),
  pickMaterial_(false),
  fillMaterial_(false)
{
}

//------------------------------------------------------------------------------

MaterialPicker::~MaterialPicker() {

}

//------------------------------------------------------------------------------

void MaterialPicker::initializePlugin() {
   QWidget* toolBox = new QWidget();
   QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, toolBox);
   QGridLayout* grid = new QGridLayout();

   pickMaterialButton_ = new QPushButton("&pick Material", toolBox);
   fillMaterialButton_ = new QPushButton("&fill Material", toolBox);
   QPushButton* clearListButton =  new QPushButton("clear list", toolBox);

   pickMaterialButton_->setCheckable(true);
   fillMaterialButton_->setCheckable(true);

   QLabel* materials = new QLabel("Materials:");

   materialListWidget_ = new QListWidget(toolBox);

   layout->addWidget(materials);
   layout->addWidget(materialListWidget_);
   layout->addWidget(clearListButton);
   grid->addWidget(pickMaterialButton_, 0, 0);
   grid->addWidget(fillMaterialButton_, 0, 1);

   layout->addLayout(grid);

   connect(pickMaterialButton_, SIGNAL(clicked()), this, SLOT(slotPickMaterialMode()));
   connect(fillMaterialButton_, SIGNAL(clicked()), this, SLOT(slotFillMaterialMode()));
   connect(clearListButton, SIGNAL(clicked()), this, SLOT(clearList()));
   connect(materialListWidget_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editMode(QListWidgetItem*)));

   emit addToolbox( tr("Material Picker"), toolBox);
}

//------------------------------------------------------------------------------

void MaterialPicker::clearList() {
  materialListWidget_->clear();
  materialList_.clear();
}

//------------------------------------------------------------------------------

void MaterialPicker::slotPickMaterialMode() {
  pickMaterialButton_->setChecked(true);
  fillMaterialButton_->setChecked(false);
  pickMaterial_ = true;
  fillMaterial_ = false;

  PluginFunctions::actionMode( Viewer::PickingMode );
  PluginFunctions::pickMode(pickModeName_);
}

//------------------------------------------------------------------------------

void MaterialPicker::slotFillMaterialMode() {
  fillMaterialButton_->setChecked(true);
  pickMaterialButton_->setChecked(false);
  fillMaterial_ = true;
  pickMaterial_ = false;

  PluginFunctions::actionMode( Viewer::PickingMode );
  PluginFunctions::pickMode(pickModeName_);
}

//------------------------------------------------------------------------------

void MaterialPicker::pluginsInitialized() {
  emit addPickMode(pickModeName_);
}

//------------------------------------------------------------------------------

void MaterialPicker::slotMouseEvent(QMouseEvent* _event) {
  if ( PluginFunctions::pickMode() != pickModeName_)
    return;

  if (_event->type() == QEvent::MouseButtonPress) {
    unsigned int node_idx, target_idx;
    OpenMesh::Vec3d hitPoint;

    // Get picked object's identifier by picking in scenegraph
    if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING ,_event->pos(), node_idx, target_idx, &hitPoint) ){
      BaseObjectData* object;
      if ( PluginFunctions::getPickedObject(node_idx, object) ) {

        // pick material
        if ( pickMaterial_ && !fillMaterial_ ) {
          MaterialNode* material = object->materialNode();

          if (material) {

            // store the material information
            MaterialInfo materialInfo;
            materialInfo.color_material = material->colorMaterial();
            materialInfo.base_color = material->base_color();
            materialInfo.ambient_color = material->ambient_color();
            materialInfo.diffuse_color = material->diffuse_color();
            materialInfo.specular_color = material->specular_color();
            materialInfo.shininess = material->shininess();
            materialInfo.reflectance = material->reflectance();

            // update list widget and material list
            materialListWidget_->addItem( QString("material id: %1").arg(material->id()) );
            materialListWidget_->setCurrentItem( materialListWidget_->item(materialListWidget_->count() - 1) );

            materialList_.push_back(materialInfo);

          }

        // apply material from current item in list widget to picked object
        } else if ( fillMaterial_ && !pickMaterial_ ){
          MaterialNode* material = object->materialNode();

          if (material) {

            int row = materialListWidget_->currentRow();
            material->colorMaterial(materialList_[row].color_material);
            material->set_base_color(materialList_[row].base_color);
            material->set_ambient_color(materialList_[row].ambient_color);
            material->set_diffuse_color(materialList_[row].diffuse_color);
            material->set_specular_color(materialList_[row].specular_color);
            material->set_shininess(materialList_[row].shininess);
            material->set_reflectance(materialList_[row].reflectance);

          }
        }
      }
    }
  }
}

//------------------------------------------------------------------------------

void MaterialPicker::editMode(QListWidgetItem* _item) {
  _item->setFlags(_item->flags() | Qt::ItemIsEditable);
  materialListWidget_->editItem(_item);
}

//------------------------------------------------------------------------------

void MaterialPicker::slotPickModeChanged(const std::string& _mode) {
  pickMaterialButton_->setChecked( _mode == pickModeName_ && pickMaterial_ );
  fillMaterialButton_->setChecked( _mode == pickModeName_ && fillMaterial_ );
}

Q_EXPORT_PLUGIN2( materialPicker , MaterialPicker );
