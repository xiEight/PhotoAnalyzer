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
    delete area;
    delete ui;
}

void MainWindow::on_chooseImageButton_clicked()
{
    QString name = QFileDialog::getOpenFileName(this, "Open File");
    if (!name.isEmpty())
    {
        image = QImage(name);
        if (image.isNull())
            showError("Cannot Load Image", "Load Error");


        ui->imageLabel->setPixmap(QPixmap::fromImage(image));
        ui->imageLabel->resize(image.width(),image.height());
        area->setWidget(ui->imageLabel);
        area->setVisible(true);
        ui->verticalLayout->addWidget(area);
        area->show();
    }
    fileName = name;
}

void MainWindow::on_connectButton_clicked()
{
    try
    {
        clientSocket = new Client(ui->IPedit->text().toStdString(), ui->portEdit->text().toUShort());
        ui->statusLabel->setText("Status: connected");
    }
    catch (std::runtime_error const &err)
    {
        showError(err.what(),"Connection error");
    }
}

void MainWindow::on_sendBtn_clicked()
{
    QString saveFileName = fileName;
    saveFileName.insert(saveFileName.lastIndexOf("/") + 1,"D");
    crypter = new gost();
    crypter->set_key("key");
    crypter->start(fileName.toStdString(), saveFileName.toStdString(),1);

}

void MainWindow::showError(QString const & errName, QString const &windowTitle)
{
    QMessageBox box;
    box.setWindowTitle(windowTitle);
    box.setText(errName);
    box.exec();
}
