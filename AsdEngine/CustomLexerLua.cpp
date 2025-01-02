#include "CustomLexerLua.h"

CustomLexerLua::CustomLexerLua(QObject* parent)
    : QsciLexerLua(parent), m_customKeywordsColor(QColor(255, 140, 0))  // ����Ĭ����ɫΪ��ɫ (dark orange)
{
    // ����Զ���ؼ���
    m_customKeywords << "Myadd" << "print" << "sleep" << "Asm2Code" << "readmem" << "writemem"
        << "allocmem" << "changememprot" << "createremotethread" << "Send";

    // �����Զ���ؼ��ֵ���ɫ
    setCustomKeywordsColor(m_customKeywordsColor);
}

const char* CustomLexerLua::keywords(int set) const
{
    if (set == 1) {
        // ��ȡԭʼ�� Lua �ؼ���
        QStringList keywords = QString(QsciLexerLua::keywords(set)).split(' ');

        // ���Զ���ؼ�����ӵ� Lua �Ĺؼ��ּ���
        keywords.append(m_customKeywords);

        // ���ؼ����б�ת��Ϊ�ַ���
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
