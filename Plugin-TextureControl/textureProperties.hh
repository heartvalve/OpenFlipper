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




#include "ui_textureProperties.hh"
#include <QtGui>

#ifdef QWT
  #include <qwt_plot_curve.h>
#endif

class texturePropertiesWidget : public QDialog, public Ui::Dialog
{
  Q_OBJECT

  public:
    texturePropertiesWidget(QWidget *parent = 0);

    void setOriginalData(std::vector< double > _x, std::vector< double > _y  );

  private:
#ifdef QWT
    QwtPlot* originalDataHistogram_;
    QwtPlotCurve histogramCurve_;
#endif

};

