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
*   $Revision: 12438 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-09-22 17:16:17 +0200 (Thu, 22 Sep 2011) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef TEXTBROWSERWIDGET_HH
#define TEXTBROWSERWIDGET_HH

#include <QPlainTextEdit>
#include <QObject>

#include <vector>

class TextBrowserSideArea;


class TextBrowserWidget : public QPlainTextEdit
{
    Q_OBJECT

  private:
    enum FoldType {
      SHADER,
      RENDEROBJECT
    };

    struct Fold {
      Fold() :
        start(-1),
        end(-1),
        folded(false),
        type(SHADER)
      {}

      Fold(int _start, int _end, FoldType _type) :
        start(_start),
        end(_end),
        folded(false),
        type(_type)
      {}

      bool contains (int n) const {
        return (start <= n) && (n <= end);
      }

      // start positition in the document
      int start;
      // end positition in the document
      int end;
      bool folded;
      FoldType type;
    };


  public:
    TextBrowserWidget(QWidget *parent = 0);

    void sideAreaPaintEvent(QPaintEvent *event);
    int sideAreaWidth();

  protected:
    void resizeEvent(QResizeEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent* e);

  private slots:
    void updateTextBrowserSideAreaWidth();
    void updateTextBrowserSideArea(const QRect &, int);
    void foldAll();
    void unfoldAll();
    void fold(Fold& _fold);
    void unfold(Fold& _fold);
    void toggleFold(int _position);
    void updateFolds();

    /** \brief get the _fold corresponding to the document _position
     *
     * @param _position position in the document
     * @param _fold     fold in folds at the position if found
     * @return          true if fold was found (otherwise false is returned)
     */
    bool getFold(int _position, Fold& _fold);

  private:
    TextBrowserSideArea* sideArea_;
    static QString const startRenderObjectTag_;
    static QString const startVertexShaderTag_;
    static QString const endVertexShaderTag_;
    static QString const startTessControlShaderTag_;
    static QString const endTessControlShaderTag_;
    static QString const startTessEvalShaderTag_;
    static QString const endTessEvalShaderTag_;
    static QString const startGeometryShaderTag_;
    static QString const endGeometryShaderTag_;
    static QString const startFragmentShaderTag_;
    static QString const endFragmentShaderTag_;

    std::vector<Fold> folds_;
    /// maps positions in the document to indices in folds_
    std::map<int, size_t> blockPosToFold_;
};


class TextBrowserSideArea : public QWidget
{
  public:
    TextBrowserSideArea(TextBrowserWidget* _textBrowser) :
      QWidget(_textBrowser),
      textBrowser_(_textBrowser)
  {}

    QSize sizeHint() const {
      return QSize(textBrowser_->sideAreaWidth(), 0);
    }

  protected:
    void paintEvent(QPaintEvent *event) {
      textBrowser_->sideAreaPaintEvent(event);
    }

  private:
    TextBrowserWidget* textBrowser_;
};


#endif // TEXTBROWSERWIDGET_HH
