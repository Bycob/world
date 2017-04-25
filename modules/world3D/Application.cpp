#include "Application.h"

#include "util.h"

Application::Application()
        : _running(false), _mainView(std::make_unique<MainView>(*this)) {

}

void Application::run(int argc, char **argv) {
    loadWorld(argc, argv);

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

void Application::loadWorld(int argc, char **argv) {

}