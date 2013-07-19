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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>
#include <QTextEdit>

class QTextDocument;

class Highlighter : public QSyntaxHighlighter
{
  Q_OBJECT

  public:
    Highlighter( QTextDocument* parent = 0);
    Highlighter( QTextEdit* parent = 0);

    /// Updates the highlighter with the current rule set defined in the patterns
    void update();

    QStringList keywordPatterns_;
    QStringList pluginPatterns_;
    QStringList functionPatterns_;
    QStringList typePatterns_;

  protected:

    void highlightBlock(const QString &text);

  private:
    /// common initializer function called by the constructors
    void init();

    struct HighlightingRule
    {
      QRegExp pattern;
      QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules_;

    QRegExp commentStartExpression_;
    QRegExp commentEndExpression_;

    QTextCharFormat keywordFormat_;
    QTextCharFormat pluginFormat_;
    QTextCharFormat functionFormat_;
    QTextCharFormat typeFormat_;

//     QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat_;
    QTextCharFormat multiLineCommentFormat_;
    QTextCharFormat quotationFormat_;
    QTextCharFormat listFormat_;
//     QTextCharFormat functionFormat;
};

#endif
