#ifndef PREVIEWPANEL3D_H
#define PREVIEWPANEL3D_H

#include <QWidget>
#include <Qt3DCore/QEntity>

#include <worldcore.h>

class PreviewPanel3D {
public:
    PreviewPanel3D(QWidget *parent = Q_NULLPTR);
    QWidget *getWidget() const;

public slots:

    void setScene(const world::Scene *scene);

private:
    QWidget *_widget = nullptr;
    Qt3DCore::QEntity *_rootEntity = nullptr;
};

#endif // PREVIEWPANEL3D_H
