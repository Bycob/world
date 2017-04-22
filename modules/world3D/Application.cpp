#include "Application.h"

#include "util.h"

Application::Application(int argc, char **argv)
        : _running(false), _mainView(std::make_unique<MainView>(*this)) {

}

void Application::run() {
    _running = true;
    _mainView->show();

    while(_running) {
        sleep(20);
    }

    _mainView->waitClose();
}

void Application::requestStop() {
    _running = false;
}