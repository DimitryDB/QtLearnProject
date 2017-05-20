#include "mainwindow.h"
#include "Application.h"


int main(int argc, char *argv[])
{
    STORE::Application a(argc, argv);
    STORE::MainWindow w;
    w.show();

    return a.exec();
}
