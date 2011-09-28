/*
 * ColorChooserButton.hh
 *
 *  Created on: Sep 15, 2011
 *      Author: ebke
 */

#ifndef COLORCHOOSERBUTTON_HH_
#define COLORCHOOSERBUTTON_HH_

#include <qpushbutton.h>
#include <qcolor.h>

class ColorChooserButton: public QPushButton {
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)

    public:

        explicit ColorChooserButton(QWidget *parent=0);
        explicit ColorChooserButton(const QString &text, QWidget *parent=0);
        ColorChooserButton(const QIcon& icon, const QString &text, QWidget *parent=0);

        virtual ~ColorChooserButton();

        const QColor &color() const { return color_; }
        void setColor(const QColor &color) {
            color_ = color;
        }

        void paintEvent(QPaintEvent *);

    private:
        void init();

    public Q_SLOTS:
        void onClick();

    private:
        QColor color_;
};

#endif /* COLORCHOOSERBUTTON_HH_ */
