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

//=============================================================================
//
// CLASS InfoMeshObjectPlugin
//
// Author:  David Bommes <bommes@cs.rwth-aachen.de>
//=============================================================================

#ifndef INFOPLUGIN_HH
#define INFOPLUGIN_HH

//== INCLUDES =================================================================

#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/InformationInterface.hh>

#include <OpenFlipper/BasePlugin/StatusbarInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/Skeleton/Skeleton.hh>

#include "infoDialog.hh"

//== CLASS DEFINITION =========================================================


/** \brief Plugin to visualize information about objects in the scene
 
  Plugin to visualize information about objects in the scene
*/
class InfoSkeletonObjectPlugin : public QObject, BaseInterface, InformationInterface, LoggingInterface, StatusbarInterface
{
  Q_OBJECT
      Q_INTERFACES(BaseInterface)
      Q_INTERFACES(InformationInterface)
      Q_INTERFACES(LoggingInterface)
      Q_INTERFACES(StatusbarInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-SkeletonObjectInfo")
#endif


  signals:

    // BaseInterface
    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // StatusbarInterface
    void addWidgetToStatusbar(QWidget* _widget);

  private slots :

    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();
    
    void noguiSupported( ) {} ;

    // InformationInterface
    void slotInformationRequested(const QPoint _clickedPoint, DataType _type);
    DataType supportedDataTypes();

  public :

  // default constructor
    InfoSkeletonObjectPlugin(): info_(0) {};

  // default destructor
      ~InfoSkeletonObjectPlugin() {};

      /// Name of the Plugin
      QString name(){ return (QString("InfoSkeletonObject")); };

      /// Description of the Plugin
      QString description() { return (QString(tr("Provides Information on Skeleton Objects"))); };

  private :
    void printSkeletonInfo( Skeleton* _id,   unsigned int _objectId, unsigned int _index, ACG::Vec3d& _hitPoint );

    InfoDialog* info_;

  public slots:
  		/// get total number of joints for a given skeleton
      int jointCount(int _skeletonID);
      /// get total number of branches for a given skeleton
      int branchCount(int _skeletonID);
      /// get total number of leaves for a given skeleton
      int leafCount(int _skeletonID);

      /// get minumum bounding box point
      Vector boundingBoxMin(int _skeletonID);
      /// get maximum bounding box point
      Vector boundingBoxMax(int _skeletonID);
      /// get the size of the bounding box
      Vector boundingBoxSize(int _skeletonID);

      /// get the length of a bone
      double boneLength(int _skeletonID, int _jointID);

      /// get the minimal bone length
      double minBoneLength(int _skeletonID);
      /// get the maximal bone length
      double maxBoneLength(int _skeletonID);
      /// get the mean bone length
      double meanBoneLength(int _skeletonID);

  private:
     /// set scripting slot descriptions
     void setDescriptions();

  public slots:
    QString version() { return QString("1.0"); };

/** @} */

};

#endif //MOVEPLUGIN_HH

