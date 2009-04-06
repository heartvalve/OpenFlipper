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

#ifndef HELPASSISTANT_HH_
#define HELPASSISTANT_HH_

class QProcess;
class QString;

#include <string>

class HelpAssistant {

public:

	/// Constructor
	HelpAssistant();

	/// Desctructor
	virtual ~HelpAssistant();

	/// Start and show help browser
	bool startAssistant(const std::string& _filename);

private:
	QString* filename_;

	QProcess* proc_;
};

#endif /* HELPASSISTANT_HH_ */
