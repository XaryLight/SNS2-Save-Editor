// Select windows select the saves

#pragma once
#ifndef SNS2_EDITOR_SELECT_H
#define SNS2_EDITOR_SELECT_H

#include <base/std.hpp>
#include <base/qt.hpp>

namespace gui{
    class Select: public QMainWindow {
        Q_OBJECT
        public:
            void run();
            explicit Select(QWidget* parent = nullptr);
        signals:
            void onButtonClicked();
        public slots:
        private slots:
            void setupUI();
        protected:
            QPushButton *m_button;
            QLabel *m_label;
        protected slots:
    };
}

#endif //SNS2_EDITOR_SELECT_H
