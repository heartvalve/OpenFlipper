/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
 * SpinBoxEventFilter.cc
 *
 *  Created on: Aug 1, 2013
 *      Author: ebke
 */

//#define VERBOSE_DEBUGGING_OUTPUT

#include "SpinBoxEventFilter.hh"

#include <QAbstractSpinBox>
#include <QAbstractSlider>
#include <QEvent>
#include <QWidget>
#include <QObjectList>
#include <QMetaEnum>
#include <stack>

#ifdef VERBOSE_DEBUGGING_OUTPUT
#include <iostream>
#endif

SpinBoxEventFilter::SpinBoxEventFilter(QObject *parent) :
    QObject(parent), scrolling(false) {

    scrollingTimer.setSingleShot(true);
    scrollingTimer.setInterval(500);

    connect(&scrollingTimer, SIGNAL( timeout() ), this, SLOT( unsetScrolling() ));
}

SpinBoxEventFilter::~SpinBoxEventFilter() {
}

bool SpinBoxEventFilter::eventFilter(QObject *object, QEvent *event) {

    if (event->type() != QEvent::Wheel) return QObject::eventFilter(object, event);

    QWidget *widget = qobject_cast<QWidget*>(object);

    if (scrollAreas.find(widget) != scrollAreas.end()) {
#ifdef VERBOSE_DEBUGGING_OUTPUT
        std::cout << "Reset isScrolling." << std::endl;
#endif
        setScrolling();
        return QObject::eventFilter(object, event);
    }

    if (!isScrolling() && widget->isEnabled()) {
#ifdef VERBOSE_DEBUGGING_OUTPUT
        std::cout << "Not scrolling. Letting wheel event pass." << std::endl;
#endif
        return QObject::eventFilter(object, event);
    }

    setScrolling();
    event->ignore();
#ifdef VERBOSE_DEBUGGING_OUTPUT
    std::cout << "Swallowing wheel event." << std::endl;
#endif
    return true;

//#ifdef VERBOSE_DEBUGGING_OUTPUT
//    std::cout << "Not a spin box. Letting wheel event pass." << std::endl;
//#endif
//    return QObject::eventFilter(object, event);
}

inline bool SpinBoxEventFilter::isScrolling() {
    return scrolling;
}

void SpinBoxEventFilter::hookUpToWidgetTree(QWidget *root_widget) {
    std::stack<QWidget*> dfs;
    dfs.push(root_widget);

    while (!dfs.empty()) {
        QWidget *cur = dfs.top(); dfs.pop();

        cur->installEventFilter(this);

        const QObjectList &children = cur->children();
        for (QObjectList::const_iterator it = children.begin(), it_end = children.end();
                it != it_end; ++it) {

            QWidget *widget = qobject_cast<QWidget*>(*it);
            if (widget)
                dfs.push(widget);
        }
    }
}

void SpinBoxEventFilter::registerScrollArea(QWidget *scrollArea) {
    scrollAreas.insert(scrollArea);
    scrollArea->installEventFilter(this);
}

void SpinBoxEventFilter::setScrolling() {
    scrolling = true;
    scrollingTimer.start();
}

void SpinBoxEventFilter::unsetScrolling() {
#ifdef VERBOSE_DEBUGGING_OUTPUT
    std::cout << "Scrolling timeout." << std::endl;
#endif
    scrolling = false;
}
