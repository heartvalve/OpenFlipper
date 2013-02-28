/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

#include <QString> 
#include <QStringList> 
#include <QSettings>
#include <QFileInfo>
#include <QTextStream>

#include <math.h>
#include <limits>
#include <iostream>


/* Test File specification:
Doubles can be given as:

Exact test:
VALUE_DOUBLE=1.0

Tolerance test (0.0001)
VALUE_DOUBLE=1.0,0.0001


*/


bool compareDouble(QString _key , QVariant _result, QVariant _reference) {

  // maximal allowed double tolerance
  double tolerance = std::numeric_limits<int>::epsilon();

  // Check if the reference consists of two colon separated values
  // Second value would specify maximal allowed tolerance
  QStringList referenceData = _reference.toString().split(';');
  if ( referenceData.size() == 2) {
    tolerance = referenceData[1].toDouble();
  }


  double result    = _result.toDouble();
  double reference = referenceData[0].toDouble();

  if ( fabs(result-reference) <= tolerance )  {
    return true;
  } else {

    //qcerr.setRealNumberPrecision(40);
	std::cerr.precision(40);
    std::cerr << "===================================================================\n";
	std::cerr << "Comparison failed for key " <<  _key.toStdString() << " :\n";
    std::cerr << "Result:                " << result << "\n";
    std::cerr << "Expected:              " << reference << "\n";
    std::cerr << "Difference:            " << fabs(result-reference) << "\n";
    std::cerr << "Allowed tolerance was: " << tolerance << "\n";
    std::cerr << "===================================================================\n";
    return false;
  }
  
}

bool compareString(QString _key ,QVariant _result, QVariant _reference) {

  QString resultStr = _result.toString().simplified();
  QString resultRef = _reference.toString().simplified();
  
  if (resultStr == resultRef ) {
    return true;
  } else {
	std::cerr << "Comparison failed for key " <<  _key.toStdString() << " :\n";
    std::cerr << "Result: " << resultStr.toStdString() << " ; Expected: " << resultRef.toStdString() << "\n";
    return false;
  }

}

int main(int argv, char **args)
{
  std::cout << "============================================================\n" ;
  std::cout << "Executing compare tool\n";
  std::cout << "Comparing results to reference:\n" ;

  // Flag if everything went fine
  bool ok = true;

  // Return if we did not get exactly two arguments
  if ( argv != 3 ) {
    std::cerr << "Wrong number of arguments!\n";
    std::cerr << "Usage:\n";
    std::cerr << "compareTool ResultFile ReferenceFile\n";
    return(1);
  }

  QString file1(args[1]);
  QString file2(args[2]);

  QFileInfo resultFileInfo(file1);
  if ( !resultFileInfo.exists() ) {
    std::cerr << "ERROR! Result file: " << file1.toStdString() << " does not exist!\n";

    return 1;
  }

  QFileInfo referenceFileInfo(file2);
  if ( !referenceFileInfo.exists() ) {
    std::cerr << "ERROR! Reference file: " << file2.toStdString() << " does not exist!\n";
    return 1;
  }

  QSettings resultFile(file1,QSettings::IniFormat);
  QSettings referenceFile(file2,QSettings::IniFormat);
  
  if ( resultFile.status() != QSettings::NoError) {
    std::cerr << "QSettings error when opening result file: " << file1.toStdString() << "\n";
    return 1;
  }

  if ( referenceFile.status() != QSettings::NoError) {
    std::cerr << "QSettings error when opening result reference file: " << file2.toStdString() << "\n";
    return 1;
  }

  QStringList toplevelKeys = referenceFile.childKeys();
  QStringList groups       = referenceFile.childGroups();
   
  if ( groups.size() == 0 ) {
   for ( int i = 0 ; i < toplevelKeys.size(); ++i) {
     if ( resultFile.contains(toplevelKeys[i]) ) {
       if ( toplevelKeys[i].endsWith("_DOUBLE") ) {
         ok &= compareDouble(toplevelKeys[i],resultFile.value(toplevelKeys[i]), referenceFile.value(toplevelKeys[i]));
       } else
         ok &= compareString( toplevelKeys[i],resultFile.value(toplevelKeys[i]), referenceFile.value(toplevelKeys[i]));
     } else {
       std::cerr << "Missing key in result file: " << toplevelKeys[i].toStdString() << "\n";
       ok = false;
     }
 
   }   
 

  } else {
    std::cerr << "Multiple levels!" << "\n";
    return 1;
  }
  

  if ( ! ok ) {
    std::cerr << "At least one of the tests failed!\n";
    return 1;
  }

  return(0);
}
