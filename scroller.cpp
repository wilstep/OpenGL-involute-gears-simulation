#include "scroller.h"

Scroller::Scroller() : QScrollArea()
{

}

void Scroller::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        if(bFullScreen) showNormal();
        else close();
        break;
    case Qt::Key_A:
        emit aboutButton();
        break;
    case Qt::Key_F:
        if(bFullScreen) showNormal();
        else emit goFullSceen();
        break;
    case Qt::Key_I:
        emit instructionButton();
    case Qt::Key_P:
    case Qt::Key_Pause:
        emit pauseButton();
        break;
    case Qt::Key_Q:
        close();
        break;
    case Qt::Key_R:
        emit resetButton();
        break;
    case Qt::Key_T:
        emit toggleButton();
        break;
    case Qt::Key_Plus:
    case Qt::Key_Up:
    case Qt::Key_Right:
        emit speedChange(1);
        break;
    case Qt::Key_Minus:
    case Qt::Key_Down:
    case Qt::Key_Left:
        emit speedChange(-1);
        break;
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

