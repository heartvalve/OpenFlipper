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

    // Texture Interface
    void addTexture( QString _textureName , QString _filename , uint dimension );
    void updatedTextures( QString , int );
    void setTextureMode(QString _textureName ,QString _mode);

    // Base Interface
    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

  private slots:
    void slotUpdateTexture( QString _textureName , int _identifier );

    void pluginsInitialized();

  public slots:

    /** \brief Scripting slot to trigger computation of mean curvature
     *
     * The curvature will be stored on the mesh on the vertex property called "Mean Curvature"
     */
    bool computeMeanCurvature(int _objectId);

    QString version() { return QString("1.0"); };

  public :

    MeanCurvaturePlugin();
    ~MeanCurvaturePlugin();

    QString name() { return (QString("MeanCurvature")); };
    QString description( ) { return (QString("Generates Mean Curvature information")); };

  private:
    template< typename MeshT >
    void computeMeanCurvature(MeshT* _mesh);
};

#endif //MEANCURVATUREPLUGIN_HH
