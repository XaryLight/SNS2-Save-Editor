
#include <res/lang.h>
#include <gui/Select.h>
#include <base/surroundings.h>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    surroundings::app = &app;

    lang::load_language();
    const auto root = new gui::Select;
    root->show();

    return app.exec();
}
