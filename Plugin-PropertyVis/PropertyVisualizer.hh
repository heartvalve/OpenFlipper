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

#ifndef PROPERTY_VISUALIZER_HH
#define PROPERTY_VISUALIZER_HH

#include "Utils.hh"

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#include "OpenMesh/Core/Geometry/VectorT.hh"

#ifdef ENABLE_SKELETON_SUPPORT
    #include <ObjectTypes/Skeleton/BaseSkin.hh>
#endif

#include <stdexcept>

class VizException : public std::logic_error {
    public:
        VizException(const std::string &msg) : std::logic_error(msg) {}
};


/*! \class PropertyVisualizer
 *  \brief This class vizualizes a property.
 *
 * For each property one object of this class is created. It is responsible for
 * the visualization of that property. It provides a widget that can be used
 * to set options for the visualization.
 *
 */
class PropertyVisualizer: public QObject
{
    Q_OBJECT

signals:
    void log(Logtype _type, QString _message);
    void log(QString _message);

public:
    /**
     * @brief Constructor
     *
     * @param _propertyInfo Information about the property this visualizer should visualize.
     */
    PropertyVisualizer(PropertyInfo _propertyInfo)
        :
          propertyInfo(_propertyInfo),
          widget(0)
    {
    }

    /// Destructor
    virtual ~PropertyVisualizer(){delete widget; }

    /// Visualizes the property.
    virtual void visualize(bool _setDrawMode = true);

    /// Removes the property.
    virtual void removeProperty();

    /// Duplicates the property.
    virtual void duplicateProperty();

    /// Clears the property visualization.
    virtual void clear();

    /**
     * @brief Returns a beautiful name.
     *
     * The beautiful name consists of a symbol representing the entity type, the property's
     * name and the property's type.
     */
    virtual QString getName() { return propertyInfo.toString(); }

    /** @brief Returns the visualizer's widget.
     *
     * Returns the visualizer's widget that can be used to set some options
     * for the visualization.
     */
    virtual QWidget* getWidget() { return widget; }

    /// Returns the PropertyInfo.
    const PropertyInfo& getPropertyInfo() const { return propertyInfo; }

    /**
     * @brief Returns the value of a property in text form.
     * @param i The id of the entity whose property we want to know.
     * @return The property value in text form.
     */
    virtual QString getPropertyText(unsigned int i) = 0;


    /**
     * @brief Returns the value of a property in text form.
     * @param index The id of the entity whose property should be set.
     * @param text The value in text form.
     *
     * This method sets the porperty of a given entity by transforming
     * the text into the correct type. This is used when a property is
     * loaded from a file.
     */
    virtual void setPropertyFromText(unsigned int index, QString text) = 0;

    /// Returns the number of entities.
    virtual int getEntityCount() = 0;

    /// Returns the header for saving.
    virtual QString getHeader() = 0;


    static inline QString toStr(bool b)               { return b ? QObject::tr("True") : QObject::tr("False"); }
    static inline QString toStr(double d)             { return QObject::tr("%1").arg(d); }
    static inline QString toStr(int i)                { return QObject::tr("%1").arg(i); }
    static inline QString toStr(unsigned int i)       { return QObject::tr("%1").arg(i); }
    static        QString toStr(OpenMesh::Vec3d v);
    static        QString toStr(OpenMesh::Vec2d v);
    static        QString toStr(OpenMesh::Vec2f v);
#ifdef ENABLE_SKELETON_SUPPORT
    static        QString toStr(BaseSkin::SkinWeights sw);
#endif

    static inline bool            strToBool  (QString str) { return (str.compare(QObject::tr("True"))==0); }
    static inline double          strToDouble(QString str) { return str.toDouble() ;    }
    static inline int             strToInt   (QString str) { return str.toInt();        }
    static inline unsigned int    strToUInt  (QString str) { return str.toUInt();       }
    static        OpenMesh::Vec3d strToVec3d (QString str);
    static        OpenMesh::Vec2d strToVec2d (QString str);
    static        OpenMesh::Vec2f strToVec2f (QString str);


protected:

    PropertyInfo propertyInfo;
    QWidget* widget;

};

#endif /* PROPERTY_VISUALIZER_HH */
