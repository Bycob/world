//
// Created by louis on 22/04/17.
//

#ifndef WORLD_MAINVIEW_H
#define WORLD_MAINVIEW_H

#include <memory>
#include <atomic>
#include <mutex>
#include <thread>

#include <irrlicht/irrlicht.h>

class Application;

class MainView {
public:
    MainView(Application & app);
    ~MainView();

    void show();

    void waitClose();
private:
    Application & _app;

    std::atomic_bool _running;
    std::unique_ptr<std::thread> _graphicThread;

    irr::video::IVideoDriver *_driver ;
    irr::IrrlichtDevice *_device ;
    irr::scene::ISceneManager *_scenemanager ;

    irr::scene::ICameraSceneNode *_camera;

    void runInternal();
    void updateScene();
};


#endif //WORLD_MAINVIEW_H
