#include "panelworldmap.h"
#include "ui_panelworldmap.h"

#include <worldapi/world/WorldMapGenerator.h>

#include "qtworld.h"

PanelWorldMap::PanelWorldMap(QWidget *parent) :
    GeneratePanel(parent),
    ui(new Ui::PanelWorldMap)
{
    ui->setupUi(this);
}

PanelWorldMap::~PanelWorldMap()
{
    delete ui;
}

void PanelWorldMap::generate() {
    WorldMapGenerator generator(500, 400);

    double biomeDensity = ui->biomeDensity->value();
    int limitBrightness = ui->limitBrigntness->value();

    generator.emplaceReliefMapGenerator<CustomWorldRMGenerator>(biomeDensity, limitBrightness);

    std::unique_ptr<WorldMap> generated(generator.generate());
    img::Image img = generated->getReliefMapAsImage();

    emit imageChanged(QtWorld::getQImage(img));

    generated.reset();
}
