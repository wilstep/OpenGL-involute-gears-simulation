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
    int hValue, vValue;

signals:
    void fullScreenExited();
    void keyPressed(int key);
};

#endif // SCROLLER_H
