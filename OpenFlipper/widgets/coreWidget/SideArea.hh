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

#ifndef SIDE_AREA_
#define SIDE_AREA_

//=============================================================================
//
//  CLASS SideElement
//
//=============================================================================

//== INCLUDES =================================================================

#include <QWidget>
#include <QString>
#include <QSettings>

//== FORWARDDECLARATIONS ======================================================

class QVBoxLayout;

class SideElement;

/** \class SideArea SideArea.hh <OpenFlipper/widgets/coreWidget/SideArea.hh>

  A widget that holds the different plugin tool widgets
 **/


class SideArea : public QWidget {
  public:

    /** Create a SideElement
      \param _parent Parent widget
    */
    SideArea (QWidget *_parent = 0);

    /** Adds a plugin tool widget
      \param _w Plugin widget
      \param _name Plugin name
    */
    void addItem (QWidget *_w, QString _name);

    /// clears the whole tool widget area
    void clear ();

    /// returns the current state
    void saveState (QSettings &_settings);

    /// restores the state
    void restoreState (QSettings &_settings);

  private:
    // elements
    QVector<SideElement *> items_;

    // main layout
    QVBoxLayout *layout_;
};

//=============================================================================
//=============================================================================
#endif // SIDE_AREA_ defined
//=============================================================================