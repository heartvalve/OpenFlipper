//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================

#include "helpAssistant.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

#include <QMessageBox>
#include <QProcess>
#include <QLibraryInfo>
#include <QString>

#include <iostream>

HelpAssistant::HelpAssistant() : filename_(0), proc_(0) {

}

HelpAssistant::~HelpAssistant() {

	 if (proc_ && proc_->state() == QProcess::Running) {
		 proc_->terminate();
		 proc_->waitForFinished(3000);
	 }
	 delete proc_;
}

bool HelpAssistant::startAssistant(const std::string& _filename) {

	filename_ = new QString(_filename.c_str());

	if (!proc_)
		 proc_ = new QProcess();

	if (proc_->state() != QProcess::Running) {

		QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();

#if !defined(Q_OS_MAC)
		app += QLatin1String("assistant");
#else
		app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
#endif

		QStringList args;
		args << QLatin1String("-file")
		 << OpenFlipper::Options::applicationDirStr()
		 + QLatin1String("/Help/") + *filename_
		 << QLatin1String("-server");

		std::cerr << app.toStdString() << " " << args[0].toStdString() << " " << args[1].toStdString() << std::endl;

		proc_->start(app, args);

		if (!proc_->waitForStarted()) {
			QMessageBox::critical(0, QObject::tr("OpenFlipper Help"),
			 QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
			return false;
		}

		// Load page index.html:
		QByteArray ba("SetSource ");
		ba.append("qthelp://com.trolltech.qmake/doc/index.html");

		proc_->write(ba + '\0');

	}
	return true;

}
