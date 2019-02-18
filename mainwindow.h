// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <memory>
#include <QMainWindow>
#include <QTimer>
#include <QRadioButton>
#include <QtMath>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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
    void on_horizontalScrollBar_valueChanged(int value);

    void on_lightPosX_editingFinished();

    void on_lightPosY_editingFinished();

    void on_lightPosZ_editingFinished();

    void on_resetButton_clicked();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<QTimer> timer;
    bool pause = false;
    float pa = 20.0f * M_PI / 180.0f;
    unsigned int Na, Nb;
    bool rebuildGears = false;
};

#endif // MAINWINDOW_H
