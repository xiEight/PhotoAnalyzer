#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include "gost.h"
#include "client.h"
#include <fstream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_chooseImageButton_clicked();

    void on_connectButton_clicked();

    void on_sendBtn_clicked();

private:
    Ui::MainWindow *ui;
    QScrollArea *area;
    QImage image;
    QByteArray buffer;
    gost *crypter = nullptr;
    Client *clientSocket = nullptr;
    QString fileName;
    std::mutex fileEncryptingLock;
    std::condition_variable cv;
    bool fileOpComplete = false;
    void showError(QString const &errName, QString const &windowTitle);
};

#endif // MAINWINDOW_H
