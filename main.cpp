// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3

#include "mainwindow.h"
#include <QApplication>
#include <iostream>
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
    MainWindow window;
    window.setFixedSize(1390, 920);
    QIcon icon(":/gears.png");
    window.setWindowIcon(icon);
    window.show();

    return app.exec();
}
