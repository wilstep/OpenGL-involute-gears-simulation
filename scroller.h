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
};

#endif // SCROLLER_H
