#ifndef CUSTOMLEXERLUA_H
#define CUSTOMLEXERLUA_H

#include <Qsci/qscilexerlua.h>
#include <QStringList>
#include <QColor>

class CustomLexerLua : public QsciLexerLua
{
    Q_OBJECT

public:
    CustomLexerLua(QObject* parent = nullptr);

    const char* keywords(int set) const override;
    void setCustomKeywordsColor(const QColor& color);

private:
    QStringList m_customKeywords;
    QColor m_customKeywordsColor;
};

#endif // CUSTOMLEXERLUA_H
