#ifndef GENERATEPANEL_H
#define GENERATEPANEL_H

#include <QWidget>
#include <memory>

#include <worldapi/mesh.h>

#include "scene.h"

class GeneratePanel : public QWidget
{
    Q_OBJECT
public:
    explicit GeneratePanel(QWidget *parent = 0);

    virtual const Scene * getMeshes();
signals:
    void meshesChanged(const Scene * objects);

public slots:
    virtual void generate() = 0;

protected:
    std::unique_ptr<Scene> myScene;
};

#endif // GENERATEPANEL_H
