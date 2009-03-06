
#include <QWidget>
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
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================

#ifndef SIDE_ELEMENT_
#define SIDE_ELEMENT_

//=============================================================================
//
//  CLASS SideElement
//
//=============================================================================

//== INCLUDES =================================================================

#include <QSettings>

//== FORWARDDECLARATIONS ======================================================

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
      \param _parent SideArea widget that holds this widget
      \param _w Plugin tool widget
      \param _name Plugin name
    */
    SideElement (SideArea *_parent, QWidget *_w, QString _name);

    /// Destructor
    ~SideElement ();

    /// saves the current state
    void saveState (QSettings &_settings);

    /// restores the state
    void restoreState (QSettings &_settings);

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

    // main layout
    QVBoxLayout *mainLayout_;

    // status
    bool active_;

    // label that display the plugin name
    QLabel *label_;

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