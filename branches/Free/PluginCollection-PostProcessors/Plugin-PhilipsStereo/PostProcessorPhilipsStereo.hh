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
*   $Revision: 12292 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-09-08 10:47:22 +0200 (Do, 08 Sep 2011) $                     *
*                                                                            *
\*===========================================================================*/


#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/PostProcessorInterface.hh>

#include "widgets/philipsStereoSettingsWidget.hh"
#include <ACG/GL/globjects.hh>

class PostProcessorPhilipsStereoPlugin : public QObject, BaseInterface, PostProcessorInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(PostProcessorInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-PostProcessorPhilipsStereo")
#endif

  public:
   PostProcessorPhilipsStereoPlugin();
  
  public :
    QString name() { return (QString("Philips Stereo Postprocessor Plugin")); };
    QString description( ) { return (QString(tr("Takes the rendered Image and renders it to be compatible with philips autostereoscopic displays"))); };

    
  public slots:
    QString version() { return QString("1.0"); };

    // PostProcessorInterface
    QString postProcessorName();

  signals:
    //BaseInterface
    void updateView();

  private slots:

    // PostProcessorInterface
    QAction* optionsAction();
    void postProcess(ACG::GLState* _glstate, const PostProcessorInput& _input, GLuint _targetFBO);

    void slotShowOptionsMenu();

    QString checkOpenGL();

  private:

    GLSL::PtrProgram pProgram_;

    PhilipsStereoSettingsWidget* settingsWidget_;


};

