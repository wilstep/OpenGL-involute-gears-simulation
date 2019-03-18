#include "widget.h"
#include "scroller.h"
#include <QApplication>
#include <QScrollArea>
#include <QIcon>
#include <fstream>
#include <memory>

#ifdef _WIN32
    #include <QWinTaskbarButton>
#endif


class MyApplication : public QApplication
{
public:
    MyApplication(int &argc, char **argv) : QApplication(argc, argv) {}
    bool notify(QObject* receiver, QEvent* event) override;
};

bool MyApplication::notify(QObject* receiver, QEvent* event)

{
    try {
        return QApplication::notify(receiver, event);
    }
    catch (std::ifstream::failure e) {
        std::ofstream fout("exception.log", std::ios::app);
        fout << "ifstream file error: " << e.what() << std::endl;
        fout.close();
        exit(1);
    }
    catch(const std::runtime_error &e){
        std::ofstream fout("exception.log", std::ios::app);
        fout << "Runtime error: "  << e.what() << std::endl;
        fout.close();
        exit(1);
    }
    catch(const std::out_of_range &e){
        std::ofstream fout("exception.log", std::ios::app);
        fout << "Out of Range error: " << e.what() << '\n';
        fout.close();
        exit(1);
    }
    catch (const std::bad_alloc &e){
        std::ofstream fout("exception.log", std::ios::app);
        fout << "Allocation error: " << e.what() << std::endl;
        fout.close();
       exit(1);
    }
    catch(std::exception &e){
        std::ofstream fout("exception.log", std::ios::app);
        fout << "unspecified standard exception caught: " << e.what() << std::endl;
        fout.close();
        exit(1);
    }
    return false;
}


int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);
    
    auto scroller = std::make_unique<Scroller>();
    auto wiget = std::make_unique<Widget>(scroller.get());
    scroller->setWidget(wiget.get());
    auto w = QDesktopWidget().availableGeometry().width();
    auto h = QDesktopWidget().availableGeometry().height();
    w = (w > 1392) ? 1392 : w;
    h = (h > 922) ? 922 : h;
    scroller->resize(w, h);
    QIcon icon(":/gears.bmp");
    scroller->setWindowIcon(icon);

#ifdef _WIN32
    app.setStyle("fusion");
    auto button = std::make_unique<QWinTaskbarButton>(scroller.get());
    button->setWindow(scroller->windowHandle());
    button->setOverlayIcon(icon);
#endif

    scroller->show();
    return app.exec();
}
