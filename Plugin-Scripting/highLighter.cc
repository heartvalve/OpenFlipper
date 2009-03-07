#include <QtGui>

#include "highLighter.hh"

 Highlighter::Highlighter(QTextEdit *parent)
  : QSyntaxHighlighter(parent)
{
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
  keywordPatterns_ << "while" << "for" << "print" << "var";

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
