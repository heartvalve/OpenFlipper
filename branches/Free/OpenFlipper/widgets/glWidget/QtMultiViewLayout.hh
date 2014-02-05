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




//=============================================================================
//
//  CLASS QtMultiViewLayout - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalDefines.hh>
#include <QGraphicsLayout>
#include <QGraphicsLayoutItem>
#include <QGraphicsWidget>

//== NAMESPACES ===============================================================

//== CLASS DEFINITION =========================================================


/** Graphics scene layout for multi view.
*/

class DLLEXPORT QtMultiViewLayout : public QGraphicsLayout
{
  public:

    /// MultiView display modes
    enum MultiViewMode {
       /*
       * #############
       * #           #
       * #           #
       * #     1     #
       * #           #
       * #           #
       * #############
       */
       SingleView,
       /*
       * #############
       * #     #     #
       * #     #     #
       * #  1  #  2  #
       * #     #     #
       * #     #     #
       * #############
       */
       DoubleView,
       /*
       * #############
       * #  1  #  2  #
       * #     #     #
       * #############
       * #  3  #  4  #
       * #     #     #
       * #############
       */
      Grid,
      /*
       * #############
       * #       # 2 #
       * #       #####
       * #   1   # 3 #
       * #       #####
       * #       # 4 #
       * #############
       */	
      HSplit
    };

    QtMultiViewLayout (QGraphicsLayoutItem * _parent = 0);

    /// Adds Widget to Layout
    void addItem (QGraphicsWidget *_item, unsigned int _pos);

    /// Sets layout mode
    void setMode (MultiViewMode _mode);

    /// Retruns current layout modes
    MultiViewMode mode() const { return mode_;};

    /// Sets space between items
    void setSpacing (unsigned int _s);

    /// Sets primary element for SingleView and HSplit
    void setPrimary (unsigned int _i);


    /// Pure virtual functions that have to be implemented
    virtual int count() const;
    virtual QGraphicsLayoutItem * itemAt(int _i) const;
    virtual void removeAt (int _index);

    virtual QSizeF sizeHint(Qt::SizeHint _which, const QSizeF & _constraint = QSizeF()) const;

    /// Tracks geometry changes
    virtual void setGeometry(const QRectF & rect);


  private:

    /// Recalculate layout
    void reLayout ();

    /// current modes
    MultiViewMode mode_;

    /// Spacing
    unsigned int  spacing_;

    /// Items
    QGraphicsWidget *items_[4];

    /// Primary element
    unsigned int primary_;

};

//=============================================================================

//=============================================================================
