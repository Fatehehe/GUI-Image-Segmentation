#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //connect ui spinbox dan slider
    connect(ui->hminSlider, &QSlider::valueChanged,
            ui->hminSpinBox, &QSpinBox::setValue);

    connect(ui->hminSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int val){
        ui->hminSlider->setValue(val);
    });

    connect(ui->hmaxSlider, &QSlider::valueChanged,
            ui->hmaxSpinBox, &QSpinBox::setValue);

    connect(ui->hmaxSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int val){
        ui->hmaxSlider->setValue(val);
    });

    connect(ui->sminSlider, &QSlider::valueChanged,
            ui->sminSpinBox, &QSpinBox::setValue);

    connect(ui->sminSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int val){
        ui->sminSlider->setValue(val);
    });

    connect(ui->smaxSlider, &QSlider::valueChanged,
            ui->smaxSpinBox, &QSpinBox::setValue);

    connect(ui->smaxSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int val){
        ui->smaxSlider->setValue(val);
    });

    connect(ui->vminSlider, &QSlider::valueChanged,
            ui->vminSpinBox, &QSpinBox::setValue);

    connect(ui->vminSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int val){
        ui->vminSlider->setValue(val);
    });

    connect(ui->vmaxSlider, &QSlider::valueChanged,
            ui->vmaxSpinBox, &QSpinBox::setValue);

    connect(ui->vmaxSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int val){
        ui->vmaxSlider->setValue(val);
    });

    //set graphicsView
    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->scene()->addItem(&pixmap);

    ui->conditionLabel->setText("None");
}

MainWindow::~MainWindow()
{
    delete ui;
}

//buka gambar dari folder pake opencv terus dishow ke label
void MainWindow::openImage()
{
    cv::Mat img = cv::imread("/home/fatih/Downloads/bocchi.jpg");
    cv::cvtColor(img, img, cv::COLOR_BGR2HSV);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(video.isOpened())
    {
        QMessageBox::warning(this,
                             "Warning",
                             "Stop the video before closing the application!");
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::on_startBtn_pressed()
{
    using namespace cv;

        if(!img.empty()){
            QMessageBox::critical(this,
                                  "image still exist",
                                  "Release the loaded image!");
            return;
        }

        if(video.isOpened())
        {
            ui->startBtn->setText("Start");
            ui->imageOnlyCheckBox->setEnabled(true);
            ui->imgSegmentationCheckBox->setEnabled(true);
            video.release();
            std::cout<<video.isOpened()<<std::endl;
            return;
        }

        bool isCamera;
        int cameraIndex = ui->videoEdit->currentText().toInt(&isCamera);
        std::cout<<isCamera<<std::endl;
        if(isCamera)
        {
            if(!video.open(cameraIndex))
            {
                QMessageBox::critical(this,
                                      "Camera Error",
                                      "Make sure you entered a correct camera index,"
                                      "<br>or that the camera is not being accessed by another program!");
                return;
            }
        }
        else
        {
            if(!video.open(ui->videoEdit->currentText().trimmed().toStdString()))
            {
                QMessageBox::critical(this,
                                      "Video Error",
                                      "Make sure you entered a correct and supported video file path,"
                                      "<br>or a correct RTSP feed URL!");
                return;
            }
        }

        ui->startBtn->setText("Stop");

        while(video.isOpened())
        {
            video >> frame;
            ui->imageOnlyCheckBox->setEnabled(false);
            ui->imgSegmentationCheckBox->setEnabled(false);
            if(!frame.empty())
            {
                cv::Scalar lower(ui->hminSlider->value(),
                                 ui->sminSlider->value(),
                                 ui->vminSlider->value());

                cv::Scalar upper(ui->hmaxSlider->value(),
                                 ui->smaxSlider->value(),
                                 ui->vmaxSlider->value());

                cv::Mat hsv_frame, maskHSV, bitwise_frame;

                cv::cvtColor(frame, hsv_frame, cv::COLOR_BGR2HSV);
                inRange(hsv_frame, lower, upper, maskHSV);

                std::vector<std::vector<cv::Point>> contours;
                std::vector<cv::Vec4i> hierarchy;

                findContours(maskHSV, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                cv::Mat drawing = cv::Mat::zeros(maskHSV.size(), CV_8UC3);

                for (int i = 0; i < (int)contours.size(); i++)
                    {
                        cv::Scalar color_contours = cv::Scalar(0, 255, 0);
                        drawContours(drawing, contours, i, color_contours, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
                    }

                cv::fillPoly(drawing, contours, cv::Scalar(255, 255, 255));

                if(ui->segmentationCheckBox->isChecked() && ui->cameraOnlyCheckBox->isChecked())
                {
                    cv::bitwise_and(frame, drawing, bitwise_frame);

                    QImage qimg(bitwise_frame.data,
                                bitwise_frame.cols,
                                bitwise_frame.rows,
                                bitwise_frame.step,
                                QImage::Format_RGB888);

                    pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()) );
                }
                else if(ui->cameraOnlyCheckBox->isChecked())
                {
                    QImage qimg(frame.data,
                                frame.cols,
                                frame.rows,
                                frame.step,
                                QImage::Format_RGB888);

                    pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()) );

                    ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
                }
                else if(ui->segmentationCheckBox->isChecked())
                {
                    QImage qimg(drawing.data,
                                drawing.cols,
                                drawing.rows,
                                drawing.step,
                                QImage::Format_RGB888);

                    pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()) );

                    ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
                }
            }
            qApp->processEvents();
        }


        ui->startBtn->setText("Start");
}


void MainWindow::on_saveButton_clicked()
{
    //Save the file to disk
    QString filename = QFileDialog::getSaveFileName(this, "Save As");
    if( filename.isEmpty()){
        return;
    }

    QFile file(filename);

    //Open the file
    if( !file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
        return;
    }

    QTextStream  out(&file);

    nlohmann::json color_range;
    color_range["hue_min"] = ui->hminSlider->value();
    color_range["hue_max"] = ui->hmaxSlider->value();
    color_range["sat_min"] = ui->sminSlider->value();
    color_range["sat_max"] = ui->smaxSlider->value();
    color_range["val_min"] = ui->vminSlider->value();
    color_range["val_max"] = ui->vmaxSlider->value();

    std::string output_range = color_range.dump();

    out << QString::fromStdString(output_range);

    file.close();
}

void MainWindow::on_loadDataButton_clicked()
{
    std::fstream newfile;

    nlohmann::json file;

    QString filename = QFileDialog::getOpenFileName(this, "Choose File");
    if(filename.isEmpty()){
        return;
    }

    qDebug()<< filename ;

    newfile.open(filename.toStdString(), std::ios::in);

    std::string tp;
    std::getline(newfile, tp);

    qDebug() << QString::fromStdString(tp);

    file = nlohmann::json::parse(tp);

    ui->hminSlider->setValue(file.value("hue_min",0));
    ui->hmaxSlider->setValue(file.value("hue_max",0));
    ui->sminSlider->setValue(file.value("sat_min",0));
    ui->smaxSlider->setValue(file.value("sat_max",0));
    ui->vminSlider->setValue(file.value("val_min",0));
    ui->hmaxSlider->setValue(file.value("val_max",0));

}

void MainWindow::on_imageLoadButton_clicked()
{

    if(video.isOpened())
    {
        QMessageBox::warning(this,
                             "Warning",
                             "Stop the video before change the image!");
        return;
    }

    if(!img.empty()){
        ui->conditionLabel->setText("None");
        ui->imageLoadButton->setText("Open Image");
        ui->cameraOnlyCheckBox->setEnabled(true);
        ui->segmentationCheckBox->setEnabled(true);
//        std::cout<< " img: " << img.empty() << std::endl;
        img.release();
        return;
    }

    QString filename = QFileDialog::getOpenFileName(this, "Choose File");

    if(filename.isEmpty()){
        return;
    }

    ui->imageLoadButton->setText("Release Image");
    ui->conditionLabel->setText("Exist");

    img = cv::imread(filename.toStdString());

    while(ui->conditionLabel->text() == "Exist"){
        if(!img.empty()){
            ui->cameraOnlyCheckBox->setEnabled(false);
            ui->segmentationCheckBox->setEnabled(false);
            cv::Scalar lower(ui->hminSlider->value(),
                             ui->sminSlider->value(),
                             ui->vminSlider->value());

            cv::Scalar upper(ui->hmaxSlider->value(),
                             ui->smaxSlider->value(),
                             ui->vmaxSlider->value());

            cv::Mat hsv_img, maskHSV, bitwise_img;

            cv::cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);
            cv::inRange(hsv_img, lower, upper, maskHSV);

            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;

            cv::findContours(maskHSV, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            cv::Mat drawing = cv::Mat::zeros(maskHSV.size(), CV_8UC3);

            for (int i = 0; i < (int)contours.size(); i++)
                {
                    cv::Scalar color_contours = cv::Scalar(0, 255, 0);
                    drawContours(drawing, contours, i, color_contours, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
                }

            cv::fillPoly(drawing, contours, cv::Scalar(255, 255, 255));

            if(ui->imgSegmentationCheckBox->isChecked() && ui->imageOnlyCheckBox->isChecked())
            {
                cv::bitwise_and(img, drawing, bitwise_img);

                QImage qimg(bitwise_img.data,
                            bitwise_img.cols,
                            bitwise_img.rows,
                            bitwise_img.step,
                            QImage::Format_RGB888);

                pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()) );
            }
            else if(ui->imageOnlyCheckBox->isChecked())
            {
                QImage qimg(img.data,
                            img.cols,
                            img.rows,
                            img.step,
                            QImage::Format_RGB888);

                pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()) );

                ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
            }
            else if(ui->imgSegmentationCheckBox->isChecked())
            {
                QImage qimg(drawing.data,
                            drawing.cols,
                            drawing.rows,
                            drawing.step,
                            QImage::Format_RGB888);

                pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()) );

                ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
            }
        }
        qApp->processEvents();
    }
}

