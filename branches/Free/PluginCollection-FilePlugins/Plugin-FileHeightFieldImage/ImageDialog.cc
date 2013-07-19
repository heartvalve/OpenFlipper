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

#include <iostream>

ImageDialog::ImageDialog(QImage _image, QWidget *parent) :
        QDialog(parent),
        imageWidth_(_image.width() ),
        imageHeight_(_image.height() ),
        dragging_(false)
{
  setupUi(this);

  pixmap_ = QPixmap::fromImage(_image).scaled(QSize(400,400),Qt::KeepAspectRatio,Qt::SmoothTransformation);

  image->setPixmap(pixmap_);

  // Set the correct boundaries for the spinboxes
  minX->setMaximum(imageWidth_);
  maxX->setMaximum(imageWidth_);
  maxX->setValue(imageWidth_);
  minY->setMaximum(imageHeight_);
  maxY->setMaximum(imageHeight_);
  maxY->setValue(imageHeight_);

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
  buffer_ = pixmap_;

  QPainter painter( &buffer_);

  QPen pen(Qt::red);

  pen.setWidth(2);
  painter.setPen( pen );

  painter.setBrush(QBrush(Qt::red,Qt::NoBrush));

  const int minXVal = minX->value();
  const int maxXVal = maxX->value();
  const int minYVal = minY->value();
  const int maxYVal = maxY->value();

  // Calculate the scaling factor that is used to fit the image to the widget
  const double scale = (double)pixmap_.width() / (double)imageWidth_;

  // Working on the actual pixmap -> no offsets!
  painter.drawRect( minXVal * scale  , minYVal  * scale , (maxXVal-minXVal)  * scale, (maxYVal-minYVal)  * scale);

  painter.end();

  image->setPixmap(buffer_);
}

void ImageDialog::mouseButtonMoveEvent ( QPoint _p) {
  if ( dragging_ ) {

    // Calculate the right values for the position.
    // The image is scaled to 400x400
    // But the aspect ratio is kept. So we need to calculate the new sizes.



    // Calculate the free space above and before the real image
    // As we are working on the pixmap widget, we need to consider these offsets to calculate
    // coordinates inside the real pixmap.
    int xoffset = (400 - image->pixmap()->width() ) / 2;
    int yoffset = (400 - image->pixmap()->height()) / 2;

    // Calculate the scaling factor that is used to fit the image to the widget
    const double scale = (double)imageWidth_ / (double)image->pixmap()->width();

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





