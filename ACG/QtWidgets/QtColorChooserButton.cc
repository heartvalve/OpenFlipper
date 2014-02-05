/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


/*
 * ColorChooserButton.cc
 *
 *  Created on: Sep 15, 2011
 *      Author: ebke
 */

#include "QtColorChooserButton.hh"

#include <QStyleOption>
#include <QStylePainter>
#include <QColorDialog>

#include <iostream>

QtColorChooserButton::QtColorChooserButton(QWidget *parent) :
        QPushButton(parent), color_(0xE0, 0x20, 0x20) {

    init();
}
QtColorChooserButton::QtColorChooserButton(const QString &text, QWidget *parent) :
        QPushButton(text, parent), color_(0xE0, 0x20, 0x20) {

    init();
}
QtColorChooserButton::QtColorChooserButton(const QIcon& icon, const QString &text, QWidget *parent) :
        QPushButton(icon, text, parent), color_(0xE0, 0x20, 0x20) {

    init();
}

QtColorChooserButton::~QtColorChooserButton() {
}

void QtColorChooserButton::init() {
    connect(this, SIGNAL( clicked() ), this, SLOT( onClick() ) );
}

void QtColorChooserButton::onClick() {
    QColor newColor = QColorDialog::getColor(color_, this, "Pick Color", QColorDialog::ShowAlphaChannel);
    if (newColor.isValid()) {
      color_ = newColor;
      emit colorChanged(color_);
    }
}

void QtColorChooserButton::paintEvent(QPaintEvent *ev) {
    QStyleOptionButton buttonOptions;
    initStyleOption(&buttonOptions);
    QStylePainter painter(this);

    const int textWd = buttonOptions.fontMetrics.width(buttonOptions.text);
    QRect textRect = buttonOptions.rect;
    textRect.setWidth(std::min(textRect.width(), textWd));
    painter.drawItemText(textRect, Qt::TextSingleLine | Qt::TextShowMnemonic | Qt::AlignVCenter,
                         this->palette(), this->isEnabled(), buttonOptions.text, QPalette::ButtonText);

    buttonOptions.rect.adjust(textWd, 0, 0, 0);
    painter.drawControl(QStyle::CE_PushButtonBevel, buttonOptions);

    QRect colorRect = this->style()->subElementRect(QStyle::SE_PushButtonFocusRect, &buttonOptions, this);
    QStyleOptionFrameV3 frameOptions;
    frameOptions.state = QStyle::State_Sunken;
    frameOptions.rect = colorRect;
    frameOptions.features = QStyleOptionFrameV2::None;

    /*
     * Draw checker board pattern.
     */
    if (this->isEnabled()) {
        static const int checkerSize = 7;
        static const QColor checkerColA(0xFF, 0xFF, 0xFF);
        static const QColor checkerColB(0x30, 0x30, 0x30);
        painter.setClipRect(colorRect, Qt::IntersectClip);
        for (int x = 0; x < colorRect.width() / checkerSize + 1; ++x) {
            for (int y = 0; y < colorRect.height() / checkerSize + 1; ++y) {
                painter.fillRect(colorRect.x() + x * checkerSize, colorRect.y() + y * checkerSize,
                                 checkerSize, checkerSize,
                                 (x % 2 == y % 2) ? checkerColA : checkerColB);
            }
        }

        painter.fillRect(colorRect, color_);
    }
    painter.drawPrimitive(QStyle::PE_FrameButtonBevel, frameOptions);
}
