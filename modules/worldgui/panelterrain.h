#ifndef PANELTERRAIN_H
#define PANELTERRAIN_H

#include <QWidget>

#include <memory>
#include <worldapi/terrain/terrain.h>

#include "generatepanel.h"

namespace Ui {
class PanelTerrain;
}

class PanelTerrain : public GeneratePanel
{
    Q_OBJECT

public:
    explicit PanelTerrain(QWidget *parent = 0);
    ~PanelTerrain();
signals:

public slots:
    virtual void generate();
private:
    std::shared_ptr<Terrain> generated;

    Ui::PanelTerrain *ui;
};

#endif // PANELTERRAIN_H
