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
 *   $Revision: 6727 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-08-05 08:03:50 +0200 (Mi, 05. Aug 2009) $                   *
 *                                                                           *
\*===========================================================================*/

//
// C++ Interface: ProcessInterface
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef PROCESSINTERFACE_HH
#define PROCESSINTERFACE_HH

#include <QtGui>
#include <OpenFlipper/threads/OpenFlipperThread.hh>

#ifndef WIN32
#warning Warning, process interface is under development and might change in future releases
#endif

 /** \brief Interface class controlling threadding in OpenFlipper
 *
 *
 * Example:\n
 * OpenFlipperThread* thread = new OpenFlipperThread(name() + "unique id");                     // Create your thread containing a unique id \n
 * connect(thread,SIGNAL( state(QString, int)), this,SIGNAL(setJobState(QString, int)));        // connect your threads state info to the global one \n
 * connect(thread,SIGNAL( finished(QString)), this,SIGNAL(finishJob(QString)));                 // connect your threads finish info to the global one ( you can do the same for a local one ) \n
 * connect(thread,SIGNAL( function()), this,SLOT(testFunctionThread()),Qt::DirectConnection);   // You can directly give a slot of your app that gets called \n
 * emit startJob( name() + "unique id", "Description" , 0 , 100 , false);                       // Tell the core about your thread
 * thread->start();                                                                             // start thread
 * thread->startProcessing();                                                                   // start processing
 */
class ProcessInterface {



   public :

      /// Destructor
      virtual ~ProcessInterface() {};

    signals :
      
      /** \brief announce a new job
      *
      * Use this function to announce a new job. The Job Id should be a unique id for your job.
      * Prepend your PluginName to your id to make it unique across plugins.
      *
      * The description will be the string visible to the user.
      *
      * min and max define the range your status values will be in.
      * 
      * blocking will define, if user interaction with the core should still be possible
      *
      */
      virtual void startJob( QString /*_jobId*/, QString /*_description */, int /*_min*/ , int /*_max*/ , bool /*_blocking */ = false) {};

      /** \brief update job state
      *
      * Emit this signal to tell the core about your job status.
      *
      * _value has to be in the range you defined!
      */
      virtual void setJobState(QString /*_jobId*/, int /*_value*/ ) {};
      
      /** \brief Cancel your job
      */
      virtual void cancelJob(QString /*_jobId*/ ) {};
      
      /** \brief Finish your job
      */
      virtual void finishJob(QString /*_jobId*/ ) {};
      
      private slots :
        /** \brief A job has been canceled 
        *
        * This function is called when the user cancels a job. 
        * The returned name is the name of the job which has been canceled
        */
        virtual void canceledJob (QString /*_job */) {};      
      
};

Q_DECLARE_INTERFACE(ProcessInterface,"OpenFlipper.ProcessInterface/1.0")

#endif // PROCESSINTERFACE_HH
