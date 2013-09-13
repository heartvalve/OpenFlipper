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
*   $Revision: 15910 $                                                       *
*   $LastChangedBy: moeller $                                                *
*   $Date: 2012-12-05 12:53:39 +0100 (Mi, 05 Dez 2012) $                     *
*                                                                            *
\*===========================================================================*/

#include "ui_postProcessorWidget.hh"

#include <vector>

#include <QDialog>
#include <QPoint>
#include <QStringList>
class QWidget;
class QFrame;
class PostProcessorInfo;

class PostProcessorDialog : public QDialog, public Ui::PostProcessorWidget
{
  Q_OBJECT

  signals:
    /// request an update for an specified viewer
    void updateExaminer(unsigned _viewer);

  public:
    PostProcessorDialog(QWidget *_parent = 0);

    /// return the names of all saved post processors
    static QStringList getSavedPostProcessorNames(const unsigned _examiner);

    /// append all saved post processors
    static void loadSavedPostProcessors(const unsigned _examiner);

  public slots:
    /// refreshes the content of the dialog with current examiner
    void refresh();

  private slots:
    /// Show the custom context menu for activation
    void slotContextMenuActivate(const QPoint& _point);

    /// Show the custom context menu for deactivation
    void slotContextMenuDeactivate(const QPoint& _point);

    /// Activates the post processor (triggered via the context menu)
    void slotActivatePostProcessor();

    /// Deactivates the current postProcessor
    void slotDeactivatePostProcessor();

    /// Move the position/ordering of postprocessor in the postprocessor
    void slotMovePostProcessor(unsigned _from, unsigned _to);

    /// move the selected active postprocessor 1 up
    void slotMoveUp();

    /// move the selected active postprocessor 1 down
    void slotMoveDown();

    /// saves active post processor chain
    void slotSaveActive();

  protected:
    void closeEvent(QCloseEvent *_event);

    void showEvent ( QShowEvent * );

  private:
    /// initiaize the window with the post processors of the current examiner
    void initWindow();

    QFrame* createFrame(const PostProcessorInfo& _pPI);

    /// holds the examiner id for the window
    unsigned currentExaminer_;

    /// maps activeRow from activeList (same as chainIdx from RendererInfo) to row from list (same as activeId from RendererInfo) for each viewer
    std::vector<unsigned> activeRowToRow_;

};

