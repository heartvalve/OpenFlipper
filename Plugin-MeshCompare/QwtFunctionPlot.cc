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
*   $Revision: 13770 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2012-02-13 16:19:41 +0100 (Mo, 13. Feb 2012) $                     *
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
#include <qwt_plot_histogram.h>

#include <ACG/Utils/ColorCoder.hh>


#include <cfloat>
#include <cmath>

//== NAMESPACES ===============================================================

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

  qwtPlot->setAxisTitle(QwtPlot::yLeft,   "count" );

  plot_zoomer_ = new QwtPlotZoomer( qwtPlot->canvas());
  plot_zoomer_->initKeyPattern();
  
  QwtPlotPanner *panner = new QwtPlotPanner( qwtPlot->canvas() );
  panner->setMouseButton( Qt::MidButton );
  
  // delete widget on close
  setAttribute(Qt::WA_DeleteOnClose, true);

  histogram_ = new Histogram();
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::setFunction(const std::vector<double>& _values)
{
  values_ = _values;
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::replot()
{
  //create intervals
  const int intervalCount = 100;

  QVector<QwtIntervalSample> intervals(intervalCount);
  std::vector< QColor > colors;

  double realMin = FLT_MAX;
  double realMax = -FLT_MAX;

  for ( unsigned int i=0; i < values_.size(); i++) {
    realMin = std::min(realMin,values_[i]);
    realMax = std::max(realMax,values_[i]);
  }

  double pos   = realMin;
  double width = ( realMax - realMin ) / intervalCount;

  QColor lastColor = Qt::black;

  ACG::ColorCoder cCoder(std::max(min_,realMin),std::min(max_,realMax));

  for ( int i = 0; i < (int)intervals.size(); i++ )
  {

    intervals[i] = QwtIntervalSample(0.0,pos, pos + width);
    pos += width;

    //compute a color for the given interval
    const double intervalCenter = pos + (width/2.0);

    colors.push_back( cCoder.color_qcolor(intervalCenter) );
    lastColor = colors.back();
  }

  // sort values into intervals
  // Could be more efficient when we calculate the correct interval based on the above loop directly
  for ( uint i=0; i < values_.size(); i++)
    for ( int j = 0; j < (int)intervals.size(); j++ )
      if ( intervals[j].interval.contains( values_[i]  ) ) {
        intervals[j].value++;
        break;
      }

  //get max Count for scaling the y-axis
  double maxCount = 0;

  for ( int i = 0; i < (int)intervals.size(); i++ )
    maxCount = std::max(maxCount, intervals[i].value);


  QwtIntervalSeriesData* data = new QwtIntervalSeriesData(intervals);
  histogram_->setData(data);
  histogram_->setColors(colors);
  histogram_->attach(qwtPlot);

  qwtPlot->setAxisScale(QwtPlot::yLeft, 0.0, maxCount);
  qwtPlot->setAxisScale(QwtPlot::xBottom, realMin, realMax);

  //define this scaling as the zoomBase
  plot_zoomer_->setZoomBase();

  //Mark the clamp values in the histogram.
  if ( min_ > realMin  ) {
    if ( ! clampMinMarker_ ) {
      clampMinMarker_ = new QwtPlotMarker();
      minSymbol_ = new QwtSymbol(QwtSymbol::VLine);
      minSymbol_->setColor(cCoder.color_qcolor(std::max(min_,realMin)));
      minSymbol_->setSize(200,1000);
      QPen pen = minSymbol_->pen();
      pen.setWidth(3);
      minSymbol_->setPen(pen);
      clampMinMarker_->setSymbol(minSymbol_);
      clampMinMarker_->attach(qwtPlot);
      clampMinMarker_->show();
    }

    // Draw at right boundary if greater than the maximal value we get from the function
    clampMinMarker_->setXValue(std::max(min_,realMin));

  } else {
    if ( clampMinMarker_ ) {
      clampMinMarker_->detach();
      delete clampMinMarker_;
      clampMinMarker_ = 0;
    }
  }

  //Mark the clamp values in the histogram.
  if ( max_ < realMax ) {
    if ( ! clampMaxMarker_ ) {
      clampMaxMarker_ = new QwtPlotMarker();
      maxSymbol_ = new QwtSymbol(QwtSymbol::VLine);
      maxSymbol_->setColor(cCoder.color_qcolor(std::min(max_,realMax)));
      maxSymbol_->setSize(200,1000);
      QPen pen = maxSymbol_->pen();
      pen.setWidth(3);
      maxSymbol_->setPen(pen);
      clampMaxMarker_->setSymbol(maxSymbol_);
      clampMaxMarker_->attach(qwtPlot);
      clampMaxMarker_->show();
    }

    // Draw at right boundary if greater than the maximal value we get from the function
    clampMaxMarker_->setXValue(std::min(max_,realMax));

  } else {
    if ( clampMaxMarker_ ) {
      clampMaxMarker_->detach();
      delete clampMaxMarker_;
      clampMaxMarker_ = 0;
    }
  }

  // an plot it
  qwtPlot->replot();
}

void QwtFunctionPlot::setMinMax(double _min, double _max) {
  min_ = _min;
  max_ = _max;
}



//=============================================================================
//=============================================================================

#endif // WITH_QWT
