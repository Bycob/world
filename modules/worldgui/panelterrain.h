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

    virtual std::vector<std::unique_ptr<Resource>> getResources();
signals:

public slots:
    virtual void generate();
private:
    std::shared_ptr<Terrain> _generated;

    // Ressources
    std::unique_ptr<Image> _terrainImage;
    std::unique_ptr<Image> _texture;

    Ui::PanelTerrain *ui;
};

#endif // PANELTERRAIN_H
