#ifndef GENERATEPANEL_H
#define GENERATEPANEL_H

#include <QWidget>
#include <memory>

#include <worldcore.h>

#include "resource.h"

class GeneratePanel : public QWidget
{
    Q_OBJECT
public:
    explicit GeneratePanel(QWidget *parent = 0);

    virtual const world::Scene * getMeshes();

    virtual std::vector<std::unique_ptr<Resource>> getResources();
signals:
    void meshesChanged(const world::Scene * objects);
    void imageChanged(const QImage * image);

public slots:
    virtual void generate() = 0;

protected:
    std::unique_ptr<world::Scene> _myScene;
};

#endif // GENERATEPANEL_H
