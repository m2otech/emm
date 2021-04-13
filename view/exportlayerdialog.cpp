#include <QFileDialog>
#include <QProgressDialog>

#include "exportlayerdialog.h"
#include "ui_exportlayerdialog.h"

#include "model/configuration.h"
#include "model/layerdata.h"
#include "view/mainwindow.h"

#include "model/exporttitlesthread.h"


ExportLayerDialog::ExportLayerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportLayerDialog)
{
    ui->setupUi(this);

    // Hide help icon from title bar
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Populate layer list
    Configuration *config = Configuration::getInstance();
    int layerCount = config->getLayers().count();
    this->visibleLayersCount = 0;
    for (int i = 0; i < layerCount; i++)
    {
        LayerData *data = config->getLayers().value(i);
        if (data->getVisible())
        {
            ui->layerSelection->addItem(data->getName());
            this->visibleLayersIDs.append(data->getLayerID());
            this->visibleLayersCount++;
        }
    }

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(exportTitles()));
    connect(ui->exportAllButton, SIGNAL(clicked()), this, SLOT(selectAll()));
    connect(ui->selectDirectoryButton, SIGNAL(clicked()), this, SLOT(selectDirectory()));

    ui->buttonBox->setEnabled(false);

    ui->exportDirectory->setReadOnly(true);

}

// Select all layers in the list
void ExportLayerDialog::selectAll()
{
    ui->layerSelection->selectAll();
}

// Select export directory
void ExportLayerDialog::selectDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Export-Verzeichnis auswählen"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    ui->exportDirectory->setText(dir);
    if (!dir.isEmpty())
        ui->buttonBox->setEnabled(true);
}

void ExportLayerDialog::exportTitles()
{
    QList<int> layers;

    for (int i = 0; i < this->visibleLayersCount; i++)
    {
         if (ui->layerSelection->item(i)->isSelected())
            layers.append(this->visibleLayersIDs.at(i));
    }

    if (layers.isEmpty())
        return;

    if (ui->exportDirectory->text().isEmpty())
        return;

    ExportTitlesThread *exportThread;

    exportThread = new ExportTitlesThread(layers, ui->exportDirectory->text());

    QProgressDialog *dia = new QProgressDialog(this);
    // TODO Add CANCEL button and make it work
    //dia->setCancelButton(new QPushButton(this));
    //dia->setCancelButtonText("ABBRECHEN");
    dia->setCancelButton(NULL);
    dia->setLabelText(tr("Titel werden exportiert...."));
    dia->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    connect(exportThread, SIGNAL(updateStatus(int)), dia, SLOT(setValue(int)));
    connect(exportThread, SIGNAL(updateMax(int)), dia, SLOT(setMaximum(int)));
    connect(exportThread, SIGNAL(updateMin(int)), dia, SLOT(setMinimum(int)));
    connect(dia, SIGNAL(canceled()), exportThread, SLOT(quit()));
    exportThread->start();
    dia->exec();
}

ExportLayerDialog::~ExportLayerDialog()
{
    delete ui;
}
