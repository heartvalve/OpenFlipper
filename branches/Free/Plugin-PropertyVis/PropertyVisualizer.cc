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

#include "PropertyVisualizer.hh"

#include <iostream>

#include <QStringList>

/*! \brief Visualizes the property
 *
 * This method should be implemented by a subclass that specializes on visualizing
 * a specific property type for a specific object type. This method here only
 * implements a warning which will tell the user that visualizing the property for
 * that object type is not implemented yet.
 */
void PropertyVisualizer::visualize(bool _setDrawMode)
{
    log(LOGERR, "Visualizing for this type of object not yet supported.");
}

/*! \brief Removes the property
 *
 * This method should be implemented by a subclass that specializes on removing
 * properties for a specific object type. This method here only implements a
 * warning which will tell the user that removing the property is not implemented
 * yet for that object type.
 */
void PropertyVisualizer::removeProperty()
{
    log(LOGERR, "Removing for this type of object not yet supported.");
}

/*! \brief Duplicates the property
 *
 * This method should be implemented by a subclass that specializes on duplicating
 * properties for a specific object type. This method here only implements a warning
 * which will tell the user that removing the property is not implemented yet
 * for that object type.
 */
void PropertyVisualizer::duplicateProperty()
{
    log(LOGERR, "Duplicating for this type of object not yet supported.");
}

/*! \brief Clears the property visualization
 *
 * This method should be implemented by a subclass that specializes on clearing
 * property visualizations for a specific object type. This method here only
 * implements a warning which will tell the user that clearing the property
 * visualitaion is not implemented yet for that object type.
 */
void PropertyVisualizer::clear()
{
    log(LOGERR, "Clearing for this type of object not yet supported.");
}

QString PropertyVisualizer::toStr(OpenMesh::Vec3d v)
{
    return QObject::tr("( %1, %2, %3 )").arg(v[0]).arg(v[1]).arg(v[2]);
}

#ifdef ENABLE_SKELETON_SUPPORT
QString PropertyVisualizer::toStr(BaseSkin::SkinWeights sw)
{
    QString text = "";
    for (std::map<unsigned int, double>::iterator it = sw.begin(); it != sw.end(); ++it) {
      text += "(";
      text += QString::number((*it).first);
      text += ",";
      text += QString::number((*it).second);
      text += ")\n";
    }
    return text;
}
#endif


OpenMesh::Vec3d PropertyVisualizer::strToVec3d (QString str)
{
    QString s = str;
    s.remove(0,2);
    s.chop(2);
    QStringList strList = s.split(QObject::tr(", "));
    return OpenMesh::Vec3d(strList[0].toDouble(),strList[1].toDouble(),strList[2].toDouble());
}
