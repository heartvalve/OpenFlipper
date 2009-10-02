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
//   $Revision: 5074 $
//   $Author: wilden $
//   $Date: 2009-02-25 18:27:57 +0100 (Mi, 25. Feb 2009) $
//
//=============================================================================




#ifndef OPENFLIPPERTHREAD_HH
#define OPENFLIPPERTHREAD_HH

#include <QThread>
#include <QProgressDialog>

#include <OpenFlipper/common/GlobalDefines.hh>

class DLLEXPORT OpenFlipperThread : public QThread
{
  Q_OBJECT
  
  public:
    OpenFlipperThread( QString _jobId );
    ~OpenFlipperThread();
    
    /** \brief Main processing
    *
    * Connect a function to the function() signal ( DirectConnection!!! )
    * Or reimplement this function
    */
    virtual void run();

    /** Reimplement this if you have to take care about how your process is canceled
    */
    virtual void cancel();
    
  private:
    QString jobId_;
    
  public slots:
    /** Call this slot with the correct job id to abort processing
    * This directly calls cancel()
    * This is only usefull when you derived from this class, as other jobs might not react on this slot
    */
    void slotCancel( QString _jobId);
    
  signals:
    /// Emit this signal to tell the core about your job status
    void state( QString _jobId, int _state );
    
    /// This signal is emitted if your job has finished
    void finished( QString _jobId );
    
    /** The thread directly calls this function. You do not have to derive from OpenFlipperThread ( although this is
    * necessary to update the core about your progress via state().
    * You can also connect one of your slots using \n
    * connect( OpenFlipperThread ,SIGNAL(function()),YourPlugin,SLOT(YourSlot(),Qt::DirectConnection) );\n
    * The default implementation will call your slot inside the given thread and the core will still respond.
    * However you should only use this function if you Dont know how long your job takes. \n
    * Otherwise you should reimplement the run() function of OpenFlipperThread.
    */
    void function();

    
};



#endif //OPENFLIPPERTHREAD_HH
