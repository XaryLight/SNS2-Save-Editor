// Constexpr
//

#ifndef SNS2_EDITOR_CONSTEXPR_H
#define SNS2_EDITOR_CONSTEXPR_H

#pragma once
#include <base/std.hpp>

constexpr int WIN_SORT_T{ 2 };

constexpr std::array<std::pair<int, int>, WIN_SORT_T> WIN_SIZE_T{{
    {200, 200},
    {720, 640}
}};

constexpr const char * ICON_PATH
    {":/icons/icon.ico"};// icon path
constexpr const char * SAVE_DATA_PATH
    {"C:/Users/{:s}/AppData/LocalLow/Uu/SiNiSistar2/SaveData_S"};//save path

#endif //SNS2_EDITOR_CONSTEXPR_H
