
#ifndef SNS2_EDITOR_ICON_H
#define SNS2_EDITOR_ICON_H

#include <base/qt.hpp>
#include <base/constexpr.hpp>

inline QIcon getAppIcon() {
    static QIcon icon(ICON_PATH);
    return icon;
}

#endif //SNS2_EDITOR_ICON_H
