#include "widget.h"
#include "ui_widget.h"
#include "scroller.h"

Widget::Widget(Scroller *iparent) :
    QWidget(iparent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    parent = iparent;
    timer = std::make_unique<QTimer>(this);
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(drawOpenGL()));
    connect(parent, SIGNAL(fullScreenExited()), this, SLOT(standardScreen()));
    connect(parent, SIGNAL(keyPressed(int)), this, SLOT(keySwitcher(int)));
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
    ui->toggleLabel->setAlignment(Qt::AlignHCenter);
    ui->toggleLabel->setTextFormat(Qt::RichText);
    ui->toggleLabel->setText("<span style='font-size:10.5pt; font-weight:600;'>Exact Involute</span>");
    ui->toggleButton->setEnabled(false);
    wMax = QDesktopWidget().screenGeometry().size().width(); // get and store screen size
    hMax = QDesktopWidget().screenGeometry().size().height();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::speedChange(int ds)
{
    int speed = ui->speedScrollBar->value() + ds;
    ui->speedScrollBar->setValue(speed);
}

void Widget::on_quitButton_clicked()
{
    parent->close();
}

// switch method to handle KeyPressEvent
// also called by scroller oject's KeyPressEvent via key & slot
void Widget::keySwitcher(int key)
{
    switch(key)
    {
    case Qt::Key_Escape:
        if(bFullScreen) parent->showNormal();
        else parent->close();
        break;
    case Qt::Key_A:
        on_aboutButton_clicked();
        break;
    case Qt::Key_F:
        if(bFullScreen) parent->showNormal();
        else on_fullScreenButton_clicked();
        break;
    case Qt::Key_I:
        on_instructionsButton_clicked();
        break;
    case Qt::Key_P:
    case Qt::Key_Pause:
        on_pausePlayButton_clicked();
        break;
    case Qt::Key_Q:
        parent->close();
        break;
    case Qt::Key_R:
        on_resetButton_clicked();
        break;
    case Qt::Key_T:
        on_toggleButton_clicked();
        break;
    case Qt::Key_Plus:
    case Qt::Key_Right:
        speedChange(1);
        break;
    case Qt::Key_Minus:
    case Qt::Key_Left:
        speedChange(-1);
        break;
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    keySwitcher(event->key());
}

void Widget::on_resetButton_clicked()
{
    ui->myOGLWidget->reset();
}

void Widget::on_pausePlayButton_clicked()
{
    if(bPause && rebuildGears){
        Na = ui->spinBox_Na->value();
        ui->myOGLWidget->setNa(Na);
        float speed = (float) ui->speedScrollBar->value() * 0.25f;
        ui->myOGLWidget->setSpeed(speed);
        Nb = ui->spinBox_Nb->value();
        ui->myOGLWidget->setNb(Nb);
        if(Nchange) ui->myOGLWidget->reZeroThetas();
        ui->pausePlayButton->setText("&Play");
        ui->myOGLWidget->rebuild();
        ui->myOGLWidget->update();
        rebuildGears = false;
        Nchange = false;
    }
    else if(bPause){ // activate simulation
        ui->pausePlayButton->setText("&Pause");
        ui->myOGLWidget->setPaused(false);
        ui->radioButton_14->setEnabled(false);
        ui->radioButton_20->setEnabled(false);
        ui->radioButton_25->setEnabled(false);
        ui->spinBox_Na->setEnabled(false);
        ui->spinBox_Nb->setEnabled(false);
        ui->toggleButton->setEnabled(false);
        bPause = false;
    }
    else{ // pause simulation
        ui->pausePlayButton->setText("&Play");
        ui->myOGLWidget->setPaused(true);
        ui->radioButton_14->setEnabled(true);
        ui->radioButton_20->setEnabled(true);
        ui->radioButton_25->setEnabled(true);
        ui->spinBox_Na->setEnabled(true);
        ui->spinBox_Nb->setEnabled(true);
        ui->toggleButton->setEnabled(true);
        bPause = true;
    }
}

void Widget::on_radioButton_14_clicked()
{
    pa = 14.5f * M_PI / 180.0f;
    ui->myOGLWidget->setPa(pa);
    rebuildGears = true;
    ui->pausePlayButton->setText("Rebuild");
}

void Widget::on_radioButton_20_clicked()
{
    pa = 20.0f * M_PI / 180.0f;
    ui->myOGLWidget->setPa(pa);
    rebuildGears = true;
    ui->pausePlayButton->setText("Rebuild");
}

void Widget::on_radioButton_25_clicked()
{
    pa = 25.0f * M_PI / 180.0f;
    ui->myOGLWidget->setPa(pa);
    rebuildGears = true;
    ui->pausePlayButton->setText("Rebuild");
}

void Widget::on_spinBox_Na_editingFinished()
{
    unsigned int N = ui->spinBox_Na->value();
    if(N != Na){
        rebuildGears = true;
        Nchange = true;
        ui->pausePlayButton->setText("Rebuild");
    }
}

void Widget::on_spinBox_Nb_editingFinished()
{
    unsigned int N = ui->spinBox_Nb->value();
    if(N != Nb){
        rebuildGears = true;
        Nchange = true;
        ui->pausePlayButton->setText("Rebuild");
    }
}


void Widget::drawOpenGL()
{
    if(!bPause){
        ui->myOGLWidget->update();
        ui->myOGLWidget->incRotate();
    }
}

void Widget::on_speedScrollBar_valueChanged(int value)
{
    float speed = (float) value * 0.25f;
    ui->myOGLWidget->setSpeed(speed);
}

void Widget::on_lightPosX_editingFinished()
{
    float x = (float) ui->lightPosX->value();
    ui->myOGLWidget->setLightX(x);
}

void Widget::on_lightPosY_editingFinished()
{
    float y = (float) ui->lightPosY->value();
    ui->myOGLWidget->setLightY(y);
}

void Widget::on_lightPosZ_editingFinished()
{
    float z = (float) ui->lightPosZ->value();
    ui->myOGLWidget->setLightZ(z);
}

void Widget::on_SeperationSpinBox_valueChanged(double x)
{
    ui->myOGLWidget->setSeperation((float) x);
    if(bPause) ui->myOGLWidget->update();
}

void Widget::on_aboutButton_clicked()
{
    //QMessageBox::about(0, "Trolltech", "<a href='http://www.trolltech.com'>Trolltech</a>");
    QMessageBox msgBox;
    msgBox.setWindowTitle("Involute Gear Simulator");

    std::string msg = "This software was made and developed by Stephen R. Williams"
        "<br>March 2019, Version 1.3.2"
        "<br><br>See <a href='https://swtinkering.blogspot.com/2019/02/involute-gear-simulator-built-with-qt.html'>blog</a> entry for more details"
        "<br><br>License GPL-3.0<br><br>";
    msg += ui->myOGLWidget->getOGLVersionInfo();
    msg += "<br>";
    msg += ui->myOGLWidget->getShaderVersionInfo();
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(msg.c_str());
    msgBox.exec();
}

void Widget::on_instructionsButton_clicked()
{
    //QMessageBox::about(0, "Trolltech", "<a href='http://www.trolltech.com'>Trolltech</a>");
    QMessageBox msgBox;
    msgBox.setWindowTitle("Mouse Instructions");

    std::string msg = "<html><head/><body><p align='center'><span style=' font-size:12pt; font-weight:600;'>"
                      "Use mouse to reorientate</span></p><p><span style=' font-size:10pt; color:#00557f;'>"
                      "• Left button to roll<br/>"
                      "• Right button to rotate<br/>"
                      "• Shift &amp; left button to translate<br/>"
                      "• Shift &amp; right button to zoom</span></p></body></html>";
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(msg.c_str());
    msgBox.exec();
}

void Widget::on_toggleButton_clicked()
{
    if(bPause){
        rebuildGears = true;
        ui->pausePlayButton->setText("Rebuild");

        if(bExact){
            ui->toggleLabel->setText("<span style='font-size:10.5pt; font-weight:600;'>Circle Approximation</span>");
            bExact = false;
        }
        else{
            ui->toggleLabel->setText("<span style='font-size:10.5pt; font-weight:600;'>Exact Involute</span>");
            bExact = true;
        }
        ui->myOGLWidget->setBExact(bExact);
        rebuildGears = true;
        ui->pausePlayButton->setText("Rebuild");
    }
}

void Widget::on_fullScreenButton_clicked()
{
    if(bFullScreen){
        parent->showNormal();
        return;
    }
    parent->storeSliderPositions();
    wMem = width();
    hMem = height();
    ui->aboutButton->hide();
    ui->instructionsButton->hide();
    ui->pausePlayButton->hide();
    ui->toggleButton->hide();
    ui->fullScreenButton->hide();
    ui->resetButton->hide();
    ui->quitButton->hide();
    ui->speedScrollBar->hide();
    ui->SeperationSpinBox->hide();
    ui->radioButton_14->hide();
    ui->radioButton_20->hide();
    ui->radioButton_25->hide();
    ui->lightPosX->hide();
    ui->lightPosY->hide();
    ui->lightPosZ->hide();
    ui->spinBox_Na->hide();
    ui->spinBox_Nb->hide();
    ui->label->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->label_5->hide();
    ui->label_6->hide();
    ui->label_7->hide();
    ui->label_8->hide();
    ui->label_10->hide();
    ui->label_11->hide();
    ui->toggleLabel->hide();
    //vScrollBar->hide();
    auto w = QDesktopWidget().screenGeometry().size().width();
    auto h = QDesktopWidget().screenGeometry().size().height();
    ui->myOGLWidget->move(0, 0);
    ui->myOGLWidget->setPerspective((float) w / (float) h);
    parent-> showFullScreen();
    resize(w, h);
    ui->myOGLWidget->resize(w, h);
    bFullScreen = true;
    ui->myOGLWidget->update();
}

void Widget::standardScreen()
{
    ui->aboutButton->show();
    ui->instructionsButton->show();
    ui->pausePlayButton->show();
    ui->toggleButton->show();
    ui->fullScreenButton->show();
    ui->resetButton->show();
    ui->quitButton->show();
    ui->speedScrollBar->show();
    ui->SeperationSpinBox->show();
    ui->radioButton_14->show();
    ui->radioButton_20->show();
    ui->radioButton_25->show();
    ui->lightPosX->show();
    ui->lightPosY->show();
    ui->lightPosZ->show();
    ui->spinBox_Na->show();
    ui->spinBox_Nb->show();
    ui->label->show();
    ui->label_2->show();
    ui->label_3->show();
    ui->label_4->show();
    ui->label_5->show();
    ui->label_6->show();
    ui->label_7->show();
    ui->label_8->show();
    ui->label_10->show();
    ui->label_11->show();
    ui->toggleLabel->show();
    ui->myOGLWidget->resize(1200, 900);
    ui->myOGLWidget->move(180, 10);
    ui->myOGLWidget->setPerspective(4.0f / 3.0f);
    resize(wMem, hMem);
    bFullScreen = false;
    parent->setSliderPositions();
    ui->myOGLWidget->update();
}
