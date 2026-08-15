//
// Created on 2026/8/4.
//

#ifndef SNS2_EDITOR_LANG_H
#define SNS2_EDITOR_LANG_H

#include <base/qt.hpp>
#include <base/std.hpp>
#include <base/surroundings.h>
#include <base/constexpr.hpp>

namespace lang {
    inline nlohmann::json LANG;
    inline std::string FILE;
    void load_language();
}

#endif //SNS2_EDITOR_LANG_H
