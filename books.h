#ifndef BOOKS_H
#define BOOKS_H

#include <QTableView>
#include <QSqlQueryModel>

namespace STORE {
namespace BOOKS {

/*********************************************************************/

class Model : public QSqlQueryModel {
    Q_OBJECT
public:
    Model(QObject *parent = 0);
    virtual ~Model();
    int columnCount(const QModelIndex &) const {return 11;}
public slots:
    void cat_item_selected(QVariant id);
};

/*********************************************************************/

class View : public QTableView {
    Q_OBJECT
    public:
    View (QWidget *parent = 0);
    virtual ~View();

};


} // namespace BOOKS
} // namespace STORE

#endif // BOOKS_H
