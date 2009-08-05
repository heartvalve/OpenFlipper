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

#ifndef HISTOGRAM_ITEM_H
#define HISTOGRAM_ITEM_H

#include <qglobal.h>
#include <qcolor.h>

#include "qwt_plot_item.h" 

class QwtIntervalData;
class QString;

class HistogramItem: public QwtPlotItem
{
public:
    explicit HistogramItem(const QString &title = QString::null);
    explicit HistogramItem(const QwtText &title);
    virtual ~HistogramItem();

    void setData(const QwtIntervalData &data);
    const QwtIntervalData &data() const;

    void setColors( std::vector< QColor >& _colors);

    virtual QwtDoubleRect boundingRect() const;

    virtual int rtti() const;

    virtual void draw(QPainter *, const QwtScaleMap &xMap, 
        const QwtScaleMap &yMap, const QRect &) const;

    void setBaseline(double reference);
    double baseline() const;

    enum HistogramAttribute
    {
        Auto = 0,
        Xfy = 1
    };

    void setHistogramAttribute(HistogramAttribute, bool on = true);
    bool testHistogramAttribute(HistogramAttribute) const;

protected:
    virtual void drawBar(QPainter *,
        Qt::Orientation o, const QRect &) const;

private:
    QColor color(uint i) const;

    void init();

    class PrivateData;
    PrivateData *d_data;
};

#endif
