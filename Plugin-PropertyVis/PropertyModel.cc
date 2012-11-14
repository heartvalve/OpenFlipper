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

#include "PropertyModel.hh"

PropertyModel::PropertyModel(QObject *parent)
    : QAbstractListModel(parent),
      widgets(0),
      currentIndices()
{
    gatherProperties();
    QVBoxLayout* layout = new QVBoxLayout();
    widgets = new QWidget();
    widgets->setLayout(layout);
}

PropertyModel::~PropertyModel() {
}

/**
 * @brief Visualizes the selected properties.
 * @param selectedIndices The
 */
void PropertyModel::visualize(QModelIndexList selectedIndices) {
    for (QModelIndexList::Iterator it = selectedIndices.begin(); it != selectedIndices.end(); ++it)
    {
        propertyVisualizers[it->row()]->visualize();
    }
}

void PropertyModel::removeProperty(QModelIndexList selectedIndices)
{
    std::vector<unsigned int> deleteIndices;
    for (QModelIndexList::Iterator it = selectedIndices.begin(); it != selectedIndices.end(); ++it)
    {
        propertyVisualizers[it->row()]->removeProperty();
        QWidget* w = propertyVisualizers[it->row()]->getWidget();
        widgets->layout()->removeWidget(w);
        delete propertyVisualizers[it->row()];
        deleteIndices.push_back(it->row());
    }
    for (std::vector<unsigned int>::reverse_iterator rit = deleteIndices.rbegin(); rit != deleteIndices.rend(); rit++){
        propertyVisualizers.erase(propertyVisualizers.begin() + *rit);
    }
}

void PropertyModel::duplicateProperty(QModelIndexList selectedIndices)
{
    for (QModelIndexList::Iterator it = selectedIndices.begin(); it != selectedIndices.end(); ++it)
        propertyVisualizers[it->row()]->duplicateProperty();
}

void PropertyModel::clear(QModelIndexList selectedIndices) {
    for (QModelIndexList::Iterator it = selectedIndices.begin(); it != selectedIndices.end(); ++it)
    {
        propertyVisualizers[it->row()]->clear();
    }
}

void PropertyModel::updateWidget(const QModelIndexList& selectedIndices)
{
    QLayout* layout = widgets->layout();

    currentIndices = selectedIndices;

    for (unsigned int i = 0; i < propertyVisualizers.size(); i++)
    {
        propertyVisualizers[i]->getWidget()->hide();
    }

    for (QModelIndexList::const_iterator it = selectedIndices.begin(), it_end = selectedIndices.end();
                it != it_end; ++it) {
        QWidget* widget = propertyVisualizers[it->row()]->getWidget();
        layout->addWidget(widget);
        widget->show();
    }
    widgets->setLayout(layout);
}

void PropertyModel::connectLogs(PropertyVisualizer* propViz)
{
    connect(propViz, SIGNAL(log(QString)), this, SLOT(slotLog(QString)));
    connect(propViz, SIGNAL(log(Logtype, QString)), this, SLOT(slotLog(Logtype, QString)));
}

int PropertyModel::rowCount(const QModelIndex & parent) const {
    return propertyVisualizers.size();
}

QVariant PropertyModel::data(const QModelIndex & index, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            return propertyVisualizers[index.row()]->getName();
        default:
            return QVariant::Invalid;
    }
}

QVariant PropertyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            return tr("Some header. %1 %2").arg(section).arg(orientation);
            break;
        default:
            return QAbstractListModel::headerData(section, orientation, role);
    }
}


QString PropertyModel::getLoadFilename()
{
  QString filter = getLoadFilenameFilter();

  QString fileName = QFileDialog::getOpenFileName(0, tr("Load Property"), QString(), filter);

  filter = tr("HalfEdge Property (*.hprop);; All Files (*.*)");
  return fileName;
}

QString PropertyModel::getLoadFilenameFilter()
{
    return tr("");
}

QString PropertyModel::getSaveFilename(unsigned int propId)
{
  QString filter(getSaveFilenameFilter(propId));
  QString defaultName = getDefaultSaveFilename(propId);

  QString fileName = QFileDialog::getSaveFileName(0, tr("Save Property"), defaultName, filter);

  return fileName;
}

QString PropertyModel::getSaveFilenameFilter(unsigned int propId)
{
    QString filter= tr("All Files (*)");

    return filter;
}

QString PropertyModel::getDefaultSaveFilename(unsigned int propId)
{
    PropertyVisualizer* propViz = propertyVisualizers[propId];

    QString name = tr(propViz->getPropertyInfo().propName().c_str());

    if (propViz->getPropertyInfo().isVertexProp())
        name += tr(".vprop");
    else if (propViz->getPropertyInfo().isHalfedgeProp())
        name += tr(".hprop");
    else if (propViz->getPropertyInfo().isEdgeProp())
        name += tr(".eprop");
    else if (propViz->getPropertyInfo().isFaceProp())
        name += tr(".fprop");
    else if (propViz->getPropertyInfo().isHalffaceProp())
        name += tr(".hfprop");
    else if (propViz->getPropertyInfo().isCellProp())
        name += tr(".cprop");

    return name;
}

void PropertyModel::openFile(QString _filename, QFile& file_, QTextStream*& file_stream_)
{
  closeFile(file_, file_stream_);
  file_.setFileName(_filename);
  if(file_.open(QIODevice::ReadWrite | QIODevice::Text))
    file_stream_ = new QTextStream(&file_);
}

void PropertyModel::closeFile(QFile& file_, QTextStream*& file_stream_)
{
  if( file_stream_)
  {
    delete file_stream_;
    file_stream_ = 0;
  }
  if(file_.exists())
  {
      file_.close();
  }
}

QString PropertyModel::readLine(QTextStream* file_stream_)
{
    if(file_stream_)
    {
      QString s;
      s = file_stream_->readLine();
      return s;
    }
    else return QString("");
}

void PropertyModel::writeLine(QString _s, QTextStream*& file_stream_)
{
  if(file_stream_)
  {
    (*file_stream_) << _s << '\n';
  }
  else std::cerr << "Warning: filestream not available...\n";
}

void PropertyModel::saveProperty(unsigned int propId)
{
    PropertyVisualizer* propVis = propertyVisualizers[propId];

    QString filename = getSaveFilename(propId);
    if (filename == "") return;

    QFile file_;
    QTextStream* file_stream_ = 0;

    openFile(filename, file_, file_stream_);

    QString header = propVis->getHeader();
    writeLine(header, file_stream_);

    int n = propVis->getEntityCount();

    for (int i = 0; i < n; ++i)
    {
        QString propertyText = propVis->getPropertyText(i);
        writeLine(propertyText, file_stream_);
    }

    closeFile(file_, file_stream_);
}

void PropertyModel::loadProperty()
{

    QString filename = getLoadFilename();
    if (filename == "") return;

    QFile file_;
    QTextStream* file_stream_ = 0;

    openFile(filename, file_, file_stream_);

    QString header = readLine(file_stream_);

    PropertyVisualizer* propVis;
    unsigned int n;


    if (parseHeader(header, propVis, n))
    {
        setPropertyFromFile(file_stream_, n, propVis);
    }
    else
    {
        emit log("Property could not be loaded.");
    }

    closeFile(file_, file_stream_);
}

void PropertyModel::setPropertyFromFile(QTextStream*& file_stream_, unsigned int n, PropertyVisualizer* propVis)
{
    for (unsigned int i = 0; i < n; ++i)
    {
        QString propertyText = readLine(file_stream_);
        propVis->setPropertyFromText(i, propertyText);
    }
}

PropertyVisualizer* PropertyModel::getPropertyVisualizer(QString propName, PropertyInfo::ENTITY_FILTER filter, TypeInfoWrapper typeInfo)
{
    for (unsigned int i = 0; i < propertyVisualizers.size(); ++i)
    {
        const PropertyInfo& propInfo =  propertyVisualizers[i]->getPropertyInfo();
        if ((propInfo.entityType() == filter) && (QString::compare(tr(propInfo.propName().c_str()), propName)==0) && (propInfo.typeinfo() == typeInfo))
            return propertyVisualizers[i];
    }
    return 0;
}

bool PropertyModel::isPropertyFree(QString propName, PropertyInfo::ENTITY_FILTER filter, TypeInfoWrapper typeInfo)
{
    return getPropertyVisualizer(propName, filter, typeInfo) == 0;
}


