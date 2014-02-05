/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/


/*
 * helpBrowser.hh
 *
 *  Created on: Apr 8, 2009
 *      Author: kremer
 */

#ifndef HELPBROWSER_HH_
#define HELPBROWSER_HH_


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QTextBrowser>
#include <QtHelp>

/** \class HelpBrowser
 *
 * This class implements OpenFlippers help browser. The QtHelp files are loaded at startup
 * into the help engine which is passed to this widget.
 *
 */
class HelpBrowser : public QTextBrowser {
	Q_OBJECT

public:

	/** \brief Constructor
	 *
	 * Sets up a new help widget which works on the given help engine
	 *
	 * @param _helpEngine  QHelpEngine
	 * @param parent       Parent widget
	 */
	HelpBrowser(QHelpEngine* _helpEngine, QWidget* parent = 0);

	/// Destructor
	virtual ~HelpBrowser();

	/** \brief re implementation of the load resource function of the text browser
	 *
	 * This function loads a resource from the help system. It also handles jumping to
	 * another namespace via relative links in the url
	 *
	 * @param _type Ignored
	 * @param _name A filename in the help system that should be loaded (image, html document)
	 * @return The loaded resource
	 */
	QVariant loadResource ( int _type, const QUrl& _name );

	/** \brief Checks if we visited other pages before
	 *
   * @return Did we visit pages before to which we can jump back? (History)
   */
	bool isBackwardAvailable();

	/** \brief Checks if the back button was pressed and we can go forward to the next page
	 *
	 * @return is a page stored in the forward list?
	 */
	bool isForwardAvailable();

	/** \brief resolves relative urls to absolute
	 *
	 * @param _url relative URL which will be resolved
	 * @return absolute url
	 */
	QUrl resolveUrl(const QUrl &_url);

signals:
/*
 * is emitted, when the page history was changed.
 * it is not equal to the signal 'sourceChanged'
 * because saving in the history can be skipped
 */
   void historyChanged(const QUrl&);

public slots:

	void open(const QString& _url);

	/** \brief Opens a new url from the help system in the browser
	 *
	 * @param _url      URL that should be opened
	 * @param _skipSave Flag if the URL should be saved in the history
	 */
	void open(const QUrl& _url, bool _skipSave = false);

	/** \brief Show last page stored in the history
	 *
	 */
	void backward();

	/** \brief Show next page stored in the history
	   *
	   */
	void forward();

private:

	/** \brief Extract path from URL
	 *
	 * Removes the filename part from the url and returns only the directory component
	 *
	 * @param _url Input url
	 * @return Path
	 */
	QUrl getCurrentDir(const QUrl& _url);

	/** \brief updateNameSpaceAndFolder
	 *
	 * This function takes the url and extracts the virtual namespace and
	 * folder information which are stored in the variables. These variables are used to
	 * find the corresponding images.
	 *
	 * @param _url The url that should be analyzed
	 */
	void updateNameSpaceAndFolder (const QUrl& _url);

private slots:
	/** \brief Adds a new page to the history
	 *
	 * @param _url URL of the new page
	 */
	void rememberHistory (const QUrl& _url);
private:

	/// The help engine the widget is working on
	QHelpEngine* helpEngine_;

	/// History of the visited pages
	QList<QUrl> visitedPages_;

	/// Current position in the history
	int currentPage_;

	/** \brief The currently active virtual folder
	 *
	 * QT manages the different help files for the plugins in different virtual folders.
	 * This variable will always hold the currently active one.
	 */
	QString currentVirtualFolder_;

	/** \brief The currently active namespace
	 *
	 * QT manages the different help files for the plugins with different name spaces.
	 * This variable will always hold the currently active one.
	 */
	QString currentNameSpace_;

};


#endif /* HELPBROWSER_HH_ */
