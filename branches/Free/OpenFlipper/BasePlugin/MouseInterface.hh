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




//
// C++ Interface: Basic Interface for mouse events
//
// Description: 
//
//
// Author: Jan Moebius <moebius@informatik.rwth-aachen.de>, (C) 2007



#ifndef MOUSEINTERFACE_HH 
#define MOUSEINTERFACE_HH 

 #include <QtGui>
 
/** \brief Interface for all plugins which have to react on mouse events 
  * 
  * When using mouse events in your plugins implement this interface. Remember that 
  * all plugins may get these signals. Therefore you hav to check if the current pickingMode
  * is active before doing anything. Only the slotMouseEventIdentify is independent of picking modes.
 */  
class MouseInterface {
  
  private slots :
    
    /**  \brief Wheel Event from Main App
      * 
      *  This slot is called if a wheel event occured in the Viewer and is extended by
      *  the current Mode set in the Viewer. You can add additional Picking Modes to
      *  the Viewer by calling examiner_widget_->addPickMode("Mode"). 
      *  This slot will only get called in pickingMode.
      *  @param _event Mousevent
      *  @param _mode Name of the current Picking Mode.
    */
    virtual void slotMouseWheelEvent(QWheelEvent * _event, const std::string & _mode) {};
    
    /**  \brief Mouse Event from Main App ( Picking Mode )
      * 
      *  This slot is called if a mouse event occured in the Viewer 
      *  This slot will only get called in pickingMode. 
      *  Right button clicks will not be passed to the plugins as this is reserved for
      *  the context Menu. 
      * 
      *  @param _event Mousevent
    */
    virtual void slotMouseEvent( QMouseEvent* _event ) {};
    
    /**  \brief Mouse Event from Main App ( Identify Mode )
      * 
      *  This slot is called if a mouse event occured in the Viewer 
      *  This slot will only get called in identifyMode. 
      *  Right button clicks will not be passed to the plugins as this is reserved for
      *  the context Menu. 
      * 
      *  @param _event Mousevent
    */    
    virtual void slotMouseEventIdentify() {};
    
  public : 
      
    /// Destructor
    virtual ~MouseInterface() {};
      
};

Q_DECLARE_INTERFACE(MouseInterface,"OpenFlipper.MouseInterface/1.0")
      
#endif // MOUSEINTERFACE_HH
