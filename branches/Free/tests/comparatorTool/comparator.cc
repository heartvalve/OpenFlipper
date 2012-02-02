// C++ Hello World

#include <QFile> 
#include <QString> 
#include <QStringList> 
#include <QSettings>

#include <iostream>
#include <math.h>



bool compareDouble(QString _key , QVariant _result, QVariant _reference) {
  std::cerr << "comparing Double" << std::endl;

  // maximal allowed double tolerance
  double tolerance = 0.0;

  // Check if the reference consists of two colon separated values
  // Second value would specify maximal allowed tolerance
  QStringList referenceData = _reference.toString().split(';');
  if ( referenceData.size() == 2) {
    tolerance = referenceData[1].toDouble();
  }


  double result    = _result.toDouble();
  double reference = referenceData[0].toDouble();

  if ( fabs(result-reference) <= tolerance )  {
    std::cerr << "Compared " << result << " " << reference << " " << " " << tolerance <<  " ok" << std::endl;
    return true;
  } else {
    std::cerr << "Comparison failed for key " <<  _key.toStdString() << " :" << std::endl;
    std::cerr << "Result: " << result << " ; Expected: " << reference << std::endl;
    std::cerr << "Difference: " << fabs(result-reference) << " allowed tolerance was: " << tolerance << std::endl;
    return false;
  }
  
}

bool compareString(QString _key ,QVariant _result, QVariant _reference) {
  std::cerr << "comparing String" << std::endl;

  QString resultStr = _result.toString().simplified();
  QString resultRef = _reference.toString().simplified();
  
  if (resultStr == resultRef ) {
    std::cerr << "Comparison ok for key " <<  _key.toStdString() << " :" << resultStr.toStdString() << " ; " << resultRef.toStdString()<< std::endl;
    return true;
  } else {
    std::cerr << "Comparison failed for key " <<  _key.toStdString() << " :" << std::endl;
    std::cerr << "Result: " << resultStr.toStdString() << " ; Expected: " << resultRef.toStdString() << std::endl;
    return false;
  }

}

int main(int argv, char **args)
{

  // Flag if everything went fine
  bool ok = true;

  // Maximal allowed deviation between two doubles
  double DoubleTolerance = 0.0;

  // Return if we did not get exactly two arguments
  if ( argv != 3 ) {
    std::cerr << "Wrong number of arguments!" << std::endl;
    std::cerr << "Usage:" << std::endl;
    std::cerr << "compareTool ResultFile ReferenceFile" << std::endl;
    return(1);
  }

  QString file1(args[1]);
  QString file2(args[2]);

  QSettings resultFile(file1,QSettings::IniFormat);
  QSettings referenceFile(file2,QSettings::IniFormat);
  
  QStringList toplevelKeys = referenceFile.childKeys();
  QStringList groups       = referenceFile.childGroups();
   
  if ( groups.size() == 0 ) {
   std::cerr << "One level only" << std::endl;
   for ( int i = 0 ; i < toplevelKeys.size(); ++i) {
     std::cerr << "Key " << i << " : " << toplevelKeys[i].toStdString() << std::endl; 
     if ( resultFile.contains(toplevelKeys[i]) ) {
       if ( toplevelKeys[i].endsWith("_DOUBLE") ) {
         ok &= compareDouble(toplevelKeys[i],resultFile.value(toplevelKeys[i]), referenceFile.value(toplevelKeys[i]));
       } else
         compareString( toplevelKeys[i],resultFile.value(toplevelKeys[i]), referenceFile.value(toplevelKeys[i]));
     } else {
       std::cerr << "Missing key in result file: " << toplevelKeys[i].toStdString() << std::endl;
       ok = false;
     }
 
   }   
 

  } else {
    std::cerr << "Multiple levels!" << std::endl;
  }
  

  

 
  std::cerr << "Got filename " << file1.toStdString() << std::endl;
  std::cerr << "Got filename " << file2.toStdString() << std::endl;

  return(0);

}
