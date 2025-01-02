#ifndef LOGTOOLS_H
#define LOGTOOLS_H

#include <sstream>
#include <string>
#include <spdlog/spdlog.h>

// �ݹ���ֹ���������������չ�������һ������
void combineToString(std::ostringstream& oss);

// �ݹ麯��ģ�壬����ÿ������������ת��Ϊ�ַ���
template <typename T, typename... Args>
void combineToString(std::ostringstream& oss, T&& first, Args&&... args) {
    oss << std::forward<T>(first); // ����һ������ת��Ϊ�ַ�������ӵ��ַ�������
    oss << " "; // �Ӹ��ո�
    if constexpr (sizeof...(args) > 0) { // �������ʣ�����
        combineToString(oss, std::forward<Args>(args)...); // �ݹ鴦��ʣ�����
    }
}

// ����ַ����ĺ��������ܿɱ����������ת��Ϊһ���ַ���
template <typename... Args>
std::string combineToString(Args&&... args) {
    std::ostringstream oss;
    combineToString(oss, std::forward<Args>(args)...);
    return oss.str(); // ������Ϻ���ַ���
}

// ��ӡ����ַ����ĺ���
template <typename... Args>
void printCombinedString(Args&&... args) {
    std::string result = combineToString(std::forward<Args>(args)...);
    spdlog::debug(result.c_str());
}

#endif // LOGTOOLS_H
