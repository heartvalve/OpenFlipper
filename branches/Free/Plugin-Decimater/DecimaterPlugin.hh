/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#ifndef DECIMATERPLUGIN_HH
#define DECIMATERPLUGIN_HH

//== INCLUDES =================================================================

#include <QObject>
#include <QMenuBar>
#include <QTimer>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>


#include "DecimaterToolbarWidget.hh"


//== CLASS DEFINITION =========================================================

/** Plugin for Decimater Support
 */
class DecimaterPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, BackupInterface, RPCInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(RPCInterface)

signals:

  // BaseInterface
  void updateView();
  void updatedObject(int);
  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

  // RPC Interface
  void pluginExists( QString _pluginName , bool& _exists  ) ;
  void functionExists( QString _pluginName , QString _functionName , bool& _exists  );

private slots:
    // BaseInterface
    void pluginsInitialized();
    
    void slotObjectUpdated( int _identifier ); // BaseInterface
    void slotObjectSelectionChanged( int _identifier ); // BaseInterface

public :

  /// Default constructor
  DecimaterPlugin() : tool_(0) {};

  /// Default destructor
  ~DecimaterPlugin() {};

  /// Initialize the toolbar (create a widget in the right side toolbox)
  bool initializeToolbox(QWidget*& _widget);

  /// Name of the Plugin
  QString name(){ return (QString("Decimater")); };

  /// Description of the Plugin
  QString description() { return (QString("Mesh Decimation ...")); };

private :

  /// Widget for Toolbox
  DecimaterToolbarWidget* tool_;

private slots:

  /// decimating called from button in toolbox
  void slot_decimate();

  /// roundness slider - spinbox sync
  void updateRoundness(int    _value);
  void updateRoundness(double _value);

  /// update number of vertices information
  void slotUpdateNumVertices();

//===========================================================================
/** @name Scripting Functions
  * @{ */
//===========================================================================

public slots:
  void decimate(int _objID, QVariantMap _constraints);

public slots:
   QString version() { return QString("1.0"); };


/** @} */

};

#endif //DECIMATERPLUGIN_HH
