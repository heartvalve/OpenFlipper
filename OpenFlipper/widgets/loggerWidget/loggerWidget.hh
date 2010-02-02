/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




#ifndef LOGGERWIDGET_HH
#define LOGGERWIDGET_HH

#include <QtGui>

#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

/** \brief Implementation of the logger Widget
 *
 * This class adds some special features to the textedit for the log window
 */
class LoggerWidget : public QWidget
{

  Q_OBJECT

  public:
    LoggerWidget( QWidget *parent = 0 );

    /** \brief Append a new logmessage to log viewer
    *
    * Display log message of given logtype
    */
    void append(QString _text, Logtype _type);
    
  protected:

    /** \brief Grab key events
    *
    * This function grabs all key events and passes them back to the core to handle them correctly
    */
    void keyPressEvent (QKeyEvent * _event );
    
    /** \brief Show context menu
    *
    * This function shows the context menu
    */
    void contextMenuEvent ( QContextMenuEvent * event );
    
    /** \brief Called when the widget is shown
    *
    * Function scrolls to bottom if the object gets visible
    */
    void showEvent ( QShowEvent * event );
  
  private:
    
    QListWidget* list_;
    
    //scrolling with list_'s scrollbar doesn't work correctly
    //use separate scrollbar as workaround
    QScrollBar* scrollBar_;
    bool blockNext_;
    
    QPushButton* allButton_;
    QPushButton* infoButton_;
    QPushButton* warnButton_;
    QPushButton* errorButton_;
    
    QPushButton* clearButton_;
    
    QMenu* context_;
    
  private slots:
    /// update the list if a button was pressed
    void updateList();
    
    /// workaround for scrolling
    void scrollTo(int _pos);
    /// map scroll position of list to scrollbar position
    void mapScrollPosition(int _pos);
    ///copy Selected rows to clipboard
    void copySelected();
};

#endif //LOGGERWIDGET_HH
