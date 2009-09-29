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




#ifndef MEANCURVATUREPLUGIN_HH
#define MEANCURVATUREPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/TextureInterface.hh>
#include <OpenFlipper/common/Types.hh>

class MeanCurvaturePlugin : public QObject, BaseInterface, TextureInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(TextureInterface)

  signals:

    void addTexture( QString _textureName , QString _filename , uint dimension );
    void updatedTextures( QString , int );
    void setTextureMode(QString _textureName ,QString _mode);

  private slots:
    void slotUpdateTexture( QString _textureName , int _identifier );

    void pluginsInitialized();

  public :

    ~MeanCurvaturePlugin() {};

    void exit(){};

    template< typename MeshT >
    void computeMeanCurvature(MeshT* _mesh);

    QString name() { return (QString("MeanCurvature")); };
    QString description( ) { return (QString("Generates Mean Curvature information")); };

  public slots:
    QString version() { return QString("1.0"); };
};

#endif //MEANCURVATUREPLUGIN_HH
