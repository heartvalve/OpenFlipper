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

#include <QtGui>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_engine.h>

// qmake users have to includepc
#include <ui_QwtFunctionPlotBase.hh>

// ACGMake users have to include
// #include "QtFunctionPlotBase.hh"

#include "HistogramItem.hh"
#include <vector>

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

  void setParameters(bool repeat, double repeatMax,
                     bool clamp,  double clampMin, double clampMax,
                     bool center,
                     bool absolute,
                     bool scale);

  void setImage(QImage* _image);

  void replot();

private:

  void initValues();

  double transform( double _value );

  QwtPlotZoomer* plot_zoomer_;

  //function data
  HistogramItem* histogram_;

  std::vector<double> values_;

  QImage* image_;

  bool   repeat_;
  double repeatMax_;
  bool   clamp_;
  double clampMin_;
  double clampMax_;
  bool   center_;
  bool   absolute_;
  bool   scale_;

  double min_;
  double max_;

  double currentMin_;
  double currentMax_;

};


//=============================================================================
} // namespace ACG

#endif // WITH_QWT

//=============================================================================
#endif // ACG_QWTFUNCTIONPLOT_HH defined
//=============================================================================

