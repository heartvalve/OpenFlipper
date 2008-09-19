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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




// Mainwindow

#include "OpenFlipper/Core/Core.hh"

// Qt
#include <qapplication.h>
#include <qgl.h>

// stdc++
#include <iostream>
#include <fstream>
#include <csignal>
#include <cstdlib>

#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenFlipper/SimpleOpt/SimpleOpt.h>

#ifndef WIN32
#include <execinfo.h>
#endif

#ifdef USE_OPENMP
#include <omp.h>
#endif

// #ifndef WIN32
//
// #include <sys/resource.h>
//
// void getSystemInfo() {
//
//   struct rusage resource_usage;
//   if (getrusage(RUSAGE_SELF, &resource_usage))
//   {
//     //error - call to getrusage failed - handle it
//     std::cerr << "Unable to get process information" << std::endl;
//   }
//   else
//   {
//     std::cerr << "Got process information" << std::endl;
//     std::cerr << "Maximum resident size   : " << resource_usage.ru_maxrss << std::endl;
//     std::cerr << "Shared Memory size      : " << resource_usage.ru_ixrss << std::endl;
//     std::cerr << "Unshared Data size      : " << resource_usage.ru_idrss << std::endl;
//     std::cerr << "Unshared Stack size     : " << resource_usage.ru_isrss << std::endl;
//     std::cerr << "Page faults             : " << resource_usage.ru_majflt << std::endl;
//     std::cerr << "Block input operations  : " << resource_usage.ru_inblock << std::endl;
//     std::cerr << "Block output operations : " << resource_usage.ru_oublock << std::endl;
//
//
//     //call successful
//     //get values for resident set size (ru_idrss)
//     //swap space is probably not accounted for but thats all what this
//     //call gives you...

//     struct rusage {
//         struct timeval ru_utime; /* user time used */
//         struct timeval ru_stime; /* system time used */
//         long   ru_minflt;        /* page reclaims */
//         long   ru_nswap;         /* swaps */
//         long   ru_msgsnd;        /* messages sent */
//         long   ru_msgrcv;        /* messages received */
//         long   ru_nsignals;      /* signals received */
//         long   ru_nvcsw;         /* voluntary context switches */
//         long   ru_nivcsw;        /* involuntary context switches */
//     };


//   }

// }

// #endif



enum {OPT_HELP , OPT_STEREO, OPT_BATCH ,OPT_CONSOLE_LOG , OPT_DEBUGGING, OPT_FULLSCREEN,
      OPT_HIDDDEN_LOGGER , OPT_NOSPLASH ,OPT_HIDDDEN_TOOLBOX , OPT_LOAD_POLYMESHES,
      OPT_REMOTE};

CSimpleOpt::SOption g_rgOptions[] = {
    { OPT_DEBUGGING        , "--debug"          , SO_NONE    },
    { OPT_HELP             , "-?"               , SO_NONE    },
    { OPT_HELP             , "--help"           , SO_NONE    },
    { OPT_HELP             , "-h"               , SO_NONE    },
    { OPT_STEREO           , "--enable-stereo"  , SO_NONE    },
    { OPT_BATCH            , "-b"               , SO_NONE    },
    { OPT_CONSOLE_LOG      , "-c"               , SO_NONE    },
    { OPT_CONSOLE_LOG      , "--log-to-console" , SO_NONE    },
    { OPT_FULLSCREEN       , "-f"               , SO_NONE    },
    { OPT_HIDDDEN_LOGGER   , "-l"               , SO_NONE    },
    { OPT_NOSPLASH         , "--no-splash"      , SO_NONE    },
    { OPT_HIDDDEN_TOOLBOX  , "-t"               , SO_NONE    },
    { OPT_LOAD_POLYMESHES  , "-p"               , SO_NONE    },
    { OPT_REMOTE           , "--remote-control" , SO_NONE    },
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

  std::cerr << " --enable-stereo \t: Enable Stereo Mode" << std::endl;
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


#ifndef WIN32
void backtrace()
{
  void *addresses[10];
  char **strings;

  int size = backtrace(addresses, 10);
  strings = backtrace_symbols(addresses, size);
  std::cerr << "Stack frames: " << size << std::endl;
  for(int i = 0; i < size; i++)
    std::cerr << i << ": " << strings[i] << std::endl;
  free(strings);
}
#endif

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
#ifndef WIN32
  std::cerr << "\33[0m"  << "Trying a backtrace to show what happened last: " << std::endl;
  backtrace();

  std::cerr << "\n" << std::endl;
  std::cerr << "Backtrace completed, trying to abort now ..." << std::endl;

#endif

  std::abort();
}

int main(int argc, char **argv)
{
  OpenFlipper::Options::argc(&argc);
  OpenFlipper::Options::argv(&argv);

// Only Install signal handler if not running in debug version, otherwise gdb will get confused
#ifndef DEBUG
  // Set a handler for segfaults
  std::signal(SIGSEGV, segfaultHandling);
#endif

  OpenFlipper::Options::windowTitle("OpenFlipper v" + OpenFlipper::Options::coreVersion());

  //======================================================
  // Parse command line Options
  //======================================================
  bool openPolyMeshes = false;

  bool remoteControl  = false;

  CSimpleOpt args(argc, argv, g_rgOptions);

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
            OpenFlipper::Options::stereo(true);
            break;
        case OPT_HIDDDEN_TOOLBOX:
            OpenFlipper::Options::hideToolbox(true);
            break;
        case OPT_HIDDDEN_LOGGER:
            OpenFlipper::Options::hideLogger(true);
            break;
        case OPT_FULLSCREEN:
            OpenFlipper::Options::fullScreen(true);
            break;
        case OPT_LOAD_POLYMESHES:
            openPolyMeshes = true;
            break;
        case OPT_NOSPLASH:
            OpenFlipper::Options::splash(false);
            break;
        case OPT_REMOTE:
            remoteControl = true;
            break;
        case OPT_HELP:
          showHelp();
          return 0;
      }
    } else {
      std::cerr << "Invalid argument: " << args.OptionText() << std::endl;
      showHelp();
      return 1;
    }
  }

  if ( !OpenFlipper::Options::nogui() ) {

    // OpenGL check
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication app(argc,argv);

    if ( !QGLFormat::hasOpenGL() ) {
      std::cerr << "This system has no OpenGL support.\n";
      return -1;
    }

    glutInit(&argc,argv);

    // create core ( this also reads the ini files )
    Core * w = new Core( );

    // After setting all Options from command line, build the real gui
    w->init();

    for ( int i = 0 ; i < args.FileCount(); ++i )
      w->commandLineOpen(args.File(i), openPolyMeshes);

    return app.exec();

  } else {

    QCoreApplication app(argc,argv);

    // create widget ( this also reads the ini files )
    Core * w = new Core( );

    // After setting all Options from command line, build the real gui
    w->init();

    for ( int i = 0 ; i < args.FileCount(); ++i )
      w->commandLineScript(args.File(i));

    if ( remoteControl)
      return app.exec();
  }

  return 0;
}
