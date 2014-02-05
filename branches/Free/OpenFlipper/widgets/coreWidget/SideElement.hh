/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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

#ifndef SIDE_ELEMENT_
#define SIDE_ELEMENT_

//=============================================================================
//
//  CLASS SideElement
//
//=============================================================================

//== INCLUDES =================================================================

#include <QWidget>
#include <QSettings>

//== FORWARDDECLARATIONS ======================================================

class QIcon;
class QLabel;
class QToolButton;
class QAction;
class QDialog;
class QVBoxLayout;

class SideArea;


//== CLASS DEFINITION =========================================================


/** \class SideElement SideElement.hh <OpenFlipper/widgets/coreWidget/SideElement.hh>

  A widget that holds the plugin side area widget
 **/


class SideElement : public QWidget
{
  Q_OBJECT

  public:
    /** Create a SideElement
      @param _parent   SideArea widget that holds this widget
      @param _w        Plugin tool widget
      @param _name     Plugin name
      @param _icon     An icon that should be shown in the title bar of the side element

    */
    SideElement (SideArea *_parent, QWidget *_w, QString _name, QIcon* _icon = 0);

    /// Destructor
    ~SideElement ();

    /// Set the element as active
    void setActive(bool _active);

    /// saves the current state
    void saveState (QSettings &_settings);

    /// restores the state
    void restoreState (QSettings &_settings);

    /// return the name
    const QString& name();

    /// returns if the SideElement is active
    bool active();

    /// returns the pointer to the plugin tool widget
    QWidget const * widget();

  private:

    /// Clickable area inside of the side element.
    class TopArea : public QWidget
    {
      public:
        TopArea (SideElement *_e);

        virtual void mousePressEvent (QMouseEvent *_event);
      private:
        SideElement *e_;
    };

    /// Called on mouse press
    void labelPress ();

  private slots:

    /// Called if the detach button was pressed
    void detachPressed (bool _checked);

    /// Called if a detached dialog was closed
    void dialogClosed ();

  private:

    // parent
    SideArea *parent_;

    // plugin widget
    QWidget *widget_;

    // plugin name
    QString name_;

    // Icon
    QIcon* icon_;

    // main layout
    QVBoxLayout *mainLayout_;

    // status
    bool active_;

    // label that display the plugin name
    QLabel *label_;
    QLabel *iconHolder_;

    // detach button & action
    QToolButton *detachButton_;
    QAction *detachAction_;

    // dialog for detached widget
    QDialog *dialog_;
};

//=============================================================================
//=============================================================================
#endif // SIDE_ELEMENT_ defined
//=============================================================================
