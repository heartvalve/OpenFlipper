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
//  CLASS QtFlapBox - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "QtFlapBox.hh"

#include <QLayout>
#include <QLabel>
#include <QScrollBar>
#include <QResizeEvent>
#include <QSize>
#include <iostream>

//== NAMESPACES ===============================================================

namespace ACG {
namespace QtWidgets {

//== IMPLEMENTATION ========================================================== 


QtFlapBox::QtFlapBox( QWidget * _parent, Qt::WindowFlags /* _f */  )
  : QScrollArea( _parent )
{
  setBackgroundRole(QPalette::Dark);
  //  setSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
  QSizePolicy sp = sizePolicy();
  sp.setHorizontalPolicy( QSizePolicy::MinimumExpanding );
  sp.setVerticalPolicy( QSizePolicy::Preferred );
  sp.setHorizontalStretch( 1 );
  sp.setVerticalStretch( 0 );
  setSizePolicy( sp );

  // create a container that holds all the flaps
  container = new QWidget;
  container->setBackgroundRole( QPalette::Button );


  // flaps are ordered vertically
  boxlayout = new QVBoxLayout( container );
  boxlayout->setMargin(0);
  boxlayout->setSpacing(0);

  // the container will be scrollable
  setWidget( container );

  relayout();
}


//-----------------------------------------------------------------------------


QtFlapBox::~QtFlapBox()
{
}

    
//-----------------------------------------------------------------------------


int
QtFlapBox::addItem(       QWidget * _widget,
		    const QIcon   & _icon,
		    const QString & _text )
{
  return insertItem( -1, _widget, _icon, _text );
}


//-----------------------------------------------------------------------------


int
QtFlapBox::addItem( QWidget * _widget, const QString & _text )
{
  return insertItem( -1, _widget, QIcon(), _text);
}


//-----------------------------------------------------------------------------


int
QtFlapBox::count() const
{
  return flapList.count();
}


//-----------------------------------------------------------------------------


int
QtFlapBox::indexOf( QWidget * _widget ) const
{
  Flap * f = flap( _widget );
  return ( f ? flapList.indexOf( *f ) : -1 );
}


//-----------------------------------------------------------------------------


int
QtFlapBox::insertItem( int             _index,
		       QWidget       * _widget,
		       const QIcon   & _icon,
		       const QString & _text )
{
  if ( ! _widget )
    return -1;

  connect( _widget, SIGNAL( destroyed( QObject * ) ),
	   this, SLOT( widgetDestroyed( QObject * ) ) );

  Flap f;
  f.button = new QPushButton;//( container );
  //  f.button->setMinimumSize( 200, 20 );
  connect( f.button, SIGNAL( clicked() ),
	   this, SLOT( buttonClicked() ) );

  f.widget = _widget;
  f.widget->setParent( container );
  f.widget->hide();

  f.setText( _text );
  f.setIcon( _icon );

  if ( _index < 0 || _index >= flapList.count() )
  {
    _index = flapList.count();
    flapList.append( f );
    boxlayout->addWidget( f.button );
    boxlayout->addWidget( f.widget );
    relayout();
  }
  else
  {
    flapList.insert( _index, f );
    relayout();
  }
  
  f.button->show();

//   d->updateTabs();
//   itemInserted(index);

  return _index;

}


//-----------------------------------------------------------------------------



int
QtFlapBox::insertItem( int             _index,
		       QWidget       * _widget,
		       const QString & _text )
{
  return insertItem( _index, _widget, QIcon(), _text );
}


//-----------------------------------------------------------------------------


bool
QtFlapBox::isItemEnabled( int _index ) const
{
  const Flap * f = flap( _index );
  return f && f->button->isEnabled();
}


//-----------------------------------------------------------------------------


QIcon
QtFlapBox::itemIcon( int _index ) const
{
  const Flap * f = flap( _index );
  return ( f ? f->icon() : QIcon() );
}

//-----------------------------------------------------------------------------


QString
QtFlapBox::itemText( int _index ) const
{
  const Flap * f = flap( _index );
  return ( f ? f->text() : QString() );
}


//-----------------------------------------------------------------------------


QString
QtFlapBox::itemToolTip( int _index ) const
{
  const Flap * f = flap( _index );
  return ( f ? f->toolTip() : QString() );
}


//-----------------------------------------------------------------------------


void
QtFlapBox::removeItem( int _index )
{
  if ( QWidget * w = widget( _index ) )
  {
    disconnect( w, SIGNAL( destroyed( QObject * ) ),
		this, SLOT( widgetDestroyed( QObject * ) ) );
    w->setParent( this );
    // destroy internal data
    widgetDestroyed( w );
    //    itemRemoved( _index );
  }
}


//-----------------------------------------------------------------------------


void
QtFlapBox::setItemEnabled( int _index, bool _enabled )
{
  Flap * f = flap( _index );
  if ( ! f )
    return;

  f->button->setEnabled( _enabled );
}


//-----------------------------------------------------------------------------


void
QtFlapBox::setItemIcon( int _index, const QIcon & _icon )
{
  Flap * f = flap( _index );
  if ( f )
    f->setIcon( _icon );
}


//-----------------------------------------------------------------------------


void
QtFlapBox::setItemText( int _index, const QString & _text )
{
  Flap * f = flap( _index );
  if ( f )
    f->setText( _text );
}


//-----------------------------------------------------------------------------


void
QtFlapBox::setItemToolTip( int _index, const QString & _tip )
{
  Flap * f = flap( _index );
  if ( f )
    f->setToolTip( _tip );
}


//-----------------------------------------------------------------------------


QWidget *
QtFlapBox::widget( int _index ) const
{
  if ( _index < 0 || _index >= (int) flapList.size() )
    return 0;

  return flapList.at( _index ).widget;
}


//-----------------------------------------------------------------------------


bool
QtFlapBox::isItemHidden( int _index ) const
{
  const Flap * f = flap( _index );

  if ( ! f )
    return false;

  return f->widget->isHidden();
}


//-----------------------------------------------------------------------------


void
QtFlapBox::setItemHidden( int _index, bool _hidden )
{
  Flap * f = flap( _index );
  if ( ! f )
    return;

  f->widget->setHidden( _hidden );
}


//-----------------------------------------------------------------------------


QSize
QtFlapBox::sizeHint() const
{
  int width_hint  = 0;
  int height_hint = 0;

  for ( FlapList::ConstIterator i = flapList.constBegin();
 	i != flapList.constEnd(); ++i)
  {
    QWidget * w = i->widget;
    QSize ws = w->sizeHint();
      
    QPushButton * b = i->button;
    QSize bs = b->sizeHint();
      
    if ( bs.width() > width_hint )
      width_hint = bs.width();

    if ( ! w->isHidden() )
    {
      height_hint += ws.height();
      if ( ws.width() > width_hint )
	width_hint = ws.width();
    }

    height_hint += bs.height();
  }


  QSize sz( width_hint + verticalScrollBar()->sizeHint().width()
	    + 2 * frameWidth(),
	    height_hint );

  return sz;
}


//-----------------------------------------------------------------------------


QtFlapBox::Flap *
QtFlapBox::flap( QWidget * _widget ) const
{
 if ( ! _widget )
   return 0;

 for ( FlapList::ConstIterator i = flapList.constBegin();
       i != flapList.constEnd(); ++i )
   if ( (*i).widget == _widget )
     return (Flap*) &(*i);

 return 0;
}


//-----------------------------------------------------------------------------


const QtFlapBox::Flap *
QtFlapBox::flap( int _index ) const
{
  if ( _index >= 0 && _index < flapList.size() )
    return &flapList.at( _index );
  return 0;
}


//-----------------------------------------------------------------------------


QtFlapBox::Flap *
QtFlapBox::flap( int _index )
{
  if ( _index >= 0 && _index < flapList.size() )
    return &flapList[ _index ];
  return 0;
}



//-----------------------------------------------------------------------------


void updateFlaps()
{
//     QToolBoxButton *lastButton = currentPage ? currentPage->button : 0;
//     bool after = false;
//     for (PageList::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i) {
//         QToolBoxButton *tB = (*i).button;
//         QWidget *tW = (*i).widget;
//         if (after) {
//             QPalette p = tB->palette();
//             p.setColor(tB->backgroundRole(), tW->palette().color(tW->backgroundRole()));
//             tB->setPalette(p);
//             tB->update();
//         } else if (tB->backgroundRole() != QPalette::Background) {
//             tB->setBackgroundRole(QPalette::Background);
//             tB->update();
//         }
//         after = (*i).button == lastButton;
//     }

}


//-----------------------------------------------------------------------------


void
QtFlapBox::relayout()
{
  delete boxlayout;
  boxlayout = new QVBoxLayout;
  boxlayout->setMargin(0);
  boxlayout->setSpacing(0);

  for ( FlapList::ConstIterator i = flapList.constBegin();
	i != flapList.constEnd(); ++i )
  {
    boxlayout->addWidget( (*i).button );
    boxlayout->addWidget( (*i).widget );
  }

  container->setLayout( boxlayout );

  QSize area_size = size();
  QSize cont_size = container->sizeHint();
  QSize s;
  s.setWidth( area_size.width() );
  s.setHeight( cont_size.height() );

  container->resize( s );
  //  resize( sizeHint() );

  updateGeometry();
}


//-----------------------------------------------------------------------------


void
QtFlapBox::buttonClicked()
{
  QPushButton * b = ::qobject_cast< QPushButton * >( sender() );

  for ( FlapList::ConstIterator i = flapList.constBegin();
	i != flapList.constEnd(); ++i)
    if ( i->button == b )
    {
      QWidget * w = i->widget;

      if ( w->isHidden() )
	w->show();
      else
	w->hide();

      relayout();
      break;
    }
  
}


//-----------------------------------------------------------------------------


void
QtFlapBox::widgetDestroyed( QObject * _object )
{
  // no verification - vtbl corrupted already
  QWidget * p = ( QWidget * ) _object;
  
  Flap * f = flap( p );

  if ( !p || !f )
    return;

  boxlayout->removeWidget( f->widget );
  boxlayout->removeWidget( f->button );
  delete f->button;

  flapList.removeAll( *f );
}


//-----------------------------------------------------------------------------


void
QtFlapBox::resizeEvent( QResizeEvent * _event )
{
  QSize container_size = container->size();

  container_size.setWidth( _event->size().width() );

  container->resize( container_size );

  QScrollArea::resizeEvent( _event );
}


//-----------------------------------------------------------------------------



//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
