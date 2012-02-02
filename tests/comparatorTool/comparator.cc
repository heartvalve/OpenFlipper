// C++ Hello World

#include <QFile> 
#include <QString> 
#include <QStringList> 
#include <QSettings>

#include <iostream>
 



void compareDouble(QVariant _result, QVariant _reference) {
  std::cerr << "comparing Double" << std::endl;

  double resultStr = _result.toDouble();
  double resultRef = _reference.toDouble();

  
}

void compareString(QVariant _result, QVariant _reference) {
  std::cerr << "comparing String" << std::endl;

  QString resultStr = _result.toString().simplified();
  QString resultRef = _reference.toString().simplified();
  
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
       compareString( resultFile.value(toplevelKeys[i]), referenceFile.value(toplevelKeys[i]));
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
