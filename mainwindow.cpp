// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) //, ui(new Ui::MainWindow)
{
    ui = new Ui::MainWindow();
    ui->setupUi(this);
    timer = std::make_unique<QTimer>(this);
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(drawOpenGL()));
    timer->start(25); // delay in milli seconds
    ui->radioButton_14->setEnabled(false);
    ui->radioButton_20->setEnabled(false);
    ui->radioButton_25->setEnabled(false);
    ui->spinBox_Na->setEnabled(false);
    ui->spinBox_Nb->setEnabled(false);
    Na = ui->spinBox_Na->value();
    Nb = ui->spinBox_Nb->value();
    ui->myOGLWidget->setNa(Na);
    ui->myOGLWidget->setNb(Nb);
    ui->myOGLWidget->setPa(pa);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_quitButton_clicked()
{
    close();
}

void MainWindow::on_pausePlayButton_clicked()
{
    if(pause){ // activate simulation
        ui->pausePlayButton->setText("Pause");
        ui->myOGLWidget->setPaused(false);
        ui->radioButton_14->setEnabled(false);
        ui->radioButton_20->setEnabled(false);
        ui->radioButton_25->setEnabled(false);
        ui->spinBox_Na->setEnabled(false);
        ui->spinBox_Nb->setEnabled(false);
        if(rebuildGears){
            ui->myOGLWidget->rebuild();
            std::cout << "Rebuilding!\n";
        }
        rebuildGears = false;
    }
    else{ // pause simulation
        ui->pausePlayButton->setText("Play");
        ui->myOGLWidget->setPaused(true);
        ui->radioButton_14->setEnabled(true);
        ui->radioButton_20->setEnabled(true);
        ui->radioButton_25->setEnabled(true);
        ui->spinBox_Na->setEnabled(true);
        ui->spinBox_Nb->setEnabled(true);
    }
    pause = !pause;
}

void MainWindow::on_radioButton_14_clicked()
{
    pa = 14.5f * M_PI / 180.0f;
    ui->myOGLWidget->setPa(pa);
    rebuildGears = true;
    std::cout << "Pressure angle 14.5 degrees\n";
}

void MainWindow::on_radioButton_20_clicked()
{
    pa = 20.0f * M_PI / 180.0f;
    ui->myOGLWidget->setPa(pa);
    rebuildGears = true;
    std::cout << "Pressure angle 20 degrees\n";
}

void MainWindow::on_radioButton_25_clicked()
{
    pa = 25.0f * M_PI / 180.0f;
    ui->myOGLWidget->setPa(pa);
    rebuildGears = true;
    std::cout << "Pressure angle 25 degrees\n";
}

void MainWindow::on_spinBox_Na_editingFinished()
{
    unsigned int N = ui->spinBox_Na->value();
    if(N != Na){
        Na = N;
        ui->myOGLWidget->setNa(Na);
        rebuildGears = true;
        std::cout << "Na = " << Na << std::endl;
    }
}

void MainWindow::on_spinBox_Nb_editingFinished()
{
    unsigned int N = ui->spinBox_Nb->value();
    if(N != Nb){
        Nb = N;
        ui->myOGLWidget->setNb(Nb);
        rebuildGears = true;
        std::cout << "Nb = " << Nb << std::endl;
    }
}

void MainWindow::drawOpenGL()
{
    if(!pause){
        ui->myOGLWidget->update();
        ui->myOGLWidget->incRotate();
    }
}


