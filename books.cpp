#include <QHeaderView>
#include <QSqlError>
#include <QSqlQuery>
#include <QtDebug>
#include "books.h"

namespace STORE {
namespace BOOKS {

/*********************************************************************/

    Model::Model(QObject *parent) : QSqlQueryModel(parent) {}

    Model::~Model() {}

    void Model::cat_item_selected(QVariant id) {
        QSqlQuery qry;
        qry.prepare(
                   "select                        \n"
                   "b.iid,                        \n"
                   "b.author,                     \n"
                   "b.title,                      \n"
                   "b.eyear,                      \n"
                   "b.elocation,                  \n"
                   "b.publisher,                  \n"
                   "b.pages,                      \n"
                   "b.annote,                     \n"
                   "b.rid_status,                 \n"
                   "s.title,                      \n"
                   "b.aconnent                    \n"
                   "from books b                  \n"
                   "left outer join status s      \n"
                   "       on b.rid_status =s.iid \n"
                   "where b.rid_catalogue = :CID; \n"
        );
        qry.bindValue(":CID",id);
        if(!qry.exec()) {
            qCritical() << qry.lastError().databaseText().toUtf8().data();
        }
        setQuery(qry);
        }

/*********************************************************************/

    View::View (QWidget *parent) : QTableView (parent) {
    Model *M = new Model(this);
    setModel(M);
    setColumnHidden(0,true);
    //setColumnHidden(1,true);
    setColumnHidden(8,true);
    setWordWrap(false);
    {
     QHeaderView *H = verticalHeader();
     H->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
    }

    View::~View() {}

/*********************************************************************/
} // namespace BOOKS
} // namespace STORE
