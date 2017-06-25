#include <QDockWidget>
#include <books.h>
#include "mainwindow.h"
#include "catalogue.h"


namespace STORE {

/*********************************************************************/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    CATALOG::Model* M =0;
    CATALOG::ColumnView *W = 0;
    BOOKS::View *B = new BOOKS::View(this);
    setCentralWidget(B);

    {
     QDockWidget *D =new QDockWidget(this);
     D->setWindowTitle(tr("catalogue"));
     W = new CATALOG::ColumnView(D);
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
    connect(W,SIGNAL(item_selected(QVariant)),B->model(),
            SLOT(cat_item_selected(QVariant)));
}

MainWindow::~MainWindow() {

}

/*********************************************************************/

} // namespace STORE
