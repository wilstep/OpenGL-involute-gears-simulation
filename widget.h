#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <memory>
#include <QMessageBox>
#include <QTimer>
#include <QRadioButton>
#include <QtMath>
#include <QDesktopWidget>

class Scroller;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(Scroller *iparent = 0);
    ~Widget();

private slots:
    void on_quitButton_clicked();
    void keyPressEvent(QKeyEvent *event);
    void on_pausePlayButton_clicked();
    void drawOpenGL();
    void on_radioButton_14_clicked();
    void on_radioButton_20_clicked();
    void on_radioButton_25_clicked();
    void on_spinBox_Na_editingFinished();
    void on_spinBox_Nb_editingFinished();
    void on_speedScrollBar_valueChanged(int value);
    void on_lightPosX_editingFinished();
    void on_lightPosY_editingFinished();
    void on_lightPosZ_editingFinished();
    void on_resetButton_clicked();
    void on_SeperationSpinBox_valueChanged(double x);
    void on_aboutButton_clicked();
    void on_instructionsButton_clicked();
    void on_toggleButton_clicked();
    void on_fullScreenButton_clicked();
    void standardScreen();

private:
    Ui::Widget *ui;
    std::unique_ptr<QTimer> timer;
    bool bPause = false, bFullScreen = false;
    float pa = 20.0f * M_PI / 180.0f;
    unsigned int Na, Nb;
    bool rebuildGears = false, Nchange = false;
    bool bExact = true;
    int wMem, hMem; // remember parameters for exiting full screen
    int wMax, hMax; // screen size
    Scroller *parent;
};

#endif // WIDGET_H
