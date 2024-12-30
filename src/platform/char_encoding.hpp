#ifndef CHAR_ENCODING_H_
#define CHAR_ENCODING_H_
#include <string>
#include <iostream>
#include <filesystem>

// This header exists to make sure that when working with filepaths 
// the native OS way of representing paths as strings
#ifdef _WIN32
    inline std::wstring path_to_native_string(const std::filesystem::path &p) {
        return p.wstring();
    }
    inline void print_native_path(const std::wstring &wide_str) {
        std::wcout << wide_str << "\n";
    }
#else
    inline std::string path_to_native_string(const std::filesystem::path &p) {
        return p.string();
    }
    inline void print_native_path(const std::wstring &utf8_str) {
        std::cout << uft8_str << "\n";
    }
#endif 

#endif //CHAR_ENCODING_H_