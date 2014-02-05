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


#ifndef KEYGENWINDOW_HH
#define KEYGENWINDOW_HH

#include <ui_keygen.hh>

class KeyGen
{
	public:
		QString name;
		QString coreHash;
		QString pluginHash;
		QString cpuHash;
		QString productHash;
		QStringList macHashes;
		QString requestSig;
	public:
		KeyGen(QString n, QString cHash, QString pHash, QString cpHash, QString prHash, QStringList mHashes, QString request);
		//returns string containing the key
		QString Generate(QString expiryDate) const;
		//finds all occurrences of info in messy string
		QString computeSignature() const;
		bool isValid() const;

		void copyHardwareHashesFrom(const KeyGen &rhs) {
		    cpuHash = rhs.cpuHash;
		    macHashes = rhs.macHashes;
		}

		QString generateRequest() {
		    return QString("%1\n%2\n%3\n%4\n%5\n%6\n%7\n\n")
		            .arg(name)
		            .arg(coreHash)
		            .arg(pluginHash)
		            .arg(cpuHash)
		            .arg(productHash)
		            .arg(macHashes.join("\n"))
		            .arg(computeSignature());
		}

		static std::vector<KeyGen> CreateFromMessyString(QString info);

		static QString filterString(QString in);
};


class KeyGenWidget : public QMainWindow, public Ui::keyWindow
{
  Q_OBJECT

public:
  KeyGenWidget(QMainWindow *parent);
  virtual ~KeyGenWidget();

public slots:
  void slotGenerateAllButton();
  void slotGenerateButton();
  
  // slot taking license duration and convert to expiration date
  void slotDate();
  
  // Analyzes the current request
  void slotAnalyze();
  
  // Split Code based on ;; for broken windows requests
  void slotSplit();
  
  void handleSelectionChanged(const QItemSelection& selection);

  void slotMangle();

private:

  void toFile(const KeyGen* gen);
  void setKeyGen(const KeyGen* gen);

  std::vector<KeyGen> keygens_;

};

 #endif // KEYGENWINDOW_HH
