#include "lang.h"
#include <res/place.h>
#include <base/surroundings.h>

namespace lang {
    void load_language() {
            // 1. 创建翻译器对象
            QTranslator translator;

            // 2. 获取当前系统的语言 (例如：中文)
            QLocale locale;
            QString localeName = locale.name(); // 返回 "zh_CN" 或 "en_US" 等

            // 3. 加载 .qm 文件
            // 注意：路径可以是资源系统 :/ 或者本地相对路径 zh_CN.qm
            // 通常建议将翻译文件放在 resources.qrc 中，方便打包
            if (translator.load(localeName, ":/i18n/")) {
                // 如果 load 返回 true，表示成功加载了对应的 .qm 文件
                surroundings::app->installTranslator(&translator);
            }
            // --- 执行业务逻辑 ---
            // 这里使用 tr() 的字符串会自动根据翻译器进行替换
            // qDebug() << tr("Hello World"); // 若加载 zh_CN，输出 "你好世界"
    }
}



