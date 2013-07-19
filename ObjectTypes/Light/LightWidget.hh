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

#include <ui_lightWidget.hh>

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <ObjectTypes/Light/LightNode.hh>
#include <OpenFlipper/common/GlobalDefines.hh>
#include "Light.hh"

class DLLEXPORT LightWidget : public QDialog, public Ui::LightWidgetBase
{
  Q_OBJECT

  public:
    LightWidget( ACG::SceneGraph::BaseNode* _node, QWidget *parent = 0);
    
  private slots:
    /// The directional checkbox changed -> update object
    void directionalToggled();
    
    /// The fixed position checkbox
    void fixedPositionChanged();
    
    /// Color values have changed
    void ambientChanged();
    void diffuseChanged();
    void specularChanged();
    
    /// Brightness value has changed
    void brightnessChanged(int _newValue);
    
    /// Light radius has changed
    void radiusChanged();
    
    /// Spot direction changed
    void spotChanged();
    
  protected:
    /// Initialize contents of widget before showing it
    virtual void showEvent ( QShowEvent * event );
    
  private:
    /// Initializes the internal object. Returns true if successfull
    bool getObject();
    
    /// Called when the object has been updated
    void updated();
  
  private:
    ACG::SceneGraph::LightNode* node_;
    
    LightObject* object_;
    LightSource* light_;
    
    bool updatingWidgets_;
};

