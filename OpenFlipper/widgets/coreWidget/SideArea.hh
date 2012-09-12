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



#ifndef SIDE_AREA_
#define SIDE_AREA_

//=============================================================================
//
//  CLASS SideElement
//
//=============================================================================

//== INCLUDES =================================================================

#include <QIcon>
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

    /** Adds a plugin tool widget
      \param _w Plugin widget
      \param _name Plugin name
      \param _icon an icon
    */
    void addItem (QWidget *_w, QString _name, QIcon* _icon);

    /// clears the whole tool widget area
    void clear ();

    //expand all elements
    void expandAll();

    /// returns the current state
    void saveState (QSettings &_settings);

    /// restores the state
    void restoreState (QSettings &_settings);

    /// set the active state of given element
    void setElementActive(QString _name, bool _active);

    /// Move a toolbox widget to a given position
    void moveItemToPosition(const QString& _name, int _position);

    /// Get number of widgets
    int getNumberOfWidgets() const;

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

