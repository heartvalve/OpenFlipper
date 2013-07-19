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
// CLASS QwtFunctionPlot
//
//
// Author:  David Bommes <bommes@cs.rwth-aachen.de>
//
// Version: $Revision: 1$
// Date:    $Date: 200X-XX-XX$
//
//=============================================================================


#ifndef ACG_QWTFUNCTIONPLOT_HH
#define ACG_QWTFUNCTIONPLOT_HH

#ifdef WITH_QWT

//== INCLUDES =================================================================


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_marker.h>

#include <ui_QwtFunctionPlotBase.hh>

#include <ACG/QtWidgets/QwtHistogramm.hh>
#include <vector>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

//== CLASS DEFINITION =========================================================



class QwtFunctionPlot : public QDialog, public Ui::QwtFunctionPlotBase
{
  Q_OBJECT
public:

  /// Default constructor
  QwtFunctionPlot( QWidget*    _parent = 0 );

  /// Destructor
  ~QwtFunctionPlot() {}

  /// set the function to plot
  void setFunction(const std::vector<double>& _values );

  void setMinMax(double _min, double _max);

public slots:
  
  void replot();

private:

  QwtPlotZoomer* plot_zoomer_;

  QwtPlotMarker* clampMinMarker_;
  QwtSymbol*     minSymbol_;

  QwtPlotMarker* clampMaxMarker_;
  QwtSymbol*     maxSymbol_;

  /// Histogram Plot
  Histogram*     histogram_;

  /// Values that will define the function to plot
  std::vector<double> values_;

  double min_,max_;
};


//=============================================================================
#endif // WITH_QWT
//=============================================================================
#endif // ACG_QWTFUNCTIONPLOT_HH defined
//=============================================================================

