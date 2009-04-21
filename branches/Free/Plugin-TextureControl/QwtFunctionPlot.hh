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


//== INCLUDES =================================================================

#include <QtGui>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_scale_engine.h>

// qmake users have to includepc
#include <ui_QwtFunctionPlotBase.hh>

// ACGMake users have to include
// #include "QtFunctionPlotBase.hh"


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
  QwtFunctionPlot( QWidget*    _parent = 0 ):
    QDialog( _parent ),
    Ui::QwtFunctionPlotBase()
  {
    setupUi( this );

    legend_ = new QwtLegend();

    // make legend checkable
    legend_->setItemMode( QwtLegend::CheckableItem );

    qwtPlot->insertLegend(legend_);

    plot_zoomer_ = new QwtPlotZoomer( qwtPlot->canvas());
      
    // connect for legend handling
    connect(qwtPlot, SIGNAL( legendChecked(QwtPlotItem*, bool)),
	    this,    SLOT( slotLegendChecked(QwtPlotItem*, bool)) );

    // delete widget on close
    setAttribute(Qt::WA_DeleteOnClose, true);

    // init scale engine
    linear_scale_engine_ = new QwtLinearScaleEngine;
    log10_scale_engine_ = new QwtLog10ScaleEngine;
  }


  /// Destructor
  ~QwtFunctionPlot() {}

  // clear plot
  void clear() { functions_.clear(); }

  void add_function( std::vector<double>& _x,
		     std::vector<double>& _y,
		     const char*          _title="",
		     QColor               _col = QColor( 255,255,255 ) );

  void add_function( std::vector<double>& _y,
		     const char*          _title="",
		     QColor               _col = QColor( 255,255,255 ) );

  // switch into function mode
  void function_mode();

  // switch into histogram mode
  void histogram_mode();
  
  // update pen size
  void update_pens(int _inc);

  // set scaling
  void linear_scaling();
  void logarithmic_scaling();

protected slots:

  void slotLegendChecked(QwtPlotItem* _plot_item, bool _visible);

  virtual void keyPressEvent ( QKeyEvent* _event );

private:

  void update_zoom_base();

private:
   std::vector< QwtPlotCurve* > functions_;

  QwtPlotZoomer* plot_zoomer_;
  QwtLegend*     legend_;

  QwtLinearScaleEngine* linear_scale_engine_;
  QwtLog10ScaleEngine*  log10_scale_engine_;
  
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_QWTFUNCTIONPLOT_HH defined
//=============================================================================

