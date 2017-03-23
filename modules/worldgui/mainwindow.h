#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qt3DCore/QEntity>

#include "generatepanel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setScene(const Scene *objects);

private:
    Ui::MainWindow *ui;
    GeneratePanel *generatePanel = nullptr;
    Qt3DCore::QEntity *rootEntity;

    void swapGeneratePanel(GeneratePanel * newPanel);
};

#endif // MAINWINDOW_H
