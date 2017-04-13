#ifndef PANELWORLDMAP_H
#define PANELWORLDMAP_H

#include <QWidget>

#include "generatepanel.h"

namespace Ui {
class PanelWorldMap;
}

class PanelWorldMap : public GeneratePanel
{
    Q_OBJECT

public:
    explicit PanelWorldMap(QWidget *parent = 0);
    ~PanelWorldMap();

public slots:
    virtual void generate();
private:
    Ui::PanelWorldMap *ui;
};

#endif // PANELWORLDMAP_H
