#include <iostream>

#include <memory>

#include <worldapi/terrain/TerrainGenerator.h>
#include <worldapi/terrain/terrain.h>
#include <worldapi/mesh.h>
#include <worldapi/objfile.h>

#include "panelterrain.h"
#include "ui_panelterrain.h"

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

void PanelTerrain::generate()
{
    int size = this->ui->size_field->value();
    int octaves = this->ui->octaves_field->value();
    int frequency = this->ui->frequency_field->value();
    double persistence = this->ui->persistence_field->value();

    PerlinTerrainGenerator generator(size, 0, octaves, frequency, persistence);
    generated = std::shared_ptr<Terrain>(generator.generate().release());

    // Ecriture du mesh (temporaire)
    ObjLoader file;
    std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(generated->convertToMesh());
    file.addMesh(mesh);

    std::cout << "Ecriture du mesh" << std::endl;
    file.write("mesh");
    std::cout << "Mesh ecrit" << std::endl;

    // Setup de ma scène
    this->myScene = std::make_unique<Scene>();
    this->myScene->addMesh(std::shared_ptr<Mesh>(generated->convertToMesh()));

    // On indique qu'elle a changé
    emit meshesChanged(this->myScene.get());
}
