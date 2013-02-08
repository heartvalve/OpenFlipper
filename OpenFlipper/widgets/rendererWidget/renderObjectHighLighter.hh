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
*   $Revision: 12438 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-09-22 17:16:17 +0200 (Do, 22 Sep 2011) $                     *
*                                                                            *
\*===========================================================================*/

#pragma once

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

class QTextDocument;

class RenderObjectHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

  public:
    RenderObjectHighlighter( QTextDocument* parent = 0);
    RenderObjectHighlighter( QTextEdit* parent = 0);

    /// Updates the highlighter with the current rule set defined in the patterns
    void update();

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



    // Single line rules
    QStringList keywordPatterns_;
    QStringList typePatterns_;

    // All single line rules combined with their format
    QVector<HighlightingRule> highlightingRules_;

    // Multi line rules
    QRegExp vertexShaderStartExpression_;
    QRegExp vertexShaderEndExpression_;

    QRegExp geometryShaderStartExpression_;
    QRegExp geometryShaderEndExpression_;

    QRegExp fragmentShaderStartExpression_;
    QRegExp fragmentShaderEndExpression_;

    // Single formats
    QTextCharFormat keywordFormat_;
    QTextCharFormat defineFormat_;
    QTextCharFormat typeFormat_;

    QTextCharFormat singleLineCommentFormat_;

    // Multi Line formats
    QTextCharFormat vertexShaderFormat_;
    QTextCharFormat geometryShaderFormat_;
    QTextCharFormat fragmentShaderFormat_;

};

