//=============================================================================
//
//  CLASS QtFunctionPlot - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "QwtFunctionPlot.hh"

#include <iostream>
#include <algorithm>

#include <QPen>

#include <qlineedit.h>
#include <qlabel.h>
#include <qpainter.h>


//== NAMESPACES ===============================================================

namespace ACG {

//== IMPLEMENTATION ========================================================== 


void 
QwtFunctionPlot::
add_function( std::vector<double>& _x,
	      std::vector<double>& _y,
	      const char*          _title,
	      QColor               _col  )
{
  // if color is white -> choose random color
  QColor col = _col;
  if( col.red() == 255 && col.green() == 255 && col.blue() == 255)
    col.setRgb( (int)(30 + double(rand())/double(RAND_MAX)*200),
		(int)(30 + double(rand())/double(RAND_MAX)*200),
		(int)(30 + double(rand())/double(RAND_MAX)*200));
  
  // create new curve and attach it to plot widget
  QwtPlotCurve *curve = new QwtPlotCurve(_title);
  curve->setData( &(_x[0]), &(_y[0]), std::min(_x.size(), _y.size()));
  curve->setPen( QPen(col));
  functions_.push_back(curve);
  curve->attach( qwtPlot );
  
  // check legend item
  ((QwtLegendItem*)legend_->find( curve))->setChecked(true);

  //  plot_zoomer_->setZoomBase(false);
  update_zoom_base();
  qwtPlot->replot();
}


// ------------------------------------------------------------------------------


void 
QwtFunctionPlot::
function_mode()
{
  for(unsigned int i=0; i<functions_.size(); ++i)
  {
    functions_[i]->setStyle( QwtPlotCurve::Lines );
  }
  qwtPlot->replot();
}


// ------------------------------------------------------------------------------


void 
QwtFunctionPlot::
histogram_mode()
{
  for(unsigned int i=0; i<functions_.size(); ++i)
  {
    functions_[i]->setStyle( QwtPlotCurve::Sticks );
  }
  qwtPlot->replot();
}

// ------------------------------------------------------------------------------


void 
QwtFunctionPlot::
linear_scaling()
{
//   qwtPlot->setAxisScaleEngine(1, (QwtScaleEngine*)linear_scale_engine_);
//   qwtPlot->replot();
}


// ------------------------------------------------------------------------------

void 
QwtFunctionPlot::
logarithmic_scaling()
{
//   qwtPlot->setAxisScaleEngine( 1, (QwtScaleEngine*)log10_scale_engine_);
//   qwtPlot->replot();
}


// ------------------------------------------------------------------------------


void 
QwtFunctionPlot::
update_pens(int _inc)
{
  for(unsigned int i=0; i<functions_.size(); ++i)
  {
    QPen pen = functions_[i]->pen();

    pen.setWidth( std::max(1,(int)pen.width() + _inc) );

    functions_[i]->setPen( pen);
  }
  qwtPlot->replot();
}


// ------------------------------------------------------------------------------


void 
QwtFunctionPlot::
add_function( std::vector<double>& _y,
	      const char*          _title,
	      QColor               _col  )
{
  // set up uniform x-vector
  std::vector<double> x(_y.size());
  for(unsigned int i=0; i<x.size(); ++i)
    x[i] = i;

  add_function( x, _y, _title, _col);
}


// ------------------------------------------------------------------------------


void 
QwtFunctionPlot::
slotLegendChecked(QwtPlotItem* _plot_item, bool _visible)
{
  // only draw checked curves
  _plot_item->setVisible(_visible);
  update_zoom_base();
  qwtPlot->replot();
}


// ------------------------------------------------------------------------------


void 
QwtFunctionPlot::
update_zoom_base()
{
  QwtDoubleRect rect;
  for(unsigned int i=0; i<functions_.size(); ++i)
  {
    if(functions_[i]->isVisible())
      rect |= functions_[i]->boundingRect();
  }
  if( rect.isValid())
  {
    plot_zoomer_->setZoomBase( rect );
  }
}


// ------------------------------------------------------------------------------


void 
QwtFunctionPlot::
keyPressEvent ( QKeyEvent* _event )
{
  switch( _event->key())
  {
    case Qt::Key_H:
    {
      std::cerr << "histogram mode...\n";
      histogram_mode();
      break;
    }
    case Qt::Key_F:
    {
      std::cerr << "function mode...\n";
      function_mode();
      break;
    }
    case Qt::Key_Plus:
    {
      update_pens(1);
      break;
    }
    case Qt::Key_Minus:
    {
      update_pens(-1);
      break;
    }
    case Qt::Key_1:
    {
      linear_scaling();
      break;
    }
    case Qt::Key_2:
    {
      logarithmic_scaling();
      break;
    }
  }
}


//=============================================================================
} // namespace db
//=============================================================================
