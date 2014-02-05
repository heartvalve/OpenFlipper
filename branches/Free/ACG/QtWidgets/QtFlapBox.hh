/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS QtFlapBox
//
//=============================================================================

#ifndef ACG_QT_FLAPBOX_HH
#define ACG_QT_FLAPBOX_HH

//== INCLUDES =================================================================

#include <iostream>

#include <QPushButton>
#include <QBoxLayout>
#include <QResizeEvent>
#include <QFrame>
#include <QScrollArea>
#include "../Config/ACGDefines.hh"

//== NAMESPACES ===============================================================

namespace ACG {
namespace QtWidgets {

//== CLASS DEFINITION =========================================================

class ACGDLLEXPORT QtFlapBox : public QScrollArea
{
  Q_OBJECT

public:

  QtFlapBox( QWidget * _parent = 0, Qt::WindowFlags _f = 0 );
  ~QtFlapBox ();

  int addItem( QWidget * _widget, const QIcon & _icon, const QString & _text );
  int addItem( QWidget * _widget, const QString & _text );

  int count () const;

  int indexOf( QWidget * _widget ) const;
  int insertItem( int _index, QWidget * _widget,
		  const QIcon & _icon, const QString & _text );
  int insertItem( int _index, QWidget * _widget, const QString & _text );

  bool isItemEnabled( int _index ) const;
  QIcon itemIcon( int _index ) const;

  QString itemText( int _index ) const;
  QString itemToolTip( int _index ) const;
  void removeItem( int _index );
  void setItemEnabled( int _index, bool _enabled );
  void setItemIcon( int _index, const QIcon & _icon );
  void setItemText( int _index, const QString & _text );
  void setItemToolTip( int _index, const QString & _toolTip );
  QWidget * widget( int _index ) const;

  // the following methods are specific for QtFlapBox

  bool isItemHidden( int _index ) const;
  void setItemHidden( int _index, bool _hidden );

  virtual QSize sizeHint() const;


  virtual void resizeEvent ( QResizeEvent * _event );

signals:

  void sizeHintChanged();


//   void toggled( int index, bool state );


// public slots:

//   void toggle( int index );


// private slots:

//   void flapToggled( bool );

  //  virtual QSize sizeHint () const;

private:


  struct Flap
  {
    QPushButton * button;
    QWidget     * widget;

    void setText( const QString & _text )
    {
      button->setText( _text );
    }

    void setIcon( const QIcon & _icon )
    {
      button->setIcon( _icon );
    }

    void setToolTip( const QString & _tip )
    {
      button->setToolTip( _tip );
    }

    QString text() const
    {
      return button->text();
    }

    QIcon icon() const
    {
      return button->icon();
    }

    QString toolTip() const
    {
      return button->toolTip();
    }

    bool operator==( const Flap & _other ) const
    {
      return widget == _other.widget;
    }
  };



private slots:

  void buttonClicked();
  void widgetDestroyed(QObject*);

private:

  typedef QList< Flap > FlapList;

        Flap * flap( QWidget * _widget ) const;
  const Flap * flap( int _index ) const;
        Flap * flap( int _index );

  void updateFlaps();
  void relayout();

  FlapList      flapList;
  QVBoxLayout * boxlayout;
  QWidget     * container;

};

//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
#endif // ACG_QT_FLAPBOX_HH defined
//=============================================================================
