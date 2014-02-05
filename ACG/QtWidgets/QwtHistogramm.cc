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


#ifdef WITH_QWT

// Include the plot header to get the qwt version
#include <qwt_plot.h>

// This file is for QWT >= 6.0 only
#if QWT_VERSION >= 0x060000

#include <qwt_plot_histogram.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>

#include <QString>
#include <QPainter>

#include <iostream>


#include "QwtHistogramm.hh"

class Histogram::PrivateData {

public:

  PrivateData() :
          attributes(Histogram::Auto),
          data(0),
          reference(0.0)
  {}

  int attributes;
  QwtIntervalSeriesData* data;
  std::vector<QColor> colors_;
  double reference;

};

Histogram::Histogram(const QwtText &title) :
        QwtPlotItem(title)
{
  init();
}

Histogram::Histogram(const QString &title) :
        QwtPlotItem(QwtText(title))
{
  init();
}

Histogram::~Histogram()
{
  delete d_data;
}

void Histogram::init()
{
  d_data = new PrivateData();

  setItemAttribute(QwtPlotItem::AutoScale, true);
  setItemAttribute(QwtPlotItem::Legend, true);

  setZ(20.0);
}

void Histogram::setBaseline(double reference)
{
  if (d_data->reference != reference) {
    d_data->reference = reference;
    itemChanged();
  }
}

double Histogram::baseline() const
{
  return d_data->reference;
}

void Histogram::setData(QwtIntervalSeriesData* data)
{
  d_data->data = data;
  itemChanged();
}

const QwtIntervalSeriesData* Histogram::data() const
{
  return d_data->data;
}

void Histogram::setColors(std::vector<QColor>& _colors)
{
  d_data->colors_ = _colors;
  itemChanged();
}

QColor Histogram::color(uint i) const
{
  if (i < d_data->colors_.size())
    return d_data->colors_[i];
  else
    return Qt::darkBlue;
}

QRectF Histogram::boundingRect() const
{
  QRectF rect = d_data->data->boundingRect();
  if (!rect.isValid())
    return rect;

  if (d_data->attributes & Xfy) {
    rect = QRectF(rect.y(), rect.x(), rect.height(), rect.width());

    if (rect.left() > d_data->reference)
      rect.setLeft(d_data->reference);
    else if (rect.right() < d_data->reference)
      rect.setRight(d_data->reference);
  } else {
    if (rect.bottom() < d_data->reference)
      rect.setBottom(d_data->reference);
    else if (rect.top() > d_data->reference)
      rect.setTop(d_data->reference);
  }

  return rect;
}


int Histogram::rtti() const
{
  return QwtPlotItem::Rtti_PlotHistogram;
}

void Histogram::setHistogramAttribute(HistogramAttribute attribute, bool on)
{
  if (bool(d_data->attributes & attribute) == on)
    return;

  if (on)
    d_data->attributes |= attribute;
  else
    d_data->attributes &= ~attribute;

  itemChanged();
}

bool Histogram::testHistogramAttribute(HistogramAttribute attribute) const
{
  return d_data->attributes & attribute;
}

void Histogram::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &) const
{
  const QwtIntervalSeriesData &iData = *(d_data->data);

  const int x0 = xMap.transform(baseline());
  const int y0 = yMap.transform(baseline());

  for (int i = 0; i < (int) iData.size(); i++) {
    if (d_data->attributes & Histogram::Xfy) {
      const int x2 = xMap.transform(iData.sample(i).value);
      if (x2 == x0)
        continue;

      int y1 = yMap.transform(iData.sample(i).interval.minValue());
      int y2 = yMap.transform(iData.sample(i).interval.maxValue());
      if (y1 > y2)
        qSwap(y1, y2);

      if (i < (int) iData.size() - 2) {
        const int yy1 = yMap.transform(iData.sample(i + 1).interval.minValue());
        const int yy2 = yMap.transform(iData.sample(i + 1).interval.maxValue());

        if (y2 == qMin(yy1, yy2)) {
          const int xx2 = xMap.transform(iData.sample(i + 1).interval.minValue());
          if (xx2 != x0 && ((xx2 < x0 && x2 < x0) || (xx2 > x0 && x2 > x0))) {
            // One pixel distance between neighbored bars
            y2++;
          }
        }
      }

      painter->setPen(QPen(color(i)));

      drawBar(painter, Qt::Horizontal, QRect(x0, y1, x2 - x0, y2 - y1));
    } else {
      const int y2 = yMap.transform(iData.sample(i).value);
      if (y2 == y0)
        continue;

      int x1 = xMap.transform(iData.sample(i).interval.minValue());
      int x2 = xMap.transform(iData.sample(i).interval.maxValue());
      if (x1 > x2)
        qSwap(x1, x2);

      if (i < (int) iData.size() - 2) {
        const int xx1 = xMap.transform(iData.sample(i + 1).interval.minValue());
        const int xx2 = xMap.transform(iData.sample(i + 1).interval.maxValue());

        if (x2 == qMin(xx1, xx2)) {
          const int yy2 = yMap.transform(iData.sample(i + 1).value);
          if (yy2 != y0 && ((yy2 < y0 && y2 < y0) || (yy2 > y0 && y2 > y0))) {
            // One pixel distance between neighbored bars
            x2--;
          }
        }
      }

      painter->setPen(QPen(color(i)));

      drawBar(painter, Qt::Vertical, QRect(x1, y0, x2 - x1, y2 - y0));
    }
  }
}

void Histogram::drawBar(QPainter *painter, Qt::Orientation, const QRect& rect) const
{
  painter->save();

  const QColor color(painter->pen().color());
  const QRect r = rect.normalized();

  const int factor = 125;
  const QColor light(color.light(factor));
  const QColor dark(color.dark(factor));

  painter->setBrush(color);
  painter->setPen(Qt::NoPen);
  QwtPainter::drawRect(painter, r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2);
  painter->setBrush(Qt::NoBrush);

  painter->setPen(QPen(light, 2));

  QwtPainter::drawLine(painter, r.left() + 1, r.top() + 2, r.right() + 1, r.top() + 2);

  painter->setPen(QPen(dark, 2));

  painter->setPen(QPen(light, 1));

  QwtPainter::drawLine(painter, r.left(), r.top() + 1, r.left(), r.bottom());
  QwtPainter::drawLine(painter, r.left() + 1, r.top() + 2, r.left() + 1, r.bottom() - 1);

  painter->setPen(QPen(dark, 1));

  QwtPainter::drawLine(painter, r.right() + 1, r.top() + 1, r.right() + 1, r.bottom());
  QwtPainter::drawLine(painter, r.right(), r.top() + 2, r.right(), r.bottom() - 1);

  painter->restore();
}

#endif

#endif // WITH_QWT
