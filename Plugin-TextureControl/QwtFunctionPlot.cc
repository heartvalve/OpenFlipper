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
//  CLASS QtFunctionPlot - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#ifdef WITH_QWT

#include "QwtFunctionPlot.hh"

#include <iostream>
#include <algorithm>

#include <QPen>

#include <QLineEdit>
#include <QLabel>
#include <QPainter>
#include <qwt_curve_fitter.h>
#include <qwt_plot_panner.h>
#include <qwt_symbol.h>


#if QWT_VERSION >= 0x060000
 #include <qwt_plot_histogram.h>
#else
 #include <qwt_interval_data.h>
#endif

#include <cfloat>
#include <cmath>
#include "TextureMath.hh"

//== NAMESPACES ===============================================================

namespace ACG {

//== IMPLEMENTATION ==========================================================


/// Default constructor
QwtFunctionPlot::QwtFunctionPlot(QWidget* _parent) :
    QDialog( _parent ),
    Ui::QwtFunctionPlotBase(),
    plot_zoomer_(0),
    clampMinMarker_(0),
    minSymbol_(0),
    clampMaxMarker_(0),
    maxSymbol_(0),
    min_(FLT_MAX),
    max_(FLT_MIN)
{
  setupUi( this );

  plot_zoomer_ = new QwtPlotZoomer( qwtPlot->canvas());
  plot_zoomer_->initKeyPattern();
  connect(zoomInButton, SIGNAL( clicked() ), this,SLOT( zoomIn() )  );
  connect(zoomOutButton,SIGNAL( clicked() ), this,SLOT( zoomOut() ) );
  connect(clampButton,  SIGNAL( clicked() ), this,SLOT( clamp() )   );
  
  QwtPlotPanner *panner = new QwtPlotPanner( qwtPlot->canvas() );
  panner->setMouseButton( Qt::MidButton );
  
  // delete widget on close
  setAttribute(Qt::WA_DeleteOnClose, true);

#if QWT_VERSION >= 0x060000
  histogram_ = new Histogram();
#else
  histogram_ = new HistogramItem();
#endif

  image_ = 0;
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::setFunction( std::vector<double>& _values)
{
  values_ = _values;

  //get min/max values
  min_ = FLT_MAX;
  max_ = -FLT_MAX;

  for ( uint i=0; i < values_.size(); i++){
    min_ = std::min(min_, values_[i] );
    max_ = std::max(max_, values_[i] );
  }

}

//------------------------------------------------------------------------------

void QwtFunctionPlot::setParameters(const TexParameters& _parameters)
{
  parameters_ = _parameters;
}

void QwtFunctionPlot::setParameters(
    bool   _repeat,
    double _repeatMax,
    bool   _clamp,
    double _clampMin,
    double _clampMax,
    bool   _center,
    bool   _absolute,
    bool   _scale)
{

  parameters_.repeat    = _repeat;
  parameters_.repeatMax = _repeatMax;
  parameters_.clamp     = _clamp;
  parameters_.clampMin  = _clampMin;
  parameters_.clampMax  = _clampMax;
  parameters_.center    = _center;
  parameters_.abs       = _absolute;
  parameters_.scale     = _scale;
}


//------------------------------------------------------------------------------

void QwtFunctionPlot::setImage(QImage* _image)
{
  image_ = _image;
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::zoomIn()
{
    emit plot_zoomer_->zoom(1);
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::zoomOut()
{
    emit plot_zoomer_->zoom(-1);
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::clamp()
{
    QRectF clamped = plot_zoomer_->zoomRect();
    clamped.setLeft( min_ );
    clamped.setRight( max_ );
    emit plot_zoomer_->zoom(clamped);
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::replot()
{
  //create intervals
  const int intervalCount = 100;

#if QWT_VERSION >= 0x060000
  QVector<QwtIntervalSample> intervals(intervalCount);
#else
  QwtArray<QwtDoubleInterval> intervals(intervalCount);
  QwtArray<double> count(intervalCount);
#endif
  std::vector< QColor > colors;

  double pos = min_;
  double width = ( max_ - min_ ) / intervalCount;

  QColor lastColor = Qt::black;

  TextureMath convert(parameters_,min_,max_);

  for ( int i = 0; i < (int)intervals.size(); i++ )
  {

#if QWT_VERSION >= 0x060000
    intervals[i] = QwtIntervalSample(0.0,pos, pos + width);
#else
    intervals[i] = QwtDoubleInterval(pos, pos + width);
#endif
    pos += width;

    //compute a color for the given interval
    if (image_ != 0){

      const double intervalCenter = pos + (width/2.0);

      const double value = convert.transform(intervalCenter);
      int val = int( value * ( image_->width() - 1) );

      val = val % image_->width(); // Simulate If texture is repeated, we have to make sure, we stay inside of the image
      colors.push_back( QColor( image_->pixel(val, 0) ) );
      lastColor = colors.back();
    }
  }

  //sort values into intervals
  for ( uint i=0; i < values_.size(); i++)
    for ( int j = 0; j < (int)intervals.size(); j++ )
#if QWT_VERSION >= 0x060000
      if ( intervals[j].interval.contains( values_[i]  ) )
        intervals[j].value++;
#else
      if ( intervals[j].contains( values_[i]  ) )
        count[j]++;
#endif

  //get max Count for scaling the y-axis
  double maxCount = 0;

  for ( int i = 0; i < (int)intervals.size(); i++ )
#if QWT_VERSION >= 0x060000
    maxCount = std::max(maxCount, intervals[i].value);
#else
    maxCount = std::max(maxCount, count[i]);
#endif


#if QWT_VERSION >= 0x060000
  QwtIntervalSeriesData* data = new QwtIntervalSeriesData(intervals);
  histogram_->setData(data);
#else
  histogram_->setData(QwtIntervalData(intervals, count));
#endif
  histogram_->setColors(colors);
  histogram_->attach(qwtPlot);

  qwtPlot->setAxisScale(QwtPlot::yLeft, 0.0, maxCount);
  qwtPlot->setAxisScale(QwtPlot::xBottom, min_, max_);

  qwtPlot->setAxisTitle(QwtPlot::yLeft,   "count" );
  qwtPlot->setAxisTitle(QwtPlot::xBottom, "values" );

  //define this scaling as the zoomBase
  plot_zoomer_->setZoomBase();

  // Mark the clamp values in the histogramm
  if ( parameters_.clamp ) {
    if ( ! clampMinMarker_ ) {
      clampMinMarker_ = new QwtPlotMarker();
      minSymbol_ = new QwtSymbol(QwtSymbol::VLine);
      minSymbol_->setColor(QColor(255,0,0));
      minSymbol_->setSize(200,1000);
      QPen pen = minSymbol_->pen();
      pen.setWidth(3);
      minSymbol_->setPen(pen);
      clampMinMarker_->setSymbol(minSymbol_);
      clampMinMarker_->attach(qwtPlot);
    }

    // Draw at left boundary if less than the minimal value we get from the function
    if ( parameters_.clampMin < min_ )
      clampMinMarker_->setXValue(min_);
    else
      clampMinMarker_->setXValue(parameters_.clampMin);



    clampMinMarker_->show();

    if ( ! clampMaxMarker_ ) {
      clampMaxMarker_ = new QwtPlotMarker();
      maxSymbol_ = new QwtSymbol(QwtSymbol::VLine);
      maxSymbol_->setColor(QColor(0,255,0));
      maxSymbol_->setSize(200,1000);
      QPen pen = maxSymbol_->pen();
      pen.setWidth(3);
      maxSymbol_->setPen(pen);
      clampMaxMarker_->setSymbol(maxSymbol_);
      clampMaxMarker_->attach(qwtPlot);
    }

    // Draw at right boundary if greater than the maximal value we get from the function
    if ( parameters_.clampMax < max_ )
      clampMaxMarker_->setXValue(parameters_.clampMax);
    else
      clampMaxMarker_->setXValue(max_);

    clampMaxMarker_->show();

  } else {
    if ( clampMinMarker_ )
      clampMinMarker_->hide();

    if ( clampMaxMarker_ )
      clampMaxMarker_->hide();
  }

  // an plot it
  qwtPlot->replot();
}





//=============================================================================
} // namespace db
//=============================================================================

#endif // WITH_QWT
