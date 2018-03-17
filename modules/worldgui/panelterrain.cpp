#include <iostream>

#include <memory>

#include <worldapi/terrain/TerrainGenerator.h>
#include <worldapi/terrain/terrain.h>
#include <worldapi/mesh.h>
#include <worldapi/assets/ObjLoader.h>

#include "panelterrain.h"
#include "ui_panelterrain.h"
#include "qtworld.h"

#define MAX_MESH_SIZE 350
 // max ~100000 vertices

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

    PerlinTerrainGenerator generator(size, 0, octaves, frequency, persistence);
    _generated = std::shared_ptr<Terrain>(generator.generate());

    if (texture) {
        Perlin perlin;

        //---
        TerrainTexmapBuilder texmapBuilder(0, 255);

        std::vector<ColorPart> slice1;
        slice1.push_back(ColorPart(47.0 / 255, 128.0 / 255, 43.0 / 255, 0.75));
        slice1.push_back(ColorPart(65.0 / 255, 53.0 / 255, 22.0 / 255, 0.25));

        std::vector<ColorPart> slice2;
        slice2.push_back(ColorPart(47.0 / 255, 128.0 / 255, 43.0 / 255, 0.15));
        slice2.push_back(ColorPart(65.0 / 255, 53.0 / 255, 22.0 / 255, 0.25));
        slice2.push_back(ColorPart(0.25, 0.25, 0.25, 0.6));

        std::vector<ColorPart> slice3;
        slice3.push_back(ColorPart(1, 1, 1, 0.7));
        slice3.push_back(ColorPart(0.25, 0.25, 0.25, 0.3));

        texmapBuilder.addSlice(1, slice1);
        texmapBuilder.addSlice(180, slice2);
        texmapBuilder.addSlice(230, slice3);
        //---

        arma::Mat<double> randomArray = perlin.generatePerlinNoise2D(size * 8, 0, 7, 16, (float)0.9);
        Image texture = generator.generateTexture(*_generated, texmapBuilder, randomArray);

        emit imageChanged(QtWorld::getQImage(texture));
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
        this->_myScene->addMesh(std::shared_ptr<Mesh>(_generated->convertToMesh()));
    }

    // On indique qu'elle a changé
    emit meshesChanged(this->_myScene.get());

    // Image
    if (!texture) {
        emit imageChanged(QtWorld::getQImage(_generated->convertToImage()));
    }
}
