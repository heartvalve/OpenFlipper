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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/
#include <QObject>
#include <QString>

#include <ACG/Scenegraph/LineNode.hh>
#include <ACG/Scenegraph/TextNode.hh>
#include <OpenFlipper/common/BaseObjectData.hh>

class Ruler : public QObject
{
  Q_OBJECT
signals:
  void updateView();

public:
  /**
   * creates a new ruler on a given object
   * @param _obj Object where additional nodes are added
   * @param _pluginName name of the plugin
   * @param _index the current index of the ruler. If you create multiple rulers, you can use this parameter to avoid identical nodenames
   */
  Ruler(BaseObjectData *_obj,const QString &_pluginName, unsigned _index);
  ~Ruler();

  void setPoints(const ACG::Vec3d& _start,const ACG::Vec3d& _end);
  void setStartPoint(const ACG::Vec3d& _start);
  void setEndPoint(const ACG::Vec3d& _end);

  const ACG::Vec3d* points() const{return points_;}

  const BaseObject* getBaseObj(){return obj_;}

private:

  void updateNodes();

  const QString pluginName_;
  const std::string lineNodeName_;
  const std::string textNodeName_;
  const std::string textTransformNodeName_;

  ACG::SceneGraph::LineNode* lineNode_;
  ACG::SceneGraph::TextNode* textNode_;
  ACG::SceneGraph::TransformNode* textTransformNode_;

  ACG::Vec3d points_[2];
  BaseObjectData* obj_;
};
