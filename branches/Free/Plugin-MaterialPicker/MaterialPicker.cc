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
#include <ACG/QtWidgets/QtMaterialDialog.hh>
#include <sstream>

//------------------------------------------------------------------------------
MaterialPicker::MaterialPicker()
  :
  pickModeName_("MaterialPicker"),
  propName_(name()+QString("/Materials")),
  pickMaterialButton_(0),
  fillMaterialButton_(0),
  materialListWidget_(0),
  materialStrings_(),
  shortKeyRow_(),
  materialNode_(),
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
   materialStrings_ = OpenFlipperSettings().value(propName_, QStringList()).toStringList();
   for (int i = 0; i < materialStrings_.size(); ++i)
   {
     QStringList savedString = materialStrings_[i].split(";");
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
     stream << savedString[8].toStdString();
     stream >> materialInfo.key;

     if (materialInfo.key != Qt::Key_unknown)
       shortKeyRow_[materialInfo.key] = materialListWidget_->count();

     materialListWidget_->addItem( itemName(savedString[0],materialInfo.key) );
     materialList_.push_back(materialInfo);
   }

   //if material was saved, set first as current
   if (materialStrings_.size())
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
   connect(materialListWidget_->itemDelegate(), SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)),this,SLOT(saveNewName(QWidget*, QAbstractItemDelegate::EndEditHint)));
   connect(removeItemButton, SIGNAL(clicked()), this, SLOT(slotRemoveCurrentItem()));
   connect(materialListWidget_,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(createContextMenu(const QPoint&)));

   materialListWidget_->setContextMenuPolicy(Qt::CustomContextMenu);
   QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"material_picker.png");
   emit addToolbox( tr("Material Picker"), toolBox, toolIcon);
}
//------------------------------------------------------------------------------

void MaterialPicker::removeItem(QListWidgetItem* _item)
{
  unsigned index = materialListWidget_->row(_item);
  materialListWidget_->takeItem(index);
  materialList_.erase(materialList_.begin()+index);
  materialStrings_.erase(materialStrings_.begin()+index);
  if (materialStrings_.isEmpty())
    OpenFlipperSettings().remove(propName_);
  else
    OpenFlipperSettings().setValue(propName_, materialStrings_);
  fillMaterialButton_->setEnabled(materialListWidget_->count());

  //update hotkey table
  std::map<int,size_t>::iterator eraseIter = shortKeyRow_.end();
  for (std::map<int,size_t>::iterator iter = shortKeyRow_.begin(); iter != shortKeyRow_.end(); ++iter)
  {
    if (iter->second > index)
      --(iter->second);
    else if (iter->second == index)
      eraseIter = iter;
  }
  if (eraseIter != shortKeyRow_.end())
    shortKeyRow_.erase(eraseIter);

}

//------------------------------------------------------------------------------

void MaterialPicker::clearList() {
  materialListWidget_->clear();
  materialList_.clear();
  materialStrings_.clear();
  fillMaterialButton_->setEnabled(false);

  //setting value empty instead of removing will cause an error at start up
  OpenFlipperSettings().remove(propName_);
}

//------------------------------------------------------------------------------

void MaterialPicker::slotRemoveCurrentItem()
{
  if (!materialListWidget_->count())
    return;

  QMessageBox msgBox;
  QListWidgetItem* item = materialListWidget_->currentItem();
  msgBox.setText(tr("Remove ")+plainName(item->text(),materialListWidget_->currentRow())+tr("?"));
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Ok);
  int ret = msgBox.exec();

  if (ret == QMessageBox::Ok)
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
  for (unsigned i = 0; i < supportedKeys_; ++i)
    emit registerKey (Qt::Key_1+i, Qt::ControlModifier, QString(tr("Material %1")).arg(i+1), false);
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

            materialInfo.key = Qt::Key_unknown;
            if (shortKeyRow_.size() < supportedKeys_)
            {
              materialInfo.key = Qt::Key_1+shortKeyRow_.size();
              shortKeyRow_[materialInfo.key] = materialListWidget_->count();
            }

            // update list widget and material list
            QString name = QString("material id: %1").arg(material->id());
            materialListWidget_->addItem( itemName(name,materialInfo.key) );
            materialListWidget_->setCurrentItem( materialListWidget_->item(materialListWidget_->count() - 1) );

            materialList_.push_back(materialInfo);

            //save material
            QString matStr = materialString(materialInfo,name);
            materialStrings_.push_back(matStr);
            OpenFlipperSettings().setValue(propName_, materialStrings_);

            fillMaterialButton_->setEnabled(true);
            OpenFlipperSettings().setValue(propName_, materialStrings_);
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

void MaterialPicker::editModeCurrent()
{
  editMode(materialListWidget_->currentItem());
}

//------------------------------------------------------------------------------
void MaterialPicker::editMode(QListWidgetItem* _item) {
  _item->setFlags(_item->flags() | Qt::ItemIsEditable);
  materialListWidget_->editItem(_item);
  _item->setText( plainName(_item->text(),materialListWidget_->row(_item)));
}

//------------------------------------------------------------------------------
void MaterialPicker::saveNewName ( QWidget * _editor, QAbstractItemDelegate::EndEditHint _hint )
{
  saveNewName(materialListWidget_->currentItem());
}
//------------------------------------------------------------------------------
QString MaterialPicker::plainName(const QString &string, int index)
{
  if (materialList_[index].key == Qt::Key_unknown)
    return string;

  QString str(string);
  return str.remove(0,4);
}
//------------------------------------------------------------------------------
void MaterialPicker::saveNewName (QListWidgetItem* _item)
{
  unsigned index = materialListWidget_->row(_item);
  QString str = materialStrings_[index];
  QStringList strList = str.split(";");

  //pass name
  strList[0] = _item->text();
  //highlight hotkey support
  if (materialList_[index].key != Qt::Key_unknown)
    _item->setText( itemName(strList[0], materialList_[index].key) );


  //create new String to save
  str = "";
  for (int i = 0; i < strList.size()-1; ++i)
    str += strList[i] + ";";
  str += strList[strList.size()-1];
  materialStrings_[index] = str;
  OpenFlipperSettings().setValue(propName_, materialStrings_);
}
//------------------------------------------------------------------------------

QString MaterialPicker::itemName(const QString &_name, int _key)
{
  if (_key == Qt::Key_unknown)
    return _name;
  return QString(tr("(%1) ")).arg(QString::number(_key-Qt::Key_1+1)) +_name;
}

//------------------------------------------------------------------------------

void MaterialPicker::slotPickModeChanged(const std::string& _mode) {
  pickMaterialButton_->setChecked( _mode == pickModeName_ && pickMaterial_ );
  fillMaterialButton_->setChecked( _mode == pickModeName_ && fillMaterial_ );
}

//------------------------------------------------------------------------------
void MaterialPicker::slotKeyEvent(QKeyEvent* _event)
{
  for (unsigned i = 0; i < supportedKeys_; ++i)
  {
    int key = Qt::Key_1+i;
    if (_event->key() == key && _event->modifiers() == Qt::ControlModifier)
    {
      if (shortKeyRow_.find(key) == shortKeyRow_.end())
        return;
      slotFillMaterialMode();
      materialListWidget_->setCurrentRow(shortKeyRow_[key]);
    }
  }
}
//------------------------------------------------------------------------------
void MaterialPicker::changeHotKey(const int &_key)
{
  std::map<int,size_t>::iterator iter = shortKeyRow_.find(_key);

  if (iter != shortKeyRow_.end())
  {
    //remove old key
    size_t oldIndex = iter->second;
    QListWidgetItem* oldItem = materialListWidget_->item(oldIndex);
    //remove name
    oldItem->setText( plainName(oldItem->text(),oldIndex) );
    materialList_[oldIndex].key = Qt::Key_unknown; //unregister key after rename, otherwise the renaming will fail
    materialStrings_[oldIndex] = materialString(materialList_[oldIndex],oldItem->text());
    saveNewName(oldItem);
  }

  //set the new item (save and hint)
  size_t newIndex = materialListWidget_->currentRow();
  QListWidgetItem* newItem = materialListWidget_->item(newIndex);
  materialList_[newIndex].key = _key;

  materialStrings_[newIndex] = materialString(materialList_[newIndex],newItem->text());
  saveNewName(newItem);

  shortKeyRow_[_key] = newIndex;
}
//------------------------------------------------------------------------------
QString MaterialPicker::materialString(const MaterialInfo& _mat, const QString &_name)
{
  std::stringstream stream;
  stream << _name.toStdString();
  stream << ";" << _mat.color_material;
  stream << ";" << _mat.base_color;
  stream << ";" << _mat.ambient_color;
  stream << ";" << _mat.diffuse_color;
  stream << ";" << _mat.specular_color;
  stream << ";" << _mat.shininess;
  stream << ";" << _mat.reflectance;
  stream << ";" << _mat.key;

  return QString(stream.str().c_str());
}
//------------------------------------------------------------------------------
void MaterialPicker::slotMaterialProperties()
{
  if (materialNode_)
    return;

  //QListWidgetItem* item = materialListWidget_->currentItem();
  materialListWidget_->setDisabled(true);

  materialNode_.reset(new MaterialNode());
  int row = materialListWidget_->currentRow();
  materialNode_->colorMaterial(materialList_[row].color_material);
  materialNode_->set_base_color(materialList_[row].base_color);
  materialNode_->set_ambient_color(materialList_[row].ambient_color);
  materialNode_->set_diffuse_color(materialList_[row].diffuse_color);
  materialNode_->set_specular_color(materialList_[row].specular_color);
  materialNode_->set_shininess(materialList_[row].shininess);
  materialNode_->set_reflectance(materialList_[row].reflectance);

  ACG::QtWidgets::QtMaterialDialog* dialog = new ACG::QtWidgets::QtMaterialDialog( 0, materialNode_.get() );

  dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowStaysOnTopHint);

  connect(dialog,SIGNAL(finished(int)),this,SLOT(slotEnableListWidget(int)));
  connect(dialog,SIGNAL(accepted()),this,SLOT(slotMaterialChanged()));

  dialog->setWindowIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-material.png"));

  dialog->show();

}
//------------------------------------------------------------------------------
void MaterialPicker::slotMaterialChanged()
{
  if (materialNode_)
    {
      int index = materialListWidget_->currentRow();
      // store the material information
      MaterialInfo materialInfo;
      materialInfo.color_material = materialNode_->colorMaterial();
      materialInfo.base_color = materialNode_->base_color();
      materialInfo.ambient_color = materialNode_->ambient_color();
      materialInfo.diffuse_color = materialNode_->diffuse_color();
      materialInfo.specular_color = materialNode_->specular_color();
      materialInfo.shininess = materialNode_->shininess();
      materialInfo.reflectance = materialNode_->reflectance();
      materialInfo.key = materialList_[index].key;
      QString name = plainName(materialListWidget_->currentItem()->text(),materialListWidget_->currentRow());
      materialStrings_[index] = materialString(materialInfo,name);
      materialList_[index] = materialInfo;
      OpenFlipperSettings().setValue(propName_, materialStrings_);
    }
  OpenFlipperSettings().setValue(propName_, materialStrings_);
}

//------------------------------------------------------------------------------
void MaterialPicker::slotEnableListWidget(int _save){
  materialListWidget_->setEnabled(true);
  if (_save == QDialog::Accepted)
    slotMaterialChanged();
  materialNode_.reset();
}

//------------------------------------------------------------------------------
void MaterialPicker::createContextMenu(const QPoint& _point)
{
  QMenu *menu = new QMenu(materialListWidget_);

  QAction* action = menu->addAction(tr("Material Properties"));
  QIcon icon;
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"datacontrol-material.png");
  action->setIcon(icon);
  action->setEnabled(true);
  connect(action,SIGNAL(triggered(bool)),this,SLOT(slotMaterialProperties()));

  action = menu->addAction(tr("Rename"));
  connect(action,SIGNAL(triggered(bool)),this,SLOT(editModeCurrent()));

  action = menu->addAction(tr("Remove"));
  connect(action, SIGNAL(triggered(bool)),this,SLOT(slotRemoveCurrentItem()));

  menu->addSeparator();

  //add hotkey selectors
  QSignalMapper* signalMapper = new QSignalMapper(menu);
  for (unsigned i = 0; i < supportedKeys_; ++i)
  {
    QAction* action = menu->addAction(tr("Key %1").arg(i+1));
    connect(action,SIGNAL(triggered(bool)),signalMapper,SLOT(map()));
    signalMapper->setMapping(action,Qt::Key_1+i);
    std::map<int,size_t>::iterator iter = shortKeyRow_.find(Qt::Key_1+i);

    //Disable already selected hotkey number
    if (iter != shortKeyRow_.end() && iter->second == static_cast<size_t>(materialListWidget_->currentRow()))
      action->setDisabled(true);
  }

  connect(signalMapper, SIGNAL(mapped(const int &)),this, SLOT(changeHotKey(const int &)));
  menu->exec(materialListWidget_->mapToGlobal(_point),0);


}
Q_EXPORT_PLUGIN2( materialPicker , MaterialPicker );
