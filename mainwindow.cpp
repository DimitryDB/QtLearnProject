#include <QDockWidget>
#include <books.h>
#include "mainwindow.h"
#include "catalogue.h"


namespace STORE {

/*********************************************************************/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    CATALOG::Model* M =0;
    BOOKS::View *B = new BOOKS::View(this);
    setCentralWidget(B);

    {
     QDockWidget *D =new QDockWidget(this);
     D->setWindowTitle(tr("catalogue"));
     CATALOG::ColumnView *W = new CATALOG::ColumnView(D);
     D->setWidget(W);
     addDockWidget(Qt::TopDockWidgetArea,D);
     M = qobject_cast<CATALOG::Model*>(W->model());
    }
    {
     QDockWidget *D =new QDockWidget(this);
     D->setWindowTitle(tr("catalogue"));
     D->setWidget(new CATALOG::TreeView(D,M));
     addDockWidget(Qt::LeftDockWidgetArea,D);
    }
}

MainWindow::~MainWindow() {

}

/*********************************************************************/

} // namespace STORE
