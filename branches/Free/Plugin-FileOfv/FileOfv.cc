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


#include <QtGui>
#include <QFileInfo>
#include <QSettings>

#include "FileOfv.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

void FileViewPlugin::initializePlugin() {
}

QString FileViewPlugin::getLoadFilters() {
  return QString( tr("View definition files ( *.ofv )") );
};

QString FileViewPlugin::getSaveFilters() {
  return QString( tr("View definition files ( *.ofv )") );
};

DataType  FileViewPlugin::supportedType() {
  return DataType();
}

int FileViewPlugin::loadObject(QString _filename) {

    // Declare variables
    int         width = 1;
    int         height = 1;
    ACG::Vec3d  eye(1.0,1.0,1.0);
    ACG::Vec3d  center(0.0,0.0,0.0);
    ACG::Vec3d  up(1.0,0.0,0.0);
//    float       fovy = 0;
    ACG::Vec4f  background;

    //bool e_widthAndHeight = false;
    bool e_eye = false;
    bool e_center = false;
    bool e_up = false;
//    bool e_fovy = false;
    bool e_background = false;

    QSettings settings(_filename, QSettings::IniFormat);
    settings.beginGroup("VIEW");

    if(settings.contains("Width") && settings.contains("Height")) {
        width = settings.value("Width").toInt();
        height = settings.value("Height").toInt();
        std::cerr << "Setting new viewport to " << width << "x" << height << std::endl;
        //e_widthAndHeight = true;
    }

    if(settings.contains("EyeX")) {
        eye[0] = settings.value("EyeX").toDouble();
        eye[1] = settings.value("EyeY").toDouble();
        eye[2] = settings.value("EyeZ").toDouble();
        std::cerr << "Setting new eye position to " << eye << std::endl;
        e_eye = true;
    }

    if(settings.contains("CenterX")) {
        center[0] = settings.value("CenterX").toDouble();
        center[1] = settings.value("CenterY").toDouble();
        center[2] = settings.value("CenterZ").toDouble();
        std::cerr << "Setting new scene center to " << center << std::endl;
        e_center = true;
    }

    if(settings.contains("UpX")) {
        up[0] = settings.value("UpX").toDouble();
        up[1] = settings.value("UpY").toDouble();
        up[2] = settings.value("UpZ").toDouble();
        std::cerr << "Setting new up vector to " << up << std::endl;
        e_up = true;
    }

//    if(settings.contains("Fovy")) {
//        fovy = settings.value("Fovy").toDouble();
//        std::cerr << "Setting fovy to " << fovy << std::endl;
//        e_fovy = true;
//    }

    if(settings.contains("BackgroundR")) {
        background[0] = settings.value("BackgroundR").toDouble();
        background[1] = settings.value("BackgroundG").toDouble();
        background[2] = settings.value("BackgroundB").toDouble();
        background[3] = settings.value("BackgroundA").toDouble();
        std::cerr << "Setting new background color to " << background << std::endl;
        e_background = true;
    }

    settings.endGroup();

    // Now set new projection and view

    // Get number of viewers
    int viewers = PluginFunctions::viewers();

    for(int i = 0; i < viewers; ++i) {

        Viewer::ViewerProperties& props = PluginFunctions::viewerProperties(i);
        ACG::GLState& state = props.glState();

//        // Perspective update
//        double aspect = 0.0;
//        if(e_widthAndHeight) aspect = width / height;
//        else                 aspect = state.aspect();

        // Set projection matrix
        //if(e_fovy)
        //    state.perspective(fovy, aspect, near, far);

        // Viewport
        //if(e_widthAndHeight)
        //    state.viewport(0, 0, width, height);

        //std::cerr << "Fovy: " << state.fovy() << std::endl;
    }

    // LookAt
    ACG::Vec3d new_eye = e_eye ? eye : PluginFunctions::eyePos();
    ACG::Vec3d new_center = e_center ? center : PluginFunctions::sceneCenter();
    ACG::Vec3d new_up = e_up ? up : PluginFunctions::upVector();

    PluginFunctions::lookAt(new_eye, new_center, new_up);

    // Background color
    if(e_background) {
        PluginFunctions::setBackColor(background);
     }

    emit updateView();

    return 0;
};

bool FileViewPlugin::saveObject(int /*_id*/, QString /*_filename*/, std::streamsize /*_precision*/) {

  return true;
}

Q_EXPORT_PLUGIN2( fileviewplugin , FileViewPlugin );

