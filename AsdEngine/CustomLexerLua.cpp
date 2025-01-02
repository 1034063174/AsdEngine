#include "CustomLexerLua.h"

CustomLexerLua::CustomLexerLua(QObject* parent)
    : QsciLexerLua(parent), m_customKeywordsColor(QColor(255, 140, 0))  // 设置默认颜色为橙色 (dark orange)
{
    // 添加自定义关键字
    m_customKeywords << "Myadd" << "print" << "sleep" << "Asm2Code" << "readmem" << "writemem"
        << "allocmem" << "changememprot" << "createremotethread" << "Send";

    // 设置自定义关键字的颜色
    setCustomKeywordsColor(m_customKeywordsColor);
}

const char* CustomLexerLua::keywords(int set) const
{
    if (set == 1) {
        // 获取原始的 Lua 关键字
        QStringList keywords = QString(QsciLexerLua::keywords(set)).split(' ');

        // 将自定义关键字添加到 Lua 的关键字集中
        keywords.append(m_customKeywords);

        // 将关键字列表转换为字符串
        static QByteArray keywordBytes = keywords.join(' ').toUtf8();
        return keywordBytes.constData();
    }

    return QsciLexerLua::keywords(set);
}

void CustomLexerLua::setCustomKeywordsColor(const QColor& color)
{
    m_customKeywordsColor = color;
    setColor(m_customKeywordsColor, QsciLexerLua::Keyword);
}
