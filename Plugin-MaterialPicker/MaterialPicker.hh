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
*   $Revision: 14388 $                                                       *
*   $LastChangedBy: moeller $                                                *
*   $Date: 2012-04-16 12:55:37 +0200 (Mon, 16 Apr 2012) $                     *
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

#include <OpenFlipper/common/Types.hh>

struct MaterialInfo
{
   bool color_material;
   ACG::Vec4f base_color;
   ACG::Vec4f ambient_color;
   ACG::Vec4f diffuse_color;
   ACG::Vec4f specular_color;
   float shininess;
   double reflectance;
};

class MaterialPicker : public QObject, BaseInterface, MouseInterface, PickingInterface, ToolboxInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(ToolboxInterface)

signals:
  void addPickMode(const std::string &_mode);

  void updateView();

  void addToolbox(QString _name, QWidget* _widget);

public slots:

  void slotMouseEvent(QMouseEvent* _event);

  void slotPickModeChanged(const std::string& _mode);

public:

  MaterialPicker();
  ~MaterialPicker();

  QString name(){return QString("MaterialPicker");}
  QString description(){return QString("Pick to store and set materials.");}

private:

  const std::string pickModeName_;
  const QString propName_;

  QPushButton* pickMaterialButton_;
  QPushButton* fillMaterialButton_;
  QListWidget* materialListWidget_;
  QStringList materials_;

  QVector<MaterialInfo> materialList_;

  /// stores the state of the pick material button
  bool pickMaterial_;
  /// stores the state of the fill material button
  bool fillMaterial_;

private slots:

  void initializePlugin();

  void pluginsInitialized();

  void slotPickMaterialMode();

  void slotFillMaterialMode();

  /// items can be renamed by double clicking them
  void editMode(QListWidgetItem* _item);

  /// saves the new material name
  void assignNewName(QListWidgetItem* _item);

  void clearList();

public slots:
  QString version(){ return QString("1.0"); }
};


#endif //MATERIALPICKER_HH
