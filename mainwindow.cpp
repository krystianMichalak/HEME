#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->move(10, 20);
    ui->setupUi(this);
    loadSettings();
}

void MainWindow::loadSettings()
{
    QSettings settings("HEME","Image", this);
    ui->inputLineEdit->setText(settings.value("inputLineEdit", "").toString());
    ui->gaussianBlurRadioButton->setChecked(settings.value("gausianBlurRadioButton", true).toBool());
    ui->medianBlurRadioButton->setChecked(settings.value("medianBlurRadioButton", false).toBool());
    ui->otherRadioButton->setChecked(settings.value("otherRadioButton", false).toBool());

    QStringList otherMethods= {tr("Threshold"), tr("Other")};

    ui->otherComboBox->addItems(otherMethods);
    ui->displayImageCheckBox->setChecked(settings.value("displayImageCheckBox", true).toBool());
    ui->outputLineEdit->setText(settings.value("outputLineEdit", "").toString());
}

void MainWindow::saveSettings()
{
    QSettings settings("HEME", "Image", this);
    settings.setValue("inputLineEdit", ui->inputLineEdit->text());
    settings.setValue("gausianBlurRadioButton", ui->gaussianBlurRadioButton->isChecked());
    settings.setValue("medianBlurRadioButton", ui->medianBlurRadioButton->isChecked());
    settings.setValue("otherRadioButton", ui->otherRadioButton->isChecked());
    settings.setValue("displayImageCheckBox", ui->displayImageCheckBox->isChecked());
    settings.setValue("outputLineEdit", ui->outputLineEdit->text());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    int result = QMessageBox::warning(
                this, tr("Exit"), tr("Are you want to close this program?"),QMessageBox::Yes, QMessageBox::No);
    if(result == QMessageBox::Yes)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::on_inputPushButton_pressed()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("Open Input Image"), QDir::currentPath(),"Images (*.jpg *.png *.bmp)");
    if(QFile::exists(fileName))
    {
        ui->inputLineEdit->setText(fileName);

        using namespace cv;
        Mat inpImg = imread(fileName.toStdString());

        namedWindow("Input Image",WINDOW_AUTOSIZE);
        imshow("Input Image",inpImg);
        moveWindow("Input Image", 1100, 20);
    }
}

void MainWindow::on_outputPushButton_pressed()
{
    QString fileName = QFileDialog::getSaveFileName(
                this, tr("Select Output Image"),QDir::currentPath(),"*.jpg;;*.png;;*.bmp");
    if(!fileName.isEmpty())
    {
        ui->outputLineEdit->setText(fileName);

        using namespace cv;
        Mat inpImg, outpImg;


        if(ui->otherRadioButton->isChecked())
        {
            inpImg = imread(ui->inputLineEdit->text().toStdString(), IMREAD_GRAYSCALE);
            threshold(inpImg,outpImg,120, 255, THRESH_BINARY);
        }
        else if(ui->medianBlurRadioButton->isChecked())
        {
            inpImg = imread(ui->inputLineEdit->text().toStdString());
            medianBlur(inpImg,outpImg,5);
        }
        else if(ui->gaussianBlurRadioButton->isChecked())
        {   inpImg = imread(ui->inputLineEdit->text().toStdString());
            GaussianBlur(inpImg,outpImg,Size(5,5),1.25);
        }
        else
            inpImg.copyTo(outpImg);
        imwrite(fileName.toStdString(),outpImg);
        if(ui->displayImageCheckBox->isChecked())
        {
            namedWindow("Output Image",WINDOW_AUTOSIZE);
            imshow("Output Image",outpImg);
            moveWindow("Output Image", 1100, 500);
        }
    }
}
