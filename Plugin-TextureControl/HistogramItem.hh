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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#ifndef HISTOGRAM_ITEM_H
#define HISTOGRAM_ITEM_H

#include <qglobal.h>
#include <QColor>

#include "qwt_plot_item.h" 

class QwtIntervalData;
class QString;

/** \brief Histogram plotting widget
 *
 * This widget can be used to render a plot. It provides a Qt widget with
 * histogram bars. The data is supplied as QwtIntervalData via HistogramItem::setData().
 * Additionally you can set colors for each bar, which are provided via
 * HistogramItem::setColors()
 *
 *
 * !!!!!!!!!!! This class is deprecated !!!!!!!!!!!
 * !!!!!!!!! Use the one from ACG/QtWidgets which is for qwt6 !!!!!!!!!!!!!!!!!!
 */
class HistogramItem: public QwtPlotItem
{
public:

    /// Constructor
    explicit HistogramItem(const QString &title = QString::null);
    /// Constructor
    explicit HistogramItem(const QwtText &title);

    /// Destructor
    virtual ~HistogramItem();

    /** \brief set data to render
     *
     * @param data data provided as QwtIntervalData
     */
    void setData(const QwtIntervalData &data);
    const QwtIntervalData &data() const;

    /** \brief Set colors
     *
     * @param _colors A vector of colors. One for each interval in the same order.
     */
    void setColors( std::vector< QColor >& _colors);


    /// Function hat will return the datas bounding rectangle (for rendering)
    virtual QwtDoubleRect boundingRect() const;

    virtual int rtti() const;

    /// The actual draw function, drawing the bars inside the plot widget.
    virtual void draw(QPainter *, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const;

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
    /// Draws a single bar
    virtual void drawBar(QPainter *, Qt::Orientation o, const QRect &) const;

private:
    inline QColor color(uint i) const;

    void init();

    class PrivateData;
    PrivateData *d_data;
};

#endif
