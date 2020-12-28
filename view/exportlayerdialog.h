#ifndef EXPORTLAYERDIALOG_H
#define EXPORTLAYERDIALOG_H

#include <QDialog>

namespace Ui {
class ExportLayerDialog;
}

class ExportLayerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportLayerDialog(QWidget *parent = nullptr);
    ~ExportLayerDialog();

private:
    Ui::ExportLayerDialog *ui;
    int visibleLayersCount;
    QList<int> visibleLayersIDs;

private slots:
    void selectAll();
    void selectDirectory();
    void exportTitles();

};

#endif // EXPORTLAYERDIALOG_H
