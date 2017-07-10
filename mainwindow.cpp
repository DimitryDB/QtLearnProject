#include <QDockWidget>
#include "books.h"
#include "catalogue.h"
#include "filter.h"
#include "mainwindow.h"



namespace STORE {

/*********************************************************************/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    CATALOG::ColumnView *W = 0;
    Filter *F = 0;
    BOOKS::View *B = new BOOKS::View(this);
    setCentralWidget(B);

    {
     QDockWidget *D =new QDockWidget(this);
     D->setWindowTitle(tr("catalogue"));
     W = new CATALOG::ColumnView(D);
     D->setWidget(W);
     addDockWidget(Qt::TopDockWidgetArea,D);
    }
    {
     QDockWidget *D =new QDockWidget(this);
     D->setWindowTitle(tr("Filter"));
     F = new Filter(D);
     D->setWidget(F);
     addDockWidget(Qt::LeftDockWidgetArea,D);
    }
    connect(W,SIGNAL(item_selected(QVariant)),B->model(),
            SLOT(cat_item_selected(QVariant)));
    connect(F,SIGNAL(apply_filter(QObject*)),B->model(),
                     SLOT(apply_filter(QObject*)));
}

MainWindow::~MainWindow() {

}

/*********************************************************************/

} // namespace STORE
