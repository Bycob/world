#ifndef PANELTERRAIN_H
#define PANELTERRAIN_H

#include <QWidget>

#include <memory>

#include <worldcore.h>
#include <worldterrain.h>

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

    virtual std::vector<std::unique_ptr<Resource>> getResources();
signals:

public slots:
    virtual void generate();
private:
    std::shared_ptr<world::Terrain> _generated;

    // Ressources
    std::unique_ptr<world::Image> _terrainImage;
    std::unique_ptr<world::Image> _texture;

    Ui::PanelTerrain *ui;
};

#endif // PANELTERRAIN_H
