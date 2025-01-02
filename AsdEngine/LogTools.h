#ifndef LOGTOOLS_H
#define LOGTOOLS_H

#include <sstream>
#include <string>
#include <spdlog/spdlog.h>

// 递归终止函数，处理参数包展开的最后一个参数
void combineToString(std::ostringstream& oss);

// 递归函数模板，处理每个参数并将其转换为字符串
template <typename T, typename... Args>
void combineToString(std::ostringstream& oss, T&& first, Args&&... args) {
    oss << std::forward<T>(first); // 将第一个参数转换为字符串并添加到字符串流中
    oss << " "; // 加个空格
    if constexpr (sizeof...(args) > 0) { // 如果还有剩余参数
        combineToString(oss, std::forward<Args>(args)...); // 递归处理剩余参数
    }
}

// 组合字符串的函数，接受可变参数并将其转换为一个字符串
template <typename... Args>
std::string combineToString(Args&&... args) {
    std::ostringstream oss;
    combineToString(oss, std::forward<Args>(args)...);
    return oss.str(); // 返回组合后的字符串
}

// 打印组合字符串的函数
template <typename... Args>
void printCombinedString(Args&&... args) {
    std::string result = combineToString(std::forward<Args>(args)...);
    spdlog::debug(result.c_str());
}

#endif // LOGTOOLS_H
