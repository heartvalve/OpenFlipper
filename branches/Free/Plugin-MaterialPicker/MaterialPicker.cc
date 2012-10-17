/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2012 by Computer Graphics Group, RWTH Aachen       *
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


#include "MaterialPicker.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <sstream>

//------------------------------------------------------------------------------
MaterialPicker::MaterialPicker()
  :
  pickModeName_("MaterialPicker"),
  propName_("MaterialPickerPlugin/Materials"),
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

   pickMaterialButton_ = new QPushButton("&pick Material", toolBox);
   fillMaterialButton_ = new QPushButton("&fill Material", toolBox);
   QPushButton* clearListButton =  new QPushButton("Clear List", toolBox);
   QPushButton* removeItemButton = new QPushButton("Remove", toolBox);

   pickMaterialButton_->setCheckable(true);
   fillMaterialButton_->setCheckable(true);

   QLabel* materials = new QLabel("Materials:");

   materialListWidget_ = new QListWidget(toolBox);

   //load saved materials
   materialString_ = OpenFlipperSettings().value(propName_, QStringList()).toStringList();

   for (int i = 0; i < materialString_.size(); ++i)
   {
     QStringList savedString = materialString_[i].split(";");
     std::stringstream stream;
     MaterialInfo materialInfo;
     stream << savedString[1].toStdString();
     stream >> materialInfo.color_material;
     stream.str("");
     stream.clear();
     stream << savedString[2].toStdString();
     stream >> materialInfo.base_color;
     stream.str("");
     stream.clear();
     stream << savedString[3].toStdString();
     stream >> materialInfo.ambient_color;
     stream.str("");
     stream.clear();
     stream << savedString[4].toStdString();
     stream >> materialInfo.diffuse_color;
     stream.str("");
     stream.clear();
     stream << savedString[5].toStdString();
     stream >> materialInfo.specular_color;
     stream.str("");
     stream.clear();
     stream << savedString[6].toStdString();
     stream >> materialInfo.shininess;
     stream.str("");
     stream.clear();
     stream << savedString[7].toStdString();
     stream >> materialInfo.reflectance;
     stream.str("");
     stream.clear();
     int key;
     if (savedString.size() < 9)
       savedString.push_back(QString::number(Qt::Key_unknown));
     stream << savedString[8].toStdString();
     stream >> key;

     if (key != Qt::Key_unknown)
       shortKeyRow_[key] = materialListWidget_->count();

     materialListWidget_->addItem( savedString[0] );
     materialList_.push_back(materialInfo);
   }

   //if material was saved, set first as current
   if (materialString_.size())
     materialListWidget_->setCurrentItem(materialListWidget_->item(0));
   else
     fillMaterialButton_->setEnabled(false);

   QGridLayout* removeGrid = new QGridLayout();
   removeGrid->addWidget(removeItemButton,0,0);
   removeGrid->addWidget(clearListButton,0,1);

   QGridLayout* pickGrid = new QGridLayout();
   pickGrid->addWidget(pickMaterialButton_, 0, 0);
   pickGrid->addWidget(fillMaterialButton_, 0, 1);

   QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, toolBox);
   layout->addWidget(materials);
   layout->addWidget(materialListWidget_);

   layout->addLayout(removeGrid);
   layout->addLayout(pickGrid);

   connect(pickMaterialButton_, SIGNAL(clicked()), this, SLOT(slotPickMaterialMode()));
   connect(fillMaterialButton_, SIGNAL(clicked()), this, SLOT(slotFillMaterialMode()));
   connect(clearListButton, SIGNAL(clicked()), this, SLOT(clearList()));
   connect(materialListWidget_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editMode(QListWidgetItem*)));
   connect(materialListWidget_,SIGNAL(itemChanged ( QListWidgetItem*)), this, SLOT(saveNewName(QListWidgetItem*)));
   connect(removeItemButton, SIGNAL(clicked()), this, SLOT(slotRemoveCurrentItem()));

   QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"material_picker.png");
   emit addToolbox( tr("Material Picker"), toolBox, toolIcon);
}
//------------------------------------------------------------------------------

void MaterialPicker::removeItem(QListWidgetItem* _item)
{
  unsigned index = materialListWidget_->row(_item);
  materialListWidget_->takeItem(index);
  materialList_.erase(materialList_.begin()+index);
  materialString_.erase(materialString_.begin()+index);
  OpenFlipperSettings().setValue(propName_, materialString_);
  fillMaterialButton_->setEnabled(materialListWidget_->count());
}

//------------------------------------------------------------------------------

void MaterialPicker::clearList() {
  materialListWidget_->clear();
  materialList_.clear();
  materialString_.clear();
  OpenFlipperSettings().setValue(propName_, materialString_);
  fillMaterialButton_->setEnabled(false);
}

//------------------------------------------------------------------------------

void MaterialPicker::slotRemoveCurrentItem()
{
  if (materialListWidget_->count())
    removeItem(materialListWidget_->currentItem());
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
  emit registerKey (Qt::Key_1, Qt::ControlModifier, tr("Material 1"), false);
  emit registerKey (Qt::Key_2, Qt::ControlModifier, tr("Material 2"), false);
  emit registerKey (Qt::Key_3, Qt::ControlModifier, tr("Material 3"), false);
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

            int key = Qt::Key_unknown;
            if (shortKeyRow_.size() < 3)
            {
              shortKeyRow_[Qt::Key_1+shortKeyRow_.size()] = materialListWidget_->count();
              key = Qt::Key_1+shortKeyRow_.size();
            }

            // update list widget and material list
            QString name = QString("material id: %1").arg(material->id());
            materialListWidget_->addItem( name );
            materialListWidget_->setCurrentItem( materialListWidget_->item(materialListWidget_->count() - 1) );

            materialList_.push_back(materialInfo);

            //save new material
            std::stringstream stream;
            stream << name.toStdString();
            stream << ";" << materialInfo.color_material;
            stream << ";" << materialInfo.base_color;
            stream << ";" << materialInfo.ambient_color;
            stream << ";" << materialInfo.diffuse_color;
            stream << ";" << materialInfo.specular_color;
            stream << ";" << materialInfo.shininess;
            stream << ";" << materialInfo.reflectance;
            stream << ";" << key;

            QString materialString = stream.str().c_str();
            materialString_.push_back(materialString);
            fillMaterialButton_->setEnabled(true);
            OpenFlipperSettings().setValue(propName_, materialString_);
          }

        // apply material from current item in list widget to picked object
        } else if ( fillMaterial_ && !pickMaterial_ ){
          MaterialNode* material = object->materialNode();

          if (material) {

            if (materialListWidget_->count() > 0)
            {
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
}

//------------------------------------------------------------------------------

void MaterialPicker::editMode(QListWidgetItem* _item) {
  _item->setFlags(_item->flags() | Qt::ItemIsEditable);
  materialListWidget_->editItem(_item);
}

//------------------------------------------------------------------------------
void MaterialPicker::saveNewName(QListWidgetItem* _item)
{
  unsigned index = materialListWidget_->row(_item);
  QString str = materialString_[index];
  QStringList strList = str.split(";");

  //pass name
  strList[0] = _item->text();

  //create new String to save
  str = "";
  for (int i = 0; i < strList.size()-1; ++i)
    str += strList[i] + ";";
  str += strList[strList.size()-1];
  materialString_[index] = str;
  OpenFlipperSettings().setValue(propName_, materialString_);

}
//------------------------------------------------------------------------------

void MaterialPicker::slotPickModeChanged(const std::string& _mode) {
  pickMaterialButton_->setChecked( _mode == pickModeName_ && pickMaterial_ );
  fillMaterialButton_->setChecked( _mode == pickModeName_ && fillMaterial_ );
}

void MaterialPicker::slotKeyEvent(QKeyEvent* _event)
{
  if (_event->key() == Qt::Key_1 && _event->modifiers() == Qt::ControlModifier)
  {
    if (shortKeyRow_.find(Qt::Key_1) == shortKeyRow_.end())
      return;
    slotFillMaterialMode();
    materialListWidget_->setCurrentRow(shortKeyRow_[Qt::Key_1]);
  }
  if (_event->key() == Qt::Key_2 && _event->modifiers() == Qt::ControlModifier)
  {
    if (shortKeyRow_.find(Qt::Key_2) == shortKeyRow_.end())
      return;
    slotFillMaterialMode();
    materialListWidget_->setCurrentRow(shortKeyRow_[Qt::Key_2]);
  }
  if (_event->key() == Qt::Key_3 && _event->modifiers() == Qt::ControlModifier)
  {
    if (shortKeyRow_.find(Qt::Key_3) == shortKeyRow_.end())
      return;
    slotFillMaterialMode();
    materialListWidget_->setCurrentRow(shortKeyRow_[Qt::Key_3]);
  }
}
Q_EXPORT_PLUGIN2( materialPicker , MaterialPicker );
