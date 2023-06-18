#include "homomorphic_filter.hpp"
#include <chrono>

#include <QApplication>
#include <QCommandLineParser>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCommandLineParser parser;

    MainWindow main_window;
    main_window.show();

    return app.exec();
}
