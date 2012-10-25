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

#ifndef MATERIALPICKER_HH
#define MATERIALPICKER_HH

#include <QObject>
#include <QString>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>

#include <OpenFlipper/common/Types.hh>
#include <ACG/Utils/SmartPointer.hh>


class MaterialPicker : public QObject, BaseInterface, MouseInterface, PickingInterface, ToolboxInterface, KeyInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(KeyInterface)

signals:
  void addPickMode(const std::string &_mode);

  void updateView();

  void addToolbox( QString  _name  , QWidget* _widget, QIcon* _icon);

  void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse = false);

public slots:

  void slotMouseEvent(QMouseEvent* _event);

  void slotPickModeChanged(const std::string& _mode);

  void slotKeyEvent (QKeyEvent* _event);

public:

  MaterialPicker();
  ~MaterialPicker();

  QString name(){return QString("MaterialPicker");}
  QString description(){return QString("Pick to store and set materials.");}

private:

  struct MaterialInfo
  {
     bool color_material;
     ACG::Vec4f base_color;
     ACG::Vec4f ambient_color;
     ACG::Vec4f diffuse_color;
     ACG::Vec4f specular_color;
     float shininess;
     double reflectance;
     int key;
  };

  const std::string pickModeName_;
  const QString propName_;
  static const unsigned supportedKeys_ = 3;

  QPushButton* pickMaterialButton_;
  QPushButton* fillMaterialButton_;

  //all these lists are synchronized:
  QListWidget* materialListWidget_;
  QVector<MaterialInfo> materialList_;
  QStringList materialStrings_;//hold materials as a String (saves/load the material at the beginning)

  //key = hotKey, value = index for lists
  std::map<int,size_t> shortKeyRow_;

  ptr::shared_ptr<MaterialNode> materialNode_;

  /// stores the state of the pick material button
  bool pickMaterial_;
  /// stores the state of the fill material button
  bool fillMaterial_;

private:
  QString itemName(const QString &_name, int _key);

private slots:

  void initializePlugin();

  void pluginsInitialized();

  void slotPickMaterialMode();

  void slotFillMaterialMode();

  void slotRemoveCurrentItem();

  /// items can be renamed by double clicking them
  void editMode(QListWidgetItem* _item);

  void editModeCurrent();

  /// saves the new material name with hotkey hint
  void saveNewName ( QWidget * _editor, QAbstractItemDelegate::EndEditHint _hint );
  void saveNewName (QListWidgetItem* _item);

  /// returns the plain name of the material without hotkey hint
  QString plainName(const QString &string, int index);

  /// returns a formatted string for saving
  QString materialString(const MaterialInfo& _mat, const QString &_name);

  void clearList();

  void removeItem(QListWidgetItem* _item);

  /// creates context menu on current item (current is the item at mouse position)
  void createContextMenu(const QPoint& _point);

  /// change specified HotKey to current item
  void changeHotKey(const int &_key);

  void slotMaterialProperties();

  void slotEnableListWidget(int _save);

  void slotMaterialChanged();

public slots:
  QString version(){ return QString("1.0"); }
};


#endif //MATERIALPICKER_HH
