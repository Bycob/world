#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qt3DCore/QEntity>

#include <world/core.h>

#include "generatepanel.h"
#include "previewpanel3d.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void changeGeneratePanel();

    void generate();

    void setScene(const world::Scene *objects);
    void setImage(const QImage *image);

private:
    Ui::MainWindow *ui;
    GeneratePanel *generatePanel = nullptr;
    PreviewPanel3D _3DPanel;

    void swapGeneratePanel(GeneratePanel *newPanel);
};

#endif // MAINWINDOW_H
