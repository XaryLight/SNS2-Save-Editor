#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h> // 用于 GB18030 和 UTF-8 之间的转换

// 引入 nlohmann/json 库
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// ================= Base64 编解码工具类 =================
class Base64 {
private:
    static const std::string chars;
public:
    static std::string encode(const std::string &in) {
        std::string out;
        int val = 0, valb = -6;
        for (unsigned char c : in) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                out.push_back(chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) out.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (out.size() % 4) out.push_back('=');
        return out;
    }

    static std::string decode(const std::string &in) {
        std::string out;
        std::vector<int> T(256, -1);
        for (int i = 0; i < 64; i++) T[chars[i]] = i;

        int val = 0, valb = -8;
        for (unsigned char c : in) {
            if (T[c] == -1) break;
            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0) {
                out.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return out;
    }
};
const std::string Base64::chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// ================= 编码转换工具 (Windows API) =================
std::string GB18030ToUTF8(const std::string &gb18030Str) {
    if (gb18030Str.empty()) return "";
    int wlen = MultiByteToWideChar(54936, 0, gb18030Str.c_str(), -1, NULL, 0);
    if (wlen == 0) return "";
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(54936, 0, gb18030Str.c_str(), -1, &wstr[0], wlen);

    int ulen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (ulen == 0) return "";
    std::string utf8Str(ulen, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], ulen, NULL, NULL);
    return utf8Str;
}

std::string UTF8ToGB18030(const std::string &utf8Str) {
    if (utf8Str.empty()) return "";
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    if (wlen == 0) return "";
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], wlen);

    int glen = WideCharToMultiByte(54936, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (glen == 0) return "";
    std::string gb18030Str(glen, 0);
    WideCharToMultiByte(54936, 0, wstr.c_str(), -1, &gb18030Str[0], glen, NULL, NULL);
    return gb18030Str;
}

// ================= 核心编解码逻辑 =================
bool decode_file(const std::string &file_path, std::string &out_json, std::string &error_msg) {
    std::ifstream infile(file_path, std::ios::binary);
    if (!infile.is_open()) {
        error_msg = "无法打开文件: " + file_path;
        return false;
    }

    std::string first_line;
    std::getline(infile, first_line);
    infile.close();

    if (!first_line.empty() && first_line.back() == '\r') first_line.pop_back();
    if (first_line.empty()) {
        error_msg = "文件为空";
        return false;
    }

    try {
        std::string binary_data = Base64::decode(first_line);
        std::string reversed_str = GB18030ToUTF8(binary_data);
        std::reverse(reversed_str.begin(), reversed_str.end());

        // 使用 nlohmann/json 解析并格式化输出 (4空格缩进)
        json j = json::parse(reversed_str);
        out_json = j.dump(4); 
        return true;
    } catch (const std::exception& e) {
        error_msg = std::string("解码异常: ") + e.what();
        return false;
    }
}

bool encode_to_file(const std::string &json_content, const std::string &output_path, 
                    const std::string &original_file_path, std::string &error_msg) {
    try {
        // 1. 使用 nlohmann/json 重新解析，确保输出紧凑格式 (无多余空格)
        json j = json::parse(json_content);
        std::string compact_json = j.dump(-1); // -1 表示不缩进，紧凑输出

        // 2. 字符串反转
        std::reverse(compact_json.begin(), compact_json.end());

        // 3. UTF-8 转 GB18030
        std::string binary_data = UTF8ToGB18030(compact_json);

        // 4. Base64 编码
        std::string final_str = Base64::encode(binary_data);

        // 构建输出行
        std::vector<std::string> output_lines = {final_str};

        // 保留原文件的第2行及之后的内容
        if (!original_file_path.empty()) {
            std::ifstream orig_file(original_file_path, std::ios::binary);
            if (orig_file.is_open()) {
                std::string line;
                std::getline(orig_file, line); // 跳过第一行
                while (std::getline(orig_file, line)) {
                    if (!line.empty() && line.back() == '\r') line.pop_back();
                    output_lines.push_back(line);
                }
                orig_file.close();
            }
        }

        // 写入文件
        std::ofstream outfile(output_path, std::ios::binary);
        if (!outfile.is_open()) {
            error_msg = "无法写入文件: " + output_path;
            return false;
        }

        for (size_t i = 0; i < output_lines.size(); ++i) {
            outfile << output_lines[i];
            if (i != output_lines.size() - 1) outfile << "\r\n";
        }
        outfile.close();
        return true;
    } catch (const std::exception& e) {
        error_msg = std::string("编码异常: ") + e.what();
        return false;
    }
}

// ================= 主程序交互 =================
int main() {
    SetConsoleOutputCP(CP_UTF8); 
    std::cout << "--- 游戏存档编解码工具 (C++) ---\n";
    std::cout << "模式 1: 解码 (将存档文件转换为可读文本)\n";
    std::cout << "模式 2: 编码 (将修改后的文本转换回存档文件)\n";

    std::cout << "请输入模式 (1 或 2): ";
    std::string mode;
    std::getline(std::cin, mode);

    if (mode == "1") {
        std::cout << "请输入要解码的文件路径: ";
        std::string in_path;
        std::getline(std::cin, in_path);

        std::string result, error;
        if (decode_file(in_path, result, error)) {
            std::ofstream out("./output.json", std::ios::binary);
            out << result;
            out.close();
            std::cout << "✅ 解码成功！内容已保存至: ./output.json\n";
        } else {
            std::cout << "❌ 解码失败: " << error << "\n";
        }
    } else if (mode == "2") {
        std::cout << "请输入包含JSON内容的文件路径: ";
        std::string in_path;
        std::getline(std::cin, in_path);

        std::cout << "请输入原始存档文件路径 (直接回车则仅生成数据行): ";
        std::string original_path;
        std::getline(std::cin, original_path);

        std::cout << "请输入生成的存档文件名 (直接回车生成 new_save_file): ";
        std::string target_save;
        std::getline(std::cin, target_save);
        if (target_save.empty()) target_save = "./new_save_file";

        std::ifstream infile(in_path, std::ios::binary);
        if (!infile.is_open()) {
            std::cout << "❌ 错误：JSON文件不存在！\n";
            return 0;
        }
        std::stringstream buffer;
        buffer << infile.rdbuf();
        std::string json_content = buffer.str();
        infile.close();

        std::string error;
        if (encode_to_file(json_content, target_save, original_path, error)) {
            std::cout << "✅ 编码成功！文件已生成: " << target_save << "\n";
        } else {
            std::cout << "❌ 编码失败: " << error << "\n";
        }
    } else {
        std::cout << "无效的模式选择。\n";
    }
    return 0;
}