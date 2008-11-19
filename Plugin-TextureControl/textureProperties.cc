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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




#include "textureProperties.hh"

#include <qwt_plot_grid.h>
#include <qwt_text.h>
#include <qwt_plot.h>
#include <float.h>

#include <QtGui>

texturePropertiesWidget::texturePropertiesWidget(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    QGridLayout* layout = new QGridLayout( originalData);
    originalDataHistogram_ = new QwtPlot(0);

    layout->addWidget( originalDataHistogram_ , 0,0  );

    QwtText axis_title_x("value");
//     axis_title_x.setFont(arial);
    QwtText axis_title_y("count");
//     axis_title_y.setFont(arial);

    originalDataHistogram_->setAxisTitle(QwtPlot::xBottom, axis_title_x);
    originalDataHistogram_->setAxisTitle(QwtPlot::yLeft, axis_title_y);
    originalDataHistogram_->setCanvasBackground(Qt::white);

    QwtPlotGrid *grid2 = new QwtPlotGrid;
    grid2->enableYMin(true);
    grid2->enableXMin(true);
    grid2->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
    grid2->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid2->attach(originalDataHistogram_);

    QwtText curve_title("Delay Spread Histogram");
//     curve_title.setFont(arial);
    histogramCurve_.setTitle(curve_title);
    histogramCurve_.attach(originalDataHistogram_);








//     connect(distanceCheckBox,SIGNAL(stateChanged ( int ) ) , this , SLOT (slotDistanceCheckbox(int)) );
//     connect(normalCheckBox,SIGNAL(stateChanged ( int ) ) , this , SLOT (slotNormalCheckbox(int)) );


}

void texturePropertiesWidget::setOriginalData(std::vector< double > _x, std::vector< double > _y  ) {
  double minx = FLT_MIN;
  double maxx = FLT_MIN;
  double miny = FLT_MAX;
  double maxy = FLT_MIN;

  for ( uint i = 0 ; i < _x.size(); ++ i ) {
    minx = std::min(minx,_x[0]);
    maxx = std::max(maxx,_x[0]);
    miny = std::min(miny,_y[0]);
    maxy = std::max(maxy,_y[0]);
  }
  histogramCurve_.setData(&_x[0], &_y[0], _x.size());
  originalDataHistogram_->replot();
}
