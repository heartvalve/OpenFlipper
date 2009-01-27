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
//   $Revision: 3512 $
//   $Author: wilden $
//   $Date: 2008-10-22 11:37:37 +0200 (Mi, 22. Okt 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS QtBaseViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"
#include "QtGLGraphicsScene.hh"
#include "QtGLGraphicsView.hh"

#include <QApplication>
#include <QStatusBar>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>

//== NAMESPACES ===============================================================

//== IMPLEMENTATION ==========================================================

void QtBaseViewer::snapshotBaseFileName(const QString& _fname)
{
  snapshotName_=_fname;
  snapshotCounter_=0;
}


//-----------------------------------------------------------------------------


void QtBaseViewer::snapshot()
{
   makeCurrent();

   glView_->raise();
   qApp->processEvents();
   makeCurrent();
   paintGL();
   glFinish();
   copyToImage(*snapshot_, 0, 0, glWidth(), glHeight(), GL_BACK);

   QFileInfo fi(snapshotName_);

   QString fname = fi.path() + QDir::separator() +fi.baseName() + "." + QString::number(snapshotCounter_) + ".";

   QString format="png";

   if (fi.completeSuffix() == "ppm")
     format="ppmraw";

   fname += format;

   bool rval=snapshot_->save(fname,format.toUpper().toLatin1());


   assert(statusbar_!=0);
   if (rval)
   {
     statusbar_->showMessage(QString("snapshot: ")+fname,5000);
//      if ( action_["SnapshotSavesView"]->isChecked() )
//      {
//        QString view; encodeView(view);
//        QFile f(fname+".view"); f.open(QIODevice::WriteOnly|QIODevice::Truncate);
//        QTextStream sf(&f); sf << view; f.close();
//      }
   }
   else
   {
     statusbar_->showMessage(QString("could not save snapshot to ")+fname);
   }

   ++snapshotCounter_;
}

//-----------------------------------------------------------------------------


void QtBaseViewer::actionSnapshot()
{
  snapshot();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::actionSnapshotName()
{
  QString fname=snapshotName_;

  fname.replace('%', '$');
  fname = QFileDialog::getSaveFileName ( 0, "Save snapshot name" );
  if (!fname.isEmpty())
  {
    fname.replace('$', '%');

    snapshotBaseFileName(fname);
    assert(statusbar_!=0);
    QString msg="next snapshot: ";
    statusbar_->showMessage(msg);
  }
}



//=============================================================================
//=============================================================================
