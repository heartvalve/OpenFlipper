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



#include "MemInfo.hh"
#include <ACG/GL/gl.hh>
#include <QtGlobal>

// Main Memory information
#ifdef WIN32
#elif defined ARCH_DARWIN
#else
#include <sys/sysinfo.h>
//Info class returned by sysinfo
//struct sysinfo {
//  long uptime;              /* Sekunden seit dem letzten Systemstart */
//  unsigned long loads[3];   /* 1, 5 und 15 minütige Systemlast */
//  unsigned long totalram;   /* nutzbare Hauptspeichergröße */
//  unsigned long freeram;    /* verfügbare Speichergröße */
//  unsigned long sharedram;  /* Größe des gemeinsamen Speichers */
//  unsigned long bufferram;  /* von Puffern benutzter Speicher */
//  unsigned long totalswap;  /* Größe des Auslagerungsspeichers */
//  unsigned long freeswap;   /* verfügbarer Auslagerungsspeicher */
//  unsigned short procs;     /* Aktuelle Prozesszahl */
//  unsigned long totalhigh;  /* Gesamtgröße des oberen Speicherbereichs */
//  unsigned long freehigh;   /* verfügbarer oberer Speicherbereich */
//  unsigned int mem_unit;    /* Größe der Speichereinheit in Byte */
//  char _f[20-2*sizeof(long)-sizeof(int)]; /* Auffüllung auf 64 bytes */
//};
#endif

#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX   0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049

void MemInfoPlugin::initializePlugin()
{

}


void MemInfoPlugin::pluginsInitialized() {

  // Check extension for NVIDIA memory information
  QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));

  updateTimer_ = new QTimer();
  updateTimer_->setSingleShot(false);

  // NVIDIA cards
  if ( glExtensions.contains("GL_NVX_gpu_memory_info") ) {

//    emit log(LOGINFO,"NVIDIA card Memory info supported, installing gpu memory monitor into status bar");

    gpuMemBar_= new QProgressBar();
    gpuMemBar_->setFixedWidth(260);
    gpuMemBar_->setFormat( "GPU Mem %p% %v/%m MB" );

    emit addWidgetToStatusbar(gpuMemBar_);

    connect(updateTimer_,SIGNAL(timeout()),this,SLOT(nvidiaMemoryInfoUpdate()));

    updateTimer_->start(4000);

    nvidiaMemoryInfoUpdate();
  }

  // Main Memory information
  #ifdef ARCH_DARWIN //Apple

  #else // Linux and Windows

 //   emit log(LOGINFO,"Main Memory monitoring supported, installing main memory monitor into status bar");

    mainMemBar_= new QProgressBar();
    mainMemBar_->setFixedWidth(260);
    mainMemBar_->setFormat( "Mem %p% %v/%m MB" );

    emit addWidgetToStatusbar(mainMemBar_);

    connect(updateTimer_,SIGNAL(timeout()),this,SLOT(cpuMemoryInfoUpdate()));

    updateTimer_->start(4000);

    cpuMemoryInfoUpdate();

  #endif

}

MemInfoPlugin::MemInfoPlugin():
    gpuMemBar_(NULL),
    mainMemBar_(NULL),
    updateTimer_(NULL)
{

}

MemInfoPlugin::~MemInfoPlugin() {
  delete updateTimer_;
}

void MemInfoPlugin::nvidiaMemoryInfoUpdate() {

  if (gpuMemBar_) {
    // get total memory on gpu
    GLint total_mem_kb = 0;
    glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_mem_kb);

    // get currently available memory on gpu
    GLint cur_avail_mem_kb = 0;
    glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur_avail_mem_kb);

    gpuMemBar_->setRange(  0 , total_mem_kb/1024 );
    gpuMemBar_->setValue( (total_mem_kb-cur_avail_mem_kb)/1024);
    setProgressBarStyleSheet(gpuMemBar_);
  }
}

void MemInfoPlugin::cpuMemoryInfoUpdate() {

  if (mainMemBar_) {

    unsigned long totalRamMB   = 0;
    unsigned long freeRamMB    = 0;
    unsigned long bufferRamMB  = 0;

    // Main Memory information
    #ifdef WIN32 //Windows
      // Define memory structure
      MEMORYSTATUSEX ms;
      // Set the size ( required according to spec ... why???? )
      ms.dwLength = sizeof (ms);
      // Get the info
      GlobalMemoryStatusEx(&ms);

	  totalRamMB = ms.ullTotalPhys/1024/1024;
      freeRamMB  = ms.ullAvailPhys/1024/1024;

    #elif defined ARCH_DARWIN // Apple

    #else // Linux

      struct sysinfo sys_info;
      sysinfo(&sys_info);



      // Unit in bytes ; /1024 -> KB ; /1024 MB
      totalRamMB  = sys_info.totalram  / 1024 / 1024 * sys_info.mem_unit;
      freeRamMB   = sys_info.freeram   / 1024 / 1024 * sys_info.mem_unit;
      bufferRamMB = sys_info.bufferram / 1024 / 1024 * sys_info.mem_unit; // Buffers get freed, if we don't have enough free ram
    #endif

    mainMemBar_->setRange(  0 , totalRamMB  );
    mainMemBar_->setValue( totalRamMB-freeRamMB-bufferRamMB);
    setProgressBarStyleSheet(mainMemBar_);
  }
}


void MemInfoPlugin::setProgressBarStyleSheet(QProgressBar* _bar)
{
  const int val = _bar->value();
  const int maxVal = _bar->maximum();
  
  // red starts with 50% mem alloc with 0 and is 1 at 100% mem alloc
  float redPerc = (val > 0.5f*maxVal) ? 2.f*(val-0.5f*maxVal) : 0.f;
  const quint32 red = 255.f*redPerc/maxVal;
  const quint32 green = 255u-red;
  const quint32 blue = 0u;

  //save color in a 32bit integer
  const quint32 color = (red<<16)+(green<<8)+blue;
  _bar->setStyleSheet(QString(" QProgressBar { border: 2px solid grey; border-radius: 2px; text-align: center; } QProgressBar::chunk {background-color: #%1; width: 1px;}").arg(color,0,16));
  
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( meminfoplugin , MemInfoPlugin );
#endif

