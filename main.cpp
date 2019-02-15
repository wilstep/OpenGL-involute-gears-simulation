// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3

#include "mainwindow.h"
#include <QApplication>
#include <iostream>
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
            std::cerr << "ifstream file error: " << e.what() << std::endl;
            exit(1);
        }
        catch(const std::runtime_error &e){
            std::cerr << "Runtime error: "  << e.what() << std::endl;
            exit(1);
        }
        catch(const std::out_of_range &e){
            std::cerr << "Out of Range error: " << e.what() << '\n';
            exit(1);
        }
        catch (const std::bad_alloc &e){
            std::cerr << "Allocation error: " << e.what() << std::endl;
            exit(1);
        }
        catch(std::exception &e){
            std::cerr << "unspecified standard exception caught: " << e.what() << std::endl;
            exit(1);
        }
        return false;
     }
};


int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);
    MainWindow window;
    window.setFixedSize(1390, 920);
    window.show();

    return app.exec();
}
