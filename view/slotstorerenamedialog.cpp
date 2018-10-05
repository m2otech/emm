#include "slotstorerenamedialog.h"
#include "ui_slotstorerenamedialog.h"

SlotStoreRenameDialog::SlotStoreRenameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SlotStoreRenameDialog)
{
    ui->setupUi(this);
}

SlotStoreRenameDialog::~SlotStoreRenameDialog()
{
    delete ui;
}

QString SlotStoreRenameDialog::getReplaceWhat()
{
    return ui->replaceWhatField->text();
}

QString SlotStoreRenameDialog::getReplaceWith()
{
    return ui->replaceWithField->text();
}

bool SlotStoreRenameDialog::getReplaceSlotsCheckBox()
{
    return ui->replaceSlotsCheckBox->isChecked();
}
