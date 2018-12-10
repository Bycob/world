#ifndef DIALOGSAVENODE_H
#define DIALOGSAVENODE_H

#include <QDialog>

namespace Ui {
class DialogSaveNode;
}

class DialogSaveNode : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSaveNode(QWidget *parent = 0);
    ~DialogSaveNode();

private:
    Ui::DialogSaveNode *ui;
};

#endif // DIALOGSAVENODE_H
