// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3

#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    ui->myOGLWidget->setLightX((float) ui->lightPosX->value());
    ui->myOGLWidget->setLightY((float) ui->lightPosY->value());
    ui->myOGLWidget->setLightZ((float) ui->lightPosZ->value()); 
    float speed = (float) ui->speedScrollBar->value() * 0.25f;
    ui->myOGLWidget->setSpeed(speed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_quitButton_clicked()
{
    close();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape) close();
}

void MainWindow::on_resetButton_clicked()
{
    ui->myOGLWidget->reset();
}

void MainWindow::on_pausePlayButton_clicked()
{  
    if(pause && rebuildGears){
        Na = ui->spinBox_Na->value();
        ui->myOGLWidget->setNa(Na);
        Nb = ui->spinBox_Nb->value();
        ui->myOGLWidget->setNb(Nb);
        if(Nchange) ui->myOGLWidget->reZeroThetas();
        ui->pausePlayButton->setText("Play");
        ui->myOGLWidget->rebuild();
        ui->myOGLWidget->update();
        rebuildGears = false;
        Nchange = false;
    }
    else if(pause){ // activate simulation
        ui->pausePlayButton->setText("Pause");
        ui->myOGLWidget->setPaused(false);
        ui->radioButton_14->setEnabled(false);
        ui->radioButton_20->setEnabled(false);
        ui->radioButton_25->setEnabled(false);
        ui->spinBox_Na->setEnabled(false);
        ui->spinBox_Nb->setEnabled(false);
        pause = !pause;
    }
    else{ // pause simulation
        ui->pausePlayButton->setText("Play");
        ui->myOGLWidget->setPaused(true);
        ui->radioButton_14->setEnabled(true);
        ui->radioButton_20->setEnabled(true);
        ui->radioButton_25->setEnabled(true);
        ui->spinBox_Na->setEnabled(true);
        ui->spinBox_Nb->setEnabled(true);
        pause = !pause;
    }
}

void MainWindow::on_radioButton_14_clicked()
{
    pa = 14.5f * M_PI / 180.0f;
    ui->myOGLWidget->setPa(pa);
    rebuildGears = true;
    ui->pausePlayButton->setText("Rebuild");
}

void MainWindow::on_radioButton_20_clicked()
{
    pa = 20.0f * M_PI / 180.0f;
    ui->myOGLWidget->setPa(pa);
    rebuildGears = true;
    ui->pausePlayButton->setText("Rebuild");
}

void MainWindow::on_radioButton_25_clicked()
{
    pa = 25.0f * M_PI / 180.0f;
    ui->myOGLWidget->setPa(pa);
    rebuildGears = true;
    ui->pausePlayButton->setText("Rebuild");
}

void MainWindow::on_spinBox_Na_editingFinished()
{
    unsigned int N = ui->spinBox_Na->value();
    if(N != Na){
        rebuildGears = true;
        Nchange = true;
    }
}

void MainWindow::on_spinBox_Nb_editingFinished()
{
    unsigned int N = ui->spinBox_Nb->value();
    if(N != Nb){
        rebuildGears = true;
        Nchange = true;
    }
}

void MainWindow::on_spinBox_Na_valueChanged(int)
{
    ui->pausePlayButton->setText("Rebuild");
}

void MainWindow::on_spinBox_Nb_valueChanged(int)
{
    ui->pausePlayButton->setText("Rebuild");
}

void MainWindow::drawOpenGL()
{
    if(!pause){
        ui->myOGLWidget->update();
        ui->myOGLWidget->incRotate();
    }
}

void MainWindow::on_speedScrollBar_valueChanged(int value)
{
    float speed = (float) value * 0.25f;
    ui->myOGLWidget->setSpeed(speed);
}

void MainWindow::on_lightPosX_editingFinished()
{
    float x = (float) ui->lightPosX->value();
    ui->myOGLWidget->setLightX(x);
}

void MainWindow::on_lightPosY_editingFinished()
{
    float y = (float) ui->lightPosY->value();
    ui->myOGLWidget->setLightY(y);
}

void MainWindow::on_lightPosZ_editingFinished()
{
    float z = (float) ui->lightPosZ->value();
    ui->myOGLWidget->setLightZ(z);
}

void MainWindow::on_SeperationSpinBox_valueChanged(double x)
{
    ui->myOGLWidget->setSeperation((float) x);
    if(pause) ui->myOGLWidget->update();
}

void MainWindow::on_pushButton_clicked()
{
    //QMessageBox::about(0, "Trolltech", "<a href='http://www.trolltech.com'>Trolltech</a>");
    QMessageBox msgBox;
    msgBox.setWindowTitle("Involute Gear Simulator");

    static const char *msg = "This program was made and developed by Stephen R. Williams\n"
        "<br>Feb 2019"
        "<br><br>See <a href='https://swtinkering.blogspot.com/2019/02/involute-gear-simulator-built-with-qt.html'>blog</a> entry for more details"
        "<br><br>License GPL-3.0";
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(msg);
    msgBox.exec();
}
