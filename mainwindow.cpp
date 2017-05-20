#include "mainwindow.h"
#include "catalogue.h"

namespace STORE {

/*********************************************************************/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    CATALOG::TableView *W = new CATALOG::TableView(this);
    setCentralWidget(W);
}

MainWindow::~MainWindow() {

}

/*********************************************************************/

} // namespace STORE
