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
 *   $Author$                                                       *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

/*
 * SpinBoxEventFilter.hh
 *
 *  Created on: Aug 1, 2013
 *      Author: ebke
 */

#ifndef SPINBOXEVENTFILTER_HH_
#define SPINBOXEVENTFILTER_HH_

#include <QObject>
#include <QTimer>
#include <set>

class SpinBoxEventFilter : public QObject {
    Q_OBJECT

    public:
        SpinBoxEventFilter(QObject *parent = 0);
        virtual ~SpinBoxEventFilter();

        bool eventFilter(QObject *object, QEvent *event);
        bool isScrolling();

        void hookUpToWidgetTree(QWidget *widget);

        void registerScrollArea(QWidget *scrollArea);

    private:
        void setScrolling();

    private slots:
        void unsetScrolling();

    private:
        std::set<QWidget*> scrollAreas;
        bool scrolling;
        QTimer scrollingTimer;
};

#endif /* SPINBOXEVENTFILTER_HH_ */
