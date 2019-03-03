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
}

void Scroller::setSliderPositions()
{
    //horizontalScrollBar()->setMaximum(hValue);
    //verticalScrollBar()->setMaximum(vValue);
    //horizontalScrollBar()->setSliderPosition(hValue);
    //verticalScrollBar()->setSliderPosition(vValue);
    scrollContentsBy(hValue, vValue);

    std::cout << "hValue = " << hValue << ", vValue = " << vValue;
    std::cout << ", h = " << horizontalScrollBar()->sliderPosition();
    std::cout << ", v = " << verticalScrollBar()->sliderPosition();
    std::cout << ", vmax = " << verticalScrollBar()->maximum();
    std::cout << std::endl;
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

