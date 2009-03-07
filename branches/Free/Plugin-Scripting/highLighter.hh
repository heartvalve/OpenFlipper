#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

class QTextDocument;

class Highlighter : public QSyntaxHighlighter
{
  Q_OBJECT

  public:
    Highlighter(QTextEdit *parent = 0);

    // Updates the highlighter with the current rule set defined in the patterns
    void update();

    QStringList keywordPatterns_;
    QStringList pluginPatterns_;
    QStringList functionPatterns_;
    QStringList typePatterns_;

  protected:

    void highlightBlock(const QString &text);

  private:

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
