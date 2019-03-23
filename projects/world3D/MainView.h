//
// Created by louis on 22/04/17.
//

#ifndef WORLD_MAINVIEW_H
#define WORLD_MAINVIEW_H

#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include <irrlicht.h>

#include "DebugScreenNode.h"

class Application;
class RenderingModule;

class MainView : public irr::IEventReceiver {
public:
    MainView(Application &app);
    ~MainView();

    void show();

    bool running();
    void waitClose();

    void resetScene();
    void onWorldChange();

    bool OnEvent(const irr::SEvent &event) override;

private:
    Application &_app;

    std::atomic_bool _running;
    std::unique_ptr<std::thread> _graphicThread;

    irr::video::IVideoDriver *_driver;
    irr::IrrlichtDevice *_device;
    irr::scene::ISceneManager *_scenemanager;

    std::atomic_bool _resetScene;
    std::atomic_bool _worldChanged;

    // Irrlicht nodes
    irr::scene::ICameraSceneNode *_camera;
    DebugScreenNode *_debug;

    std::vector<std::unique_ptr<RenderingModule>> _modules;

    bool _fpsModeActive = true;
    bool _cameraFast = true;

    void runInternal();
    void recreateModules();
    void updateScene();
};


#endif // WORLD_MAINVIEW_H
