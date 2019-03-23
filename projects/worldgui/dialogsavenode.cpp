#include "dialogsavenode.h"
#include "ui_dialogsavenode.h"

DialogSaveNode::DialogSaveNode(QWidget *parent)
        : QDialog(parent), ui(new Ui::DialogSaveNode) {
    ui->setupUi(this);
}

DialogSaveNode::~DialogSaveNode() { delete ui; }
