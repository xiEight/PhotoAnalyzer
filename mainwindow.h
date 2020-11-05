#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <gost.h>

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

private:
    Ui::MainWindow *ui;
    QScrollArea *area;
    QImage image;
    QByteArray buffer;
    gost crypter;
};

#endif // MAINWINDOW_H
