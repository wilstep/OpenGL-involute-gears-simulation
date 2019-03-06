#ifndef SCROLLER_H
#define SCROLLER_H

#include <QWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QKeyEvent>

class Scroller : public QScrollArea
{
    Q_OBJECT

public:
    Scroller();  
    void setSliderPositions();
    void storeSliderPositions();
    void show();

public slots:
    void showFullScreen();
    void showNormal();

private slots:
    void keyPressEvent(QKeyEvent *event);

private:
    bool bFullScreen = false;

    int hValue, vValue;

signals:
    void fullScreenExited();
    void goFullSceen();
    void instructionButton();
    void pauseButton();
    void speedChange(int);
    void resetButton();
    void aboutButton();
    void toggleButton();
};

#endif // SCROLLER_H
