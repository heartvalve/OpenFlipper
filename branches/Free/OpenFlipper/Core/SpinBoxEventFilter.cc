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
