//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




#include "ui_textureProperties.hh"
#include "TextureData.hh"
#include <QtGui>

#ifdef WITH_QWT
  #include "QwtFunctionPlot.hh"
#endif

class texturePropertiesWidget : public QDialog, public Ui::Dialog
{
  Q_OBJECT

  signals:
    void applyProperties(TextureData* _texData, QString _textureName, int _id);

    void getCoordinates1D(QString _textureName, int _id, std::vector< double >& _x );

  public:
    texturePropertiesWidget(QWidget *parent = 0);

    void show(TextureData* _texData, int _id, QString _name = "");

  private slots:

    void textureAboutToChange(QTreeWidgetItem* _item , int _column);

    void textureChanged(QTreeWidgetItem* _item , int _column);

    void slotButtonBoxClicked(QAbstractButton* _button);

    void slotPropertiesChanged(double _value = 0.0);

    void slotChangeImage();

  private:

    bool propChanged_;
    QTreeWidgetItem* curItem_;
    QString currentImage_;
    QImage image_;

    TextureData* texData_;
    QString      textureName_;
    int          id_;

#ifdef WITH_QWT
    ACG::QwtFunctionPlot* functionPlot_;
#endif

};

