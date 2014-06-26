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



// Mainwindow

#include "OpenFlipper/Core/Core.hh"
#include "common/glew_wrappers.hh"

// Qt
#include <QApplication>
#include <qgl.h>

// stdc++
#include <iostream>
#include <fstream>
#include <csignal>
#include <cstdlib>

#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenFlipper/SimpleOpt/SimpleOpt.h>

#if ( defined(WIN32))
  #define NO_EXECINFO
#endif

#ifndef NO_EXECINFO
#include <execinfo.h>
#endif

#ifdef WIN32
#ifndef NDEBUG
#define WIN_GET_DEBUG_CONSOLE
#endif
#endif

#ifdef USE_OPENMP
#include <omp.h>
#endif

/* ==========================================================
 *
 * Stackwalker code. Used to get a backtrace if OpenFlipper
 * crashes under windows
 *
 * ==========================================================*/


#ifdef WIN32
  #include "StackWalker/StackWalker.hh"
  #include <fstream>

  class StackWalkerToConsole : public StackWalker
  {
  protected:
	  virtual void OnOutput(LPCSTR szText)
    	{
			// Writes crash dump to .OpenFlipper config directory
			std::ofstream crashFile;
			QString crashName = OpenFlipper::Options::configDirStr() + QDir::separator() + "CrashDump.txt";
			crashFile.open(crashName.toLatin1(),std::ios::out | std::ios::app);
			crashFile << szText;
			crashFile.close();
	  }
  };
#endif


/* ==========================================================
 *
 * Console for Windows to get additional output written via
 * cerr, cout, ... that is not forwarded to log window
 *
 * ==========================================================*/

// Includes for windows debugging console
#ifdef WIN32
#ifdef WIN_GET_DEBUG_CONSOLE
  #include <fcntl.h>
  #include <io.h>
#endif
#endif

#ifdef WIN32
#ifdef WIN_GET_DEBUG_CONSOLE
    void getConsole() {
      //Create a console for this application
      AllocConsole();
      //Redirect unbuffered STDOUT to the console
      HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
      int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
      FILE *COutputHandle = _fdopen(SystemOutput, "w" );
      *stdout = *COutputHandle;
      setvbuf(stdout, NULL, _IONBF, 0);
      //Redirect unbuffered STDERR to the console
      HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
      int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
      FILE *CErrorHandle = _fdopen(SystemError, "w" );
      *stderr = *CErrorHandle;
      setvbuf(stderr, NULL, _IONBF, 0);
      //Redirect unbuffered STDIN to the console
      HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
      int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
      FILE *CInputHandle = _fdopen(SystemInput, "r" );
      *stdin = *CInputHandle;
      setvbuf(stdin, NULL, _IONBF, 0);
    }
#endif
#endif

/* ==========================================================
 *
 * Linux function printing a full stack trace to the console
 *
 * ==========================================================*/
#ifndef NO_EXECINFO
void backtrace()
{
  void *addresses[20];
  char **strings;

  int size = backtrace(addresses, 20);
  strings = backtrace_symbols(addresses, size);
  std::cerr << "Stack frames: " << size << std::endl;
  for(int i = 0; i < size; i++)
    std::cerr << i << ": " << strings[i] << std::endl;
  free(strings);

}
#endif

/* ==========================================================
 *
 * General segfault handler. This function is called if OpenFlipper
 * crashes
 *
 * ==========================================================*/
void segfaultHandling (int) {

  // prevent infinite recursion if segfaultHandling() causes another segfault
  std::signal(SIGSEGV, SIG_DFL);


  std::cerr << "\n" << std::endl;
  std::cerr << "\n" << std::endl;
  std::cerr << "\33[31m" << "=====================================================" << std::endl;
  std::cerr << "\33[31m" << "OpenFlipper or one of its plugins caused a Segfault." << std::endl;
  std::cerr << "\33[31m" << "This should not happen,... Sorry :-(" << std::endl;
  std::cerr << "\33[31m" << "=====================================================" << std::endl;
  std::cerr << "\n" << std::endl;

  // Linux Handler
#ifndef NO_EXECINFO
  std::cerr << "\33[0m"  << "Trying a backtrace to show what happened last: " << std::endl;
  backtrace();

  std::cerr << "\n" << std::endl;
  std::cerr << "Backtrace completed, trying to abort now ..." << std::endl;
#endif

  // Windows handler via StackWalker
#ifdef WIN32
  StackWalkerToConsole sw;
  sw.ShowCallstack();
#endif


  std::cerr << "Trying to get additional information (This might fail if the memory is corrupted)." << std::endl;

  if (OpenFlipper::Options::gui()) {
    for ( unsigned int i = 0 ; i <  4 ; ++i) {
      std::cerr << "DrawMode Viewer "<<  i << " " << PluginFunctions::drawMode(i).description() << std::endl;
    }
  }

  std::abort();
}

enum {OPT_HELP , OPT_STEREO, OPT_BATCH ,OPT_CONSOLE_LOG , OPT_DEBUGGING, OPT_FULLSCREEN,
      OPT_HIDDDEN_LOGGER , OPT_NOSPLASH ,OPT_HIDDDEN_TOOLBOX , OPT_LOAD_POLYMESHES,
      OPT_REMOTE, OPT_REMOTE_PORT};

CSimpleOpt::SOption g_rgOptions[] = {
    { OPT_DEBUGGING        , (char*) "--debug"          , SO_NONE    },
    { OPT_HELP             , (char*) "-?"               , SO_NONE    },
    { OPT_HELP             , (char*) "--help"           , SO_NONE    },
    { OPT_HELP             , (char*) "-h"               , SO_NONE    },
    { OPT_STEREO           , (char*) "--disable-stereo" , SO_NONE    },
    { OPT_BATCH            , (char*) "-b"               , SO_NONE    },
    { OPT_CONSOLE_LOG      , (char*) "-c"               , SO_NONE    },
    { OPT_CONSOLE_LOG      , (char*) "--log-to-console" , SO_NONE    },
    { OPT_FULLSCREEN       , (char*) "-f"               , SO_NONE    },
    { OPT_HIDDDEN_LOGGER   , (char*) "-l"               , SO_NONE    },
    { OPT_NOSPLASH         , (char*) "--no-splash"      , SO_NONE    },
    { OPT_HIDDDEN_TOOLBOX  , (char*) "-t"               , SO_NONE    },
    { OPT_LOAD_POLYMESHES  , (char*) "-p"               , SO_NONE    },
    { OPT_REMOTE           , (char*) "--remote-control" , SO_NONE    },
    { OPT_REMOTE_PORT      , (char*) "--remote-port"    , SO_REQ_SEP },
    SO_END_OF_OPTIONS                       // END
};

void showHelp() {
  std::cerr << "OpenFlipper [Options] <filenames> " << std::endl << std::endl;;
  std::cerr << "Possible Options : " << std::endl;
  std::cerr << std::endl;

  std::cerr << "Load/Save Options:" << std::endl;
  std::cerr << " -p \t: Open files as PolyMeshes" << std::endl;
  std::cerr << std::endl;

  std::cerr << "Gui Options:" << std::endl;
  std::cerr << " -f \t\t: Start Fullscreen" << std::endl;
  std::cerr << " -l \t\t: Start with hidden logger" << std::endl;
  std::cerr << " -t \t\t: Start with hidden Toolbox" << std::endl;
  std::cerr << " --no-splash \t: Disable splash screen" << std::endl;

  std::cerr << " --disable-stereo \t: Disable Stereo Mode" << std::endl;
  std::cerr << std::endl;

  std::cerr << "Log options:" << std::endl;
  std::cerr << " --log-to-console ( -c ) \t: Write logger window contents to console" << std::endl;
  std::cerr << std::endl;

  std::cerr << "Other options:" << std::endl;
  std::cerr << " -b \t: Batch mode, you have to provide a script for execution" << std::endl;
  std::cerr << " --remote-control \t: Batch mode accepting remote connections" << std::endl;

  std::cerr << std::endl;


  std::cerr << " -h \t: This help" << std::endl;
}






bool openPolyMeshes = false;
bool remoteControl  = false;

bool parseCommandLineOptions(CSimpleOpt& args){

  QString port;

  // while there are arguments left to process
  while (args.Next()) {

    if (args.LastError() == SO_SUCCESS) {

      switch (args.OptionId() ) {
        case OPT_BATCH:
          OpenFlipper::Options::nogui(true);
        break;
        case OPT_CONSOLE_LOG:
          OpenFlipper::Options::logToConsole(true);
          break;
        case OPT_DEBUGGING:
          OpenFlipper::Options::debug(true);
          break;
        case OPT_STEREO:
          OpenFlipper::Options::stereo(false);
          break;
        case OPT_HIDDDEN_TOOLBOX:
          OpenFlipperSettings().setValue("Core/Gui/ToolBoxes/hidden",true);
          break;
        case OPT_HIDDDEN_LOGGER:
          OpenFlipper::Options::loggerState(OpenFlipper::Options::Hidden);
          break;
        case OPT_FULLSCREEN:
          OpenFlipperSettings().setValue("Core/Gui/fullscreen",false);
          break;
        case OPT_LOAD_POLYMESHES:
          openPolyMeshes = true;
          break;
        case OPT_NOSPLASH:
          OpenFlipperSettings().setValue("Core/Gui/splash",false);
          break;
        case OPT_REMOTE:
          OpenFlipper::Options::remoteControl(true);
          break;
        case OPT_REMOTE_PORT:
          port = args.OptionArg();
          std::cerr << "Got option : " << port.toStdString() << std::endl;
          OpenFlipper::Options::remoteControl(port.toInt());
          break;
        case OPT_HELP:
          showHelp();
          return 0;
      }
    } else {
      std::cerr << "Invalid argument: " << args.OptionText() << std::endl;
      showHelp();
      return false;
    }
  }
  return true;
}

int main(int argc, char **argv)
{

  // Remove -psn_0_xxxxx argument which is automatically
  // attached by MacOSX
  for (int i = 0; i < argc; i++) {
    if(strlen(argv[i]) > 4) {
      if( ( (argv[i])[0] == '-' ) &&
        ( (argv[i])[1] == 'p' ) &&
        ( (argv[i])[2] == 's' ) &&
        ( (argv[i])[3] == 'n' ) ) {
        argc--;
        argv[i] = (char *)"";
      }
    }
  }

  OpenFlipper::Options::argc(&argc);
  OpenFlipper::Options::argv(&argv);

  CSimpleOpt argBatch(argc, argv, g_rgOptions);

  //check only batchMode before the core is created
  while (argBatch.Next())
    if (argBatch.OptionId() == OPT_BATCH ){
      OpenFlipper::Options::nogui(true);
      break;
    }

  CSimpleOpt args(argc, argv, g_rgOptions);

// Only Install signal handler if not running in debug version, otherwise gdb will get confused
// #ifndef DEBUG
  // Set a handler for segfaults
  std::signal(SIGSEGV, segfaultHandling);
// #endif

  OpenFlipper::Options::windowTitle(TOSTRING(PRODUCT_STRING)" v" + OpenFlipper::Options::coreVersion());

#ifdef WIN32
#ifdef WIN_GET_DEBUG_CONSOLE
  getConsole();
#endif
#endif

  if ( !OpenFlipper::Options::nogui() ) {

    // OpenGL check
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication app(argc,argv);

#ifdef __APPLE__
    // Set organization and application names
    QCoreApplication::setOrganizationName("rwth-aachen.de");
    QCoreApplication::setApplicationName("graphics.openflipper");
#endif

    if ( !QGLFormat::hasOpenGL() ) {
      std::cerr << "This system has no OpenGL support.\n";
      return -1;
    }

#ifndef __APPLE__
    glutInit(&argc,argv);
#endif

    // create core ( this also reads the ini files )
    Core * w = new Core( );

    if ( !parseCommandLineOptions(args) ) {
      delete w;
      return 1;
    }

    QString tLang = OpenFlipperSettings().value("Core/Language/Translation","en_US").toString();

    if (tLang == "locale")
      tLang = QLocale::system().name();

    // Install translator for qt internals
    QTranslator qtTranslator;
//     std::cerr << "Loading qt translations from: " << QLibraryInfo::location(QLibraryInfo::TranslationsPath).toStdString() << std::endl;
//     if ( qtTranslator.load("qt_" + tLang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)) )
//       std::cerr << "Loaded" << std::endl;
//     std::cerr << "Loading qt translations from: " << QLibraryInfo::location(QLibraryInfo::TranslationsPath).toStdString() << std::endl;
    qtTranslator.load("qt_" + tLang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
//       std::cerr << "Loaded" << std::endl;

    app.installTranslator(&qtTranslator);
    
    // install translator for Core Application
    QString translationDir = OpenFlipper::Options::translationsDirStr() + QDir::separator();

//     std::cerr << "Loading own translations from: " << QString(translationDir + " (" + tLang + ")").toStdString() << std::endl;

    QDir dir(translationDir);
    dir.setFilter(QDir::Files);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
      QFileInfo fileInfo = list.at(i);

      if ( fileInfo.baseName().contains(tLang) ){
        QTranslator* myAppTranslator = new QTranslator();

        if ( myAppTranslator->load( fileInfo.filePath() ) ){
//           std::cerr << "Loaded " << fileInfo.fileName().toStdString() << std::endl;
    
          app.installTranslator(myAppTranslator);
        } else {
  	  delete myAppTranslator;
        }
      }
     }
    
    // After setting all Options from command line, build the real gui
    w->init();

    #ifndef __APPLE__
    initGlew();
    #endif
 
    for ( int i = 0 ; i < args.FileCount(); ++i )
      w->commandLineOpen(args.File(i), openPolyMeshes);    

    return app.exec();

  } else {

    QCoreApplication app(argc,argv);

#ifdef __APPLE__
    // Set organization and application names
    QCoreApplication::setOrganizationName("rwth-aachen.de");
    QCoreApplication::setApplicationName("graphics.openflipper");
#endif

    // create widget ( this also reads the ini files )
    Core * w = new Core( );

    if ( !parseCommandLineOptions(args) ) {
      delete w;
      return 1;
    }

    // After setting all Options from command line, build the real gui
    w->init();

    for ( int i = 0 ; i < args.FileCount(); ++i )
      w->commandLineScript(args.File(i));

    return app.exec();
  }

  return 0;
}
