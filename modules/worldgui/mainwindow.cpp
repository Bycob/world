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
    ui(new Ui::MainWindow),
    rootEntity(new Qt3DCore::QEntity())
{
    ui->setupUi(this);

    swapGeneratePanel(new PanelTerrain());

    // Initialisation du panneau 3D
    Qt3DExtras::Qt3DWindow *window = new Qt3DExtras::Qt3DWindow();
    QWidget *container = QWidget::createWindowContainer(window);
    ui->centralWidget->layout()->replaceWidget(ui->rightPanel, container);

    window->setRootEntity(rootEntity);
    window->defaultFrameGraph()->setClearColor(QColor(QRgb(0x8f8f8f)));

    // Camera
    QSize windowSize = window->size();
    Qt3DRender::QCamera *camera = window->camera();

    camera->lens()->setPerspectiveProjection(70, (float) windowSize.width() / (float) windowSize.height(), 0.1, 100);
    camera->setPosition(QVector3D(5, 5, 5));
    camera->setViewCenter(QVector3D(0, 0, 0));
    camera->setUpVector(QVector3D(0, 0, 1));

    Qt3DExtras::QOrbitCameraController *controller = new Qt3DExtras::QOrbitCameraController(rootEntity);
    controller->setCamera(camera);

    // Lumière

}

MainWindow::~MainWindow()
{
    delete generatePanel;
    delete ui;
}

void MainWindow::setScene(const Scene *objects)
{
    for (Qt3DCore::QNode * child : rootEntity->childNodes()) {
        //child->deleteLater();
        std::cout << "blblbll" << std::endl;
    }

    for (std::shared_ptr<Mesh> mesh : objects->getMeshes()) {
        Qt3DRender::QMesh *qmesh = new Qt3DRender::QMesh(rootEntity);
        qmesh->setSource(QUrl::fromLocalFile(QStringLiteral("mesh.obj")));
        qDebug() << qmesh;

        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse(QColor(QRgb(0xff0000)));

        Qt3DCore::QEntity *entity = new Qt3DCore::QEntity(rootEntity);
        entity->addComponent(qmesh);
        entity->addComponent(material);

        Qt3DCore::QEntity *entity2 = new Qt3DCore::QEntity(rootEntity);
        Qt3DExtras::QConeMesh * cone = new Qt3DExtras::QConeMesh();
        cone->setTopRadius(0.5);
        cone->setBottomRadius(1);
        cone->setLength(3);
        cone->setRings(50);
        cone->setSlices(20);
        entity2->addComponent(cone);
        entity2->addComponent(material);
    }
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
        QObject::connect(ui->generateButton, SIGNAL(clicked(bool)), newPanel, SLOT(generate()));

        // Changement d'objets 3D
        QObject::connect(newPanel, SIGNAL(meshesChanged(const Scene*)), this, SLOT(setScene(const Scene*)));
    }
}
