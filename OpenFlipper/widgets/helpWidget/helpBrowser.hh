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
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/


/*
 * helpBrowser.hh
 *
 *  Created on: Apr 8, 2009
 *      Author: kremer
 */

#ifndef HELPBROWSER_HH_
#define HELPBROWSER_HH_

#define VIRTUAL_FOLDER "doc"

#include <QtGui>
#include <QTextBrowser>
#include <QtHelp>

class HelpBrowser : public QTextBrowser {
	Q_OBJECT

public:
	HelpBrowser(QHelpEngine* _helpEngine, QWidget* parent = 0);

	virtual ~HelpBrowser();

	QVariant loadResource ( int _type, const QUrl& _name );

	bool isBackwardAvailable();

	bool isForwardAvailable();

signals:
	void urlChanged ( const QUrl& src );

	void linkClicked(const QString& _link);

public slots:

	void open(const QUrl& _url);

	void open(const QString& _url);

	void open(const QUrl& _url, const QString& _str, bool _skipSave = false);

	void backward();

	void forward();

private:

	QUrl getCurrentDir(const QUrl& _url);

	QHelpEngine* helpEngine_;

	QStringList visitedPages_;
	int currentPage_;

};


#endif /* HELPBROWSER_HH_ */
