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

#include <QDialog>

// qmake users have to include pc
#include <ui_QwtFunctionPlotBase.hh>

// ACGMake users have to include
// #include "QtFunctionPlotBase.hh"

#if QWT_VERSION >= 0x060000
 #include <ACG/QtWidgets/QwtHistogramm.hh>
#else
 #include "HistogramItem.hh"
#endif

#include <vector>
#include "TextureParameters.hh"


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG
{

//== CLASS DEFINITION =========================================================



class QwtFunctionPlot : public QDialog, public Ui::QwtFunctionPlotBase
{
  Q_OBJECT
public:

  /// Default constructor
  QwtFunctionPlot( QWidget*    _parent = 0 );

  /// Destructor
  ~QwtFunctionPlot() {}

  // set the function to plot
  void setFunction( std::vector<double>& _values );

  /// Set all parameters as one block
  void setParameters(const TexParameters& _parameters);

  /// Set the parameters separately
  void setParameters(bool repeat, double repeatMax,
                     bool clamp,  double clampMin, double clampMax,
                     bool center,
                     bool absolute,
                     bool scale);

  /** The function plot accesses an image to get colors from them.
   *  The colors are equivalent to the rendered color textures.
   *
   * @param _image Image that is used to take the colors from
   */
  void setImage(QImage* _image);

public slots:
  void zoomIn();
  void zoomOut();
  void clamp();
  
  void replot();

private:

  QwtPlotZoomer* plot_zoomer_;

  QwtPlotMarker* clampMinMarker_;
  QwtSymbol*     minSymbol_;

  QwtPlotMarker* clampMaxMarker_;
  QwtSymbol*     maxSymbol_;

  // Histogramm Plot
#if QWT_VERSION >= 0x060000
  Histogram* histogram_;
#else
  HistogramItem* histogram_;
#endif

  std::vector<double> values_;

  QImage* image_;

  TexParameters parameters_;

  double min_,max_;

};


//=============================================================================
} // namespace ACG

#endif // WITH_QWT

//=============================================================================
#endif // ACG_QWTFUNCTIONPLOT_HH defined
//=============================================================================

