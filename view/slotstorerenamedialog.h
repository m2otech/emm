#ifndef SLOTSTORERENAMEDIALOG_H
#define SLOTSTORERENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class SlotStoreRenameDialog;
}

class SlotStoreRenameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SlotStoreRenameDialog(QWidget *parent = 0);
    ~SlotStoreRenameDialog();
    QString getReplaceWhat();
    QString getReplaceWith();
    bool getReplaceSlotsCheckBox();

private:
    Ui::SlotStoreRenameDialog *ui;
};

#endif // SLOTSTORERENAMEDIALOG_H
