/*
 * ColorChooserButton.cc
 *
 *  Created on: Sep 15, 2011
 *      Author: ebke
 */

#include "ColorChooserButton.hh"

#include <qstyleoption.h>
#include <qstylepainter.h>
#include <qcolordialog.h>

#include <iostream>

ColorChooserButton::ColorChooserButton(QWidget *parent) :
        QPushButton(parent), color_(0xE0, 0x20, 0x20) {

    init();
}
ColorChooserButton::ColorChooserButton(const QString &text, QWidget *parent) :
        QPushButton(text, parent), color_(0xE0, 0x20, 0x20) {

    init();
}
ColorChooserButton::ColorChooserButton(const QIcon& icon, const QString &text, QWidget *parent) :
        QPushButton(icon, text, parent), color_(0xE0, 0x20, 0x20) {

    init();
}

ColorChooserButton::~ColorChooserButton() {
}

void ColorChooserButton::init() {
    connect(this, SIGNAL( clicked() ), this, SLOT( onClick() ) );
}

void ColorChooserButton::onClick() {
    QColor newColor = QColorDialog::getColor(color_, this, "Pick Color", QColorDialog::ShowAlphaChannel);
    if (newColor.isValid()) color_ = newColor;
}

void ColorChooserButton::paintEvent(QPaintEvent *ev) {
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
