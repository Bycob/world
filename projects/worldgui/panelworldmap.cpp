#include "panelworldmap.h"
#include "ui_panelworldmap.h"

#include <worldcore.h>
#include <worldterrain.h>

#include "qtworld.h"

using namespace world;

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

    double biomeDensity = ui->biomeDensity->value();
    int limitBrightness = ui->limitBrigntness->value();

    CustomWorldRMModifier generator(biomeDensity, limitBrightness);
    generator.setMapResolution(513);

    auto &result = generator.obtainMap(0, 0);
    Image img = result.first.createImage();

    emit imageChanged(QtWorld::getQImage(img));
}
