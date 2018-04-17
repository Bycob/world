#include <iostream>

#include <memory>

#include "panelterrain.h"
#include "ui_panelterrain.h"
#include "qtworld.h"

#define MAX_MESH_SIZE 350
 // max ~100000 vertices

using namespace world;

PanelTerrain::PanelTerrain(QWidget *parent) :
    GeneratePanel(parent),
    ui(new Ui::PanelTerrain)
{
    ui->setupUi(this);
}

PanelTerrain::~PanelTerrain()
{
    delete ui;
}

std::vector<std::unique_ptr<Resource>> PanelTerrain::getResources() {
    auto result = GeneratePanel::getResources();

    result.emplace_back(new ImageResource("carte de reliefs", _terrainImage.get()));
    result.emplace_back(new ImageResource("texture du terrain", _texture.get()));

    return result;
}

void PanelTerrain::generate()
{
    int size = this->ui->size_field->value();
    int octaves = this->ui->octaves_field->value();
    int frequency = this->ui->frequency_field->value();
    double persistence = this->ui->persistence_field->value();
    bool texture = ui->textureCheckBox->isChecked();

    PerlinTerrainGenerator generator(0, octaves, frequency, persistence);
    _generated = std::make_shared<Terrain>(size);
    generator.process(*_generated);

    if (texture) {
        /*emit imageChanged(QtWorld::getQImage(_texture));*/
    }

    // Ecriture du mesh (temporaire)
    /*ObjLoader file;
    std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(generated->convertToMesh());
    file.addMesh(mesh);

    std::cout << "Ecriture du mesh" << std::endl;
    file.write("mesh");
    std::cout << "Mesh ecrit" << std::endl;*/

    // Setup de ma scène
    this->_myScene = std::make_unique<Scene>();

    if (size < MAX_MESH_SIZE) {
        std::unique_ptr<world::Mesh> mesh(_generated->createMesh());
        this->_myScene->addObject(*mesh);
    }

    // On indique qu'elle a changé
    emit meshesChanged(this->_myScene.get());

    // Image
    if (!texture) {
        emit imageChanged(QtWorld::getQImage(_generated->createImage()));
    }
}
