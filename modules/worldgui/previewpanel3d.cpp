#include "previewpanel3d.h"

#include <stdexcept>

#include <QtGui/QScreen>
#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qforwardrenderer.h>
#include <Qt3DExtras/qorbitcameracontroller.h>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/qphongmaterial.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qcamera.h>

PreviewPanel3D::PreviewPanel3D(QWidget * parent) :
    _rootEntity(new Qt3DCore::QEntity())
{

    Qt3DExtras::Qt3DWindow *window = new Qt3DExtras::Qt3DWindow();
    _widget = QWidget::createWindowContainer(window, parent);

    window->setRootEntity(_rootEntity);
    window->defaultFrameGraph()->setClearColor(QColor(QRgb(0x8f8f8f)));

    // Camera
    QSize windowSize = window->size();
    Qt3DRender::QCamera *camera = window->camera();

    camera->lens()->setPerspectiveProjection(70, (float) windowSize.width() / (float) windowSize.height(), 0.1, 100);
    camera->setPosition(QVector3D(5, 5, 5));
    camera->setViewCenter(QVector3D(0, 0, 0));
    camera->setUpVector(QVector3D(0, 0, 1));

    Qt3DExtras::QOrbitCameraController *controller = new Qt3DExtras::QOrbitCameraController(_rootEntity);
    controller->setCamera(camera);

    // Lumière...
}

void PreviewPanel3D::setScene(const Scene * objects) {
    for (Qt3DCore::QNode * child : _rootEntity->childNodes()) {
        //child->deleteLater();
    }

    for (std::shared_ptr<Mesh> mesh : objects->getMeshes()) {
        Qt3DRender::QMesh *qmesh = new Qt3DRender::QMesh(_rootEntity);
        qmesh->setSource(QUrl::fromLocalFile(QStringLiteral("mesh.obj")));
        qDebug() << qmesh;

        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse(QColor(QRgb(0xff0000)));

        Qt3DCore::QEntity *entity = new Qt3DCore::QEntity(_rootEntity);
        entity->addComponent(qmesh);
        entity->addComponent(material);

        Qt3DCore::QEntity *entity2 = new Qt3DCore::QEntity(_rootEntity);
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

QWidget * PreviewPanel3D::getWidget() const {
    return _widget;
}