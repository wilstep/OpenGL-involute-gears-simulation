#include "scroller.h"
#include <iostream>

Scroller::Scroller() : QScrollArea()
{

}

void Scroller::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        if(bFullScreen) showNormal();
        else close();
    }
    if(event->key() == Qt::Key_Pause){
        std::cout << "Pause Button pressed\n";
        emit pauseButtonPressed();
    }
}

void Scroller::setSliderPositions()
{
    horizontalScrollBar()->setValue(hValue);
    verticalScrollBar()->setValue(vValue);
}

void Scroller::storeSliderPositions()
{
    hValue = horizontalScrollBar()->sliderPosition();
    vValue = verticalScrollBar()->sliderPosition();
}

void Scroller::showNormal()
{
    QScrollArea::showNormal();
    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    bFullScreen = false;
    emit fullScreenExited();
}

void Scroller::showFullScreen()
{
    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollArea::showFullScreen();
    bFullScreen = true;
}

