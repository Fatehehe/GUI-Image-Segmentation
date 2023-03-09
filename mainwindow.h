#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <QMainWindow>
#include <QWidget>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <QImage>
#include <QPixmap>
#include <QSlider>
#include <QSpinBox>
#include <QDebug>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <QDebug>
#include <iostream>
#include "colorsegmentation.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openImage();

protected:
    void closeEvent(QCloseEvent *event);


private slots:
    void on_startBtn_pressed();

    void on_saveButton_clicked();

    void on_loadDataButton_clicked();

    void on_imageLoadButton_clicked();

private:
    Ui::MainWindow *ui;

    QGraphicsPixmapItem pixmap;
    cv::VideoCapture video;
    cv::Mat frame;
    cv::Mat img;
};
#endif // MAINWINDOW_H
