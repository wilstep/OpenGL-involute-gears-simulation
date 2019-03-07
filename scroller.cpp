#include "scroller.h"

Scroller::Scroller() : QScrollArea()
{

}

void Scroller::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event->key());
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

void Scroller::show()
{
    QScrollArea::show();
    setFocus();
}

void Scroller::showNormal()
{
    QScrollArea::showNormal();
    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    emit fullScreenExited();
}

void Scroller::showFullScreen()
{
    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollArea::showFullScreen();
}

