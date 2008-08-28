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




#ifndef SMOOTHERPLUGIN_HH
#define SMOOTHERPLUGIN_HH

#include <QObject>
#include <QMenuBar>
#include <QSpinBox>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <ACG/QtWidgets/QtExaminerViewer.hh>
#include <OpenFlipper/common/Types.hh>

class SmootherPlugin : public QObject, BaseInterface, ToolboxInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(ToolboxInterface)           

   signals:
     void update_view();

   public :

     ~SmootherPlugin() {};
     
     bool initializeToolbox(QWidget*& _widget);
     void toolboxActivated( bool /*_activated*/ ){ };
     
     QString name() { return (QString("Simple Smoother")); };
     QString description( ) { return (QString("Smooths the active Mesh")); }; 
     
   private :
      
       /// Widget for Toolbox
      QWidget* tool_;
      
      /// Layout for Toolbox
      QGridLayout* MeshDialogLayout_;
      
      /// Button for slotTransfer
      QPushButton* smoothButton_;
      
      /// SpinBox for Number of iterations
      QSpinBox* iterationsSpinbox_;
      
      QComboBox* smootherTypeBox_;
      
      
      /// Property for the active mesh to store original point positions
      OpenMesh::VPropHandleT< TriMesh::Point > orig_pos_;
      
      void simpleLaplace();
      
   private slots:
      void slotSmooth();
};

#endif //SMOOTHERPLUGIN_HH
