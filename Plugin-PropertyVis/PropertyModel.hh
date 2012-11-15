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

#ifndef PROPERTY_MODEL_H
#define PROPERTY_MODEL_H

#include <QAbstractListModel>
#include "PropertyVisualizer.hh"

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>

#include <vector>
#include <iostream>
#include <set>
#include <typeinfo>

#include <QPushButton>

#include <Widgets/VectorWidget.hh>


/*! \class PropertyModel
 *  \brief This class manages the visualizers for one object.
 *
 * For each mesh object a PropertyModel is created. It searches for properties and creates a
 * PropertyVisualizer for each of them. It provides the GUI for all selected properties.
 * Also loading and saving of properties is handled here.
 *
 * Note that for each type of object (OpenMesh, OpenVolumeMesh and others that might follow)
 * a subclass should be derived from this class.
 */
class PropertyModel: public QAbstractListModel
{
    Q_OBJECT

signals:
    void log(Logtype _type, QString _message);
    void log(QString _message);

private slots:
    void slotLog(Logtype _type, QString _message){ emit log(_type, _message); }
    void slotLog(QString _message){ emit log(_message);}

public:
    virtual void pickModeChanged(const std::string& _mode) {}
    virtual void mouseEvent(QMouseEvent* _event) {}

    PropertyModel(QObject *parent = 0);
    virtual ~PropertyModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /// Visualizes the selected properties.
    virtual void visualize(QModelIndexList selectedIndices);

    /// Removes the selected properties.
    virtual void removeProperty(QModelIndexList selectedIndices);

    /// Duplicates the selected properties.
    virtual void duplicateProperty(QModelIndexList selectedIndices);

    /// Searches for properties and creates PropertyVisualizers.
    virtual void gatherProperties(){ /* implemented by subclass */ }

    /// Clears the selected property visualization.
    virtual void clear(QModelIndexList selectedIndices);

    /// Hides the widget.
    inline void hideWidget()    { widgets->hide(); }

    /// Returns the widget.
    inline QWidget* getWidget() { return widgets;  }

    /// Updates the widget
    virtual void updateWidget(const QModelIndexList& selectedIndices);

    /// Connects the PropertyVisualizer log signals with the log slot.
    void connectLogs(PropertyVisualizer* propViz);


protected:

    /// Returns a PropertyVisualizer.
    PropertyVisualizer* getPropertyVisualizer(QString propName, PropertyInfo::ENTITY_FILTER filter, TypeInfoWrapper typeInfo);

    /// Checks if the property name is still available.
    bool isPropertyFree(QString propName, PropertyInfo::ENTITY_FILTER filter, TypeInfoWrapper typeInfo);


    /// Asks the user for a file to load.
    QString getLoadFilename();

    /// Returns the filename filter for loading.
    virtual QString getLoadFilenameFilter();

    /// Asks the user for a file to load.
    QString getSaveFilename(unsigned int propId);

    /// Returns the filename filter for saving.
    virtual QString getSaveFilenameFilter(unsigned int propId);

    /// Returns the default file name
    virtual QString getDefaultSaveFilename(unsigned int propId);

    /// Opens a file.
    void openFile(QString _filename, QFile &file_, QTextStream *&file_stream_);

    /// Closes a file.
    void closeFile(QFile& file_, QTextStream*& file_stream_);

    /// Read line from a file.
    QString readLine(QTextStream *file_stream_);

    /// Writes a line to a file.
    void writeLine(QString _s, QTextStream *&file_stream_);


    /// Saves property.
    void saveProperty(unsigned int propId);

    /// Loads property.
    void loadProperty();

    /// Sets the property values from a given file.
    virtual void setPropertyFromFile(QTextStream*& file_stream_, unsigned int n, PropertyVisualizer *propVis);

    /**
     * @brief Parses the property file header.
     *
     * @param header The header.
     * @param[out] propVis The PropertyVisualizer that will visualize the new property.
     * @param[out] n The number of values stored in the file
     * @return True if parsing was successfull, False if not.
     *
     * When loading a property from a file this method parses the header placed in the file's first line. If parsing
     * was successfull a new property and a PropertyVisualizer will be created.
     */
    virtual bool parseHeader(QString header, PropertyVisualizer*& propVis, unsigned int& n){ return false; }

protected:
    std::vector<PropertyVisualizer*> propertyVisualizers;
    QWidget* widgets;

    QModelIndexList currentIndices;
};

#endif /* PROPERTY_MODEL_H */
