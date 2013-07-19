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

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif


#include "highLighter.hh"

 Highlighter::Highlighter(QTextDocument *parent)
  : QSyntaxHighlighter(parent)
{
 init();
}

Highlighter::Highlighter(QTextEdit *parent)
: QSyntaxHighlighter(parent)
{
  init();
}

 void Highlighter::init() {
   // Set the basic format styles
   keywordFormat_.setForeground(Qt::darkGreen);
   keywordFormat_.setFontWeight(QFont::Bold);

   pluginFormat_.setForeground(Qt::darkBlue);
   pluginFormat_.setFontWeight(QFont::Bold);

   functionFormat_.setForeground(Qt::darkYellow);
   functionFormat_.setFontWeight(QFont::Bold);

   typeFormat_.setForeground(Qt::darkMagenta);
   typeFormat_.setFontWeight(QFont::Bold);

   quotationFormat_.setForeground(Qt::darkRed);

   listFormat_.setForeground(Qt::darkRed);

   singleLineCommentFormat_.setForeground(Qt::red);
   multiLineCommentFormat_.setForeground(Qt::red);

   commentStartExpression_ = QRegExp("/\\*");
   commentEndExpression_   = QRegExp("\\*/");

   // Define basic keywords
   keywordPatterns_ << "while" << "for" << "print" << "var" << "break" << "if";

   // Types which are accepted by the scripting system
   typePatterns_ << "int" << "Matrix4x4" << "QString" << "idList" << "bool" << "Vector" << "double";

   update();

   //   classFormat.setFontWeight(QFont::Bold);
   //   classFormat.setForeground(Qt::darkMagenta);
   //   rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
   //   rule.format = classFormat;
   //   highlightingRules.append(rule);
   //



   //   functionFormat.setFontItalic(true);
   //   functionFormat.setForeground(Qt::blue);
   //   rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
   //   rule.format = functionFormat;
   //   highlightingRules.append(rule);
   //
}

void Highlighter::update() {

  highlightingRules_.clear();

  HighlightingRule rule;

  // Create Rules for keywords
  foreach (QString pattern, keywordPatterns_) {
    rule.pattern = QRegExp("\\b" + pattern + "\\b" );
    rule.format = keywordFormat_;
    highlightingRules_.append(rule);
  }

  // Create Rules for plugins
  foreach (QString pattern, pluginPatterns_ ) {
    rule.pattern = QRegExp("\\b" + pattern + "\\b" );
    rule.format = pluginFormat_;
    highlightingRules_.append(rule);
  }

  // Create Rules for functions
  foreach (QString pattern, functionPatterns_ ) {
    rule.pattern = QRegExp("\\b" + pattern + "\\b" );
    rule.format = functionFormat_;
    highlightingRules_.append(rule);
  }

  // Create Rules for types
  foreach (QString pattern, typePatterns_ ) {
    rule.pattern = QRegExp("\\b" + pattern + "\\b" );
    rule.format = typeFormat_;
    highlightingRules_.append(rule);
  }

  // Rule for single line comments
  rule.pattern = QRegExp("//[^\n]*");
  rule.format = singleLineCommentFormat_;
  highlightingRules_.append(rule);

  // Rules for quotations
  rule.pattern = QRegExp("\".*\"");
  rule.format = quotationFormat_;
  highlightingRules_.append(rule);

  // Rules for Lists
  rule.pattern = QRegExp("\\[.*\\]");
  rule.format = listFormat_;
  highlightingRules_.append(rule);

}

 void Highlighter::highlightBlock(const QString &text)
{

   foreach (HighlightingRule rule, highlightingRules_) {
    QRegExp expression(rule.pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = text.indexOf(expression, index + length);
    }
  }
  setCurrentBlockState(0);

  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = text.indexOf(commentStartExpression_);

  while (startIndex >= 0) {
    int endIndex = text.indexOf(commentEndExpression_, startIndex);
    int commentLength;
    if (endIndex == -1) {
      setCurrentBlockState(1);
      commentLength = text.length() - startIndex;
    } else {
      commentLength = endIndex - startIndex + commentEndExpression_.matchedLength();
    }
    setFormat(startIndex, commentLength, multiLineCommentFormat_);
    startIndex = text.indexOf(commentStartExpression_, startIndex + commentLength);
  }

}
