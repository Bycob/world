#include "generatepanel.h"

GeneratePanel::GeneratePanel(QWidget *parent) : QWidget(parent)
{

}

const Scene * GeneratePanel::getMeshes()
{
    return this->myScene.get();
}
