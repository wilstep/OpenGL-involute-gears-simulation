#include "widget.h"
#include "scroller.h"
#include <QApplication>
#include <QScrollArea>
#include <QIcon>
#include <fstream>

//class MyApplication : public QGuiApplication
class MyApplication : public QApplication
{
public:
    MyApplication(int &argc, char **argv) : QApplication(argc, argv) {}

    bool notify(QObject* receiver, QEvent* event) Q_DECL_OVERRIDE
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
};


int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);
    
    Scroller *scroller = new Scroller;
    Widget *wiget = new Widget(scroller);
    scroller->setWidget(wiget);
    auto w = QDesktopWidget().availableGeometry().width();
    auto h = QDesktopWidget().availableGeometry().height();
    w = (w > 1392) ? 1392 : w;
    h = (h > 922) ? 922 : h;
    scroller->resize(w, h);
    QIcon icon(":/gears.png");
    scroller->setWindowIcon(icon);
    scroller->show();

#ifdef _WIN32
    app.setStyle("fusion");
#endif
    return app.exec();
}
