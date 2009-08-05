/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
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

#include <qlineedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_interval_data.h>


#include <float.h>
#include <math.h>

//== NAMESPACES ===============================================================

namespace ACG {

//== IMPLEMENTATION ==========================================================


/// Default constructor
QwtFunctionPlot::QwtFunctionPlot(QWidget* _parent) :
    QDialog( _parent ),
    Ui::QwtFunctionPlotBase(),
    min_(FLT_MAX),
    max_(FLT_MIN)
{
  setupUi( this );

  plot_zoomer_ = new QwtPlotZoomer( qwtPlot->canvas());

  // delete widget on close
  setAttribute(Qt::WA_DeleteOnClose, true);

  histogram_ = new HistogramItem();
  image_ = 0;
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::setFunction( std::vector<double>& _values)
{
  values_ = _values;

  //get min/max values
  min_ = FLT_MAX;
  max_ = FLT_MIN;

  for ( uint i=0; i < values_.size(); i++){
    min_ = std::min(min_, values_[i] );
    max_ = std::max(max_, values_[i] );
  }

}

//------------------------------------------------------------------------------

void QwtFunctionPlot::setParameters(bool _repeat, double _repeatMax,
                                    bool _clamp,  double _clampMin, double _clampMax,
                                    bool _center,
                                    bool _absolute,
                                    bool _scale)
{
  repeat_ = _repeat;
  repeatMax_ = _repeatMax;
  clamp_ = _clamp;
  clampMin_ = _clampMin;
  clampMax_ = _clampMax;
  center_ = _center;
  absolute_ = _absolute;
  scale_ = _scale;
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::setImage(QImage* _image)
{
  image_ = _image;
}

//------------------------------------------------------------------------------

double QwtFunctionPlot::transform( double _value ){

  if (absolute_)
    _value = fabs(_value);

  if (clamp_){
    _value = std::max( clampMin_, _value );
    _value = std::min( clampMax_, _value );
  }

  if (repeat_){
    _value = ( (_value - currentMin_) / (currentMax_-currentMin_) ) * repeatMax_;
  }else{

    if (center_)
      _value -= (currentMax_-currentMin_) / 2.0; //TODO:thats wrong

    if (scale_)
      _value = (_value - currentMin_) / (currentMax_-currentMin_);

    if (center_)
      _value += 0.5;
  }

  return _value;
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::initValues()
{

  //get min/max values
  currentMin_ = FLT_MAX;
  currentMax_ = FLT_MIN;

  for ( uint i=0; i < values_.size(); i++){

    double tranformed = values_[i];

    if (absolute_)
      tranformed = fabs(tranformed);

    if (clamp_){
      tranformed = std::max( clampMin_, tranformed );
      tranformed = std::min( clampMax_, tranformed );
    }

    currentMin_ = std::min(currentMin_, tranformed );
    currentMax_ = std::max(currentMax_, tranformed );
  }
}

//------------------------------------------------------------------------------

void QwtFunctionPlot::replot()
{

  initValues();

  //create intervals
  const int intervalCount = 100;

  QwtArray<QwtDoubleInterval> intervals(intervalCount);
  QwtArray<double> count(intervalCount);
  std::vector< QColor > colors;

  double pos = transform(min_);
  double width = ( transform(max_) - transform(min_) ) / intervalCount;

  QColor lastColor = Qt::black;

  for ( int i = 0; i < (int)intervals.size(); i++ )
  {
    intervals[i] = QwtDoubleInterval(pos, pos + width);
    pos += width;

    //compute a color for the given interval
    if (image_ != 0){
      double intervalCenter = pos + (width/2.0);

      if (intervalCenter > 1.0 && !repeat_){
        colors.push_back( lastColor );
        continue;
      }

      if ( intervalCenter < 0.0){
        colors.push_back( lastColor );
        continue;
      }

      if ( intervalCenter > 1.0){
        intervalCenter -= floor(intervalCenter);
      }

      int val = int( intervalCenter * image_->width() );

      colors.push_back( QColor( image_->pixel(val, 0) ) );

      lastColor = colors.back();
    }
  }

  //sort values into intervals
  for ( uint i=0; i < values_.size(); i++)
    for ( int j = 0; j < (int)intervals.size(); j++ )
      if ( intervals[j].contains( transform(values_[i])  ) )
        count[j]++;

  //get max Count for scaling the y-axis
  double maxCount = 0;

  for ( int i = 0; i < (int)intervals.size(); i++ )
    maxCount = std::max(maxCount, count[i]);


  histogram_->setData(QwtIntervalData(intervals, count));
  histogram_->setColors(colors);
  histogram_->attach(qwtPlot);

  qwtPlot->setAxisScale(QwtPlot::yLeft, 0.0, maxCount);
  qwtPlot->setAxisScale(QwtPlot::xBottom, transform(min_), transform(max_));

  qwtPlot->setAxisTitle(QwtPlot::yLeft,   "count" );
  qwtPlot->setAxisTitle(QwtPlot::xBottom, "values" );

  //define this scaling as the zoomBase
  plot_zoomer_->setZoomBase();

  // an plot it
  qwtPlot->replot();
}





//=============================================================================
} // namespace db
//=============================================================================

#endif // WITH_QWT
