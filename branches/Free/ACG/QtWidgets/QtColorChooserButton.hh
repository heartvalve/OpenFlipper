/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
 * ColorChooserButton.hh
 *
 *  Created on: Sep 15, 2011
 *      Author: ebke
 */

#ifndef QTCOLORCHOOSERBUTTON_HH_
#define QTCOLORCHOOSERBUTTON_HH_

#include <QPushButton>
#include <QColor>
#include "../Config/ACGDefines.hh"

class ACGDLLEXPORT QtColorChooserButton: public QPushButton {
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)

    public:

        explicit QtColorChooserButton(QWidget *parent=0);
        explicit QtColorChooserButton(const QString &text, QWidget *parent=0);
        QtColorChooserButton(const QIcon& icon, const QString &text, QWidget *parent=0);

        virtual ~QtColorChooserButton();

        const QColor &color() const { return color_; }
        void setColor(const QColor &color) {
            color_ = color;
        }

        void paintEvent(QPaintEvent *);

    private:
        void init();

    public Q_SLOTS:
        void onClick();

    signals:
        void colorChanged(QColor _newcolor );

    private:
        QColor color_;
};

#endif /* QTCOLORCHOOSERBUTTON_HH_ */
