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
*   $Revision: 13581 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2012-01-30 14:22:52 +0100 (Mo, 30 Jan 2012) $                     *
*                                                                            *
\*===========================================================================*/

#include "ImageDialog.hh"
#include <QtGui>
#include <iostream>

ImageDialog::ImageDialog(QImage _image, QWidget *parent) :
        QDialog(parent),
        image_(_image),
        dragging_(false)
{
  setupUi(this);

  image->setPixmap(QPixmap::fromImage(image_).scaled(QSize(400,400),Qt::KeepAspectRatio,Qt::SmoothTransformation));

  // Set the correct boundaries for the spinboxes
  minX->setMaximum(image_.width());
  maxX->setMaximum(image_.width());
  maxX->setValue(image_.width());
  minY->setMaximum(image_.height());
  maxY->setMaximum(image_.height());
  maxY->setValue(image_.height());

  // Connect the spin boxes
  connect(minX,SIGNAL(valueChanged(int )),this,SLOT(slidersChanged()));
  connect(maxX,SIGNAL(valueChanged(int )),this,SLOT(slidersChanged()));
  connect(minY,SIGNAL(valueChanged(int )),this,SLOT(slidersChanged()));
  connect(maxY,SIGNAL(valueChanged(int )),this,SLOT(slidersChanged()));

  connect(image,SIGNAL(mouseButtonMoveEvent ( QPoint ))   ,this,SLOT(mouseButtonMoveEvent ( QPoint )));
  connect(image,SIGNAL(mouseButtonPressEvent ( QPoint ))  ,this,SLOT(mouseButtonPressEvent ( QPoint )));
  connect(image,SIGNAL(mouseButtonReleaseEvent ( QPoint )),this,SLOT(mouseButtonReleaseEvent ( QPoint )));

  setMouseTracking ( true);

  grabMouse();

}


void ImageDialog::slidersChanged() {

  // Copy to internal buffer
  buffer_ = QPixmap::fromImage(image_);

  QPainter painter( &buffer_);

  QPen pen(Qt::red);

  // Calculate a reasonable pen size. We scale to 400x400 (maximal)
  // So we try to have at least two pixel line width in the result.
  int penwidth = std::max( image_.width()/400,image_.height()/400) * 2;
  pen.setWidth(penwidth);
  painter.setPen( pen );

  painter.setBrush(QBrush(Qt::red,Qt::NoBrush));

  const int minXVal = minX->value();
  const int maxXVal = maxX->value();
  const int minYVal = minY->value();
  const int maxYVal = maxY->value();

  painter.drawRect( minXVal , minYVal , (maxXVal-minXVal) , (maxYVal-minYVal) );

  painter.end();

  image->setPixmap(buffer_.scaled(QSize(400,400),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void ImageDialog::mouseButtonMoveEvent ( QPoint _p) {
  if ( dragging_ ) {

    // Calculate the right values for the position.
    // The image is scaled to 400x400
    // But the aspect ratio is kept. So we need to calculate the new sizes.

    // Calculate the free space above and before the real image
    int xoffset = (400 - image->pixmap()->width()) / 2;
    int yoffset = (400 - image->pixmap()->height()) / 2;

    // Calculate the scaling factor that is used to fit the image to the widget
    const int scale = image_.width()/image->pixmap()->width();

    int newXMin = (std::min(dragStartPoint_.x(),_p.x()) - xoffset ) * scale;
    int newXMax = (std::max(dragStartPoint_.x(),_p.x()) - xoffset ) * scale;

    int newYMin = (std::min(dragStartPoint_.y(),_p.y()) - yoffset ) * scale;
    int newYMax = (std::max(dragStartPoint_.y(),_p.y()) - yoffset ) * scale;

    minX->blockSignals(true);
    maxX->blockSignals(true);
    minY->blockSignals(true);
    maxY->blockSignals(true);

    minX->setValue(newXMin);
    maxX->setValue(newXMax);
    minY->setValue(newYMin);
    maxY->setValue(newYMax);

    minX->blockSignals(false);
    maxX->blockSignals(false);
    minY->blockSignals(false);
    maxY->blockSignals(false);

    slidersChanged();
  }
}

void ImageDialog::mouseButtonPressEvent (  QPoint _p ) {
  dragStartPoint_ = _p;
  dragging_       = true;
}

void ImageDialog::mouseButtonReleaseEvent (  QPoint _p ) {
  dragging_ = false;
}





