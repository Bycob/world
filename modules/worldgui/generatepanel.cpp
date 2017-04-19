#include "generatepanel.h"

GeneratePanel::GeneratePanel(QWidget *parent) : QWidget(parent)
{

}

const Scene * GeneratePanel::getMeshes()
{
    return this->_myScene.get();
}

std::vector<std::unique_ptr<Resource>> GeneratePanel::getResources() {
    return std::vector<std::unique_ptr<Resource>>();
}
