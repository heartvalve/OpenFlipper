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
