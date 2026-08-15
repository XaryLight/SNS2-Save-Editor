

#include "Select.h"
#include <res/icon.h>

void gui::Select::setupUI() {// 测试：使用 Qt 内置图标
    //setWindowIcon(getAppIcon());
    //m_button = new QPushButton("A");
}

void gui::Select::run() {
    show();
}

gui::Select::Select(QWidget* parent) : QMainWindow(parent) {
    setupUI();
}


