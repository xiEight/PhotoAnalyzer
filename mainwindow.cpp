#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    area = new QScrollArea();
    area->setBackgroundRole(QPalette::Dark);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_chooseImageButton_clicked()
{
    QString name = QFileDialog::getOpenFileName(this, "Open File");
    if (!name.isEmpty())
    {
        image = QImage(name);
        if (image.isNull())
        {
            QMessageBox errBox;
            errBox.setText("Cannot Load Image!");
            errBox.setWindowTitle("Error!");
            errBox.exec();
        }

        ui->imageLabel->setPixmap(QPixmap::fromImage(image));
        ui->imageLabel->resize(image.width(),image.height());
        area->setWidget(ui->imageLabel);
        area->setVisible(true);
        ui->verticalLayout->addWidget(area);
        area->show();
    }
}
