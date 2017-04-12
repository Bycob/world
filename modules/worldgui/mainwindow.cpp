#include <iostream>

#include <QtGui/QScreen>
#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qforwardrenderer.h>
#include <Qt3DExtras/qorbitcameracontroller.h>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/qphongmaterial.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qcamera.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "panelterrain.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    swapGeneratePanel(new PanelTerrain());
    ui->_3DTab->layout()->replaceWidget(ui->_3DPanel, _3DPanel.getWidget());
}

MainWindow::~MainWindow()
{
    delete generatePanel;
    delete ui;
}

void MainWindow::setScene(const Scene *objects)
{
    _3DPanel.setScene(objects);
}

void MainWindow::setImage(const QImage *image) { // TODO check s'il y a pas memory leak sur cette QImage
    if (this->ui->imagePanel->scene() == nullptr) {
        this->ui->imagePanel->setScene(new QGraphicsScene(this));
    }

    QGraphicsScene *scene = this->ui->imagePanel->scene();
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(*image));
    scene->setSceneRect(image->rect());
}

void MainWindow::generate()
{
    generatePanel->generate();
}

void MainWindow::swapGeneratePanel(GeneratePanel *newPanel)
{
    // Remplacement
    if (newPanel != nullptr) {
        if (generatePanel != nullptr) {
            ui->generateTab->layout()->replaceWidget(generatePanel, newPanel);
            delete generatePanel;
        }
        else {
            ui->generateTab->layout()->replaceWidget(ui->generatePanel, newPanel);
        }
    }
    else {
        if (generatePanel != nullptr) {
            ui->generateTab->layout()->replaceWidget(generatePanel, ui->generatePanel);
            delete generatePanel;
        }
    }
    generatePanel = newPanel;

    if (newPanel != nullptr) {
        // Connexion signal / slot
        // Géneration d'objets
        QObject::connect(ui->generateButton, SIGNAL(clicked(bool)), this, SLOT(generate()));
        // Changement d'objets 3D
        QObject::connect(newPanel, SIGNAL(meshesChanged(const Scene*)), this, SLOT(setScene(const Scene*)));
        // Changement d'image
        QObject::connect(newPanel, SIGNAL(imageChanged(const QImage*)), this, SLOT(setImage(const QImage*)));
    }
}
