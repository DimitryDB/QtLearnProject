#ifndef BOOKS_H
#define BOOKS_H

#include <QTableView>
#include <QSqlTableModel>
#include <QSqlQueryModel>

namespace STORE {
namespace BOOKS {

/*********************************************************************/

class Model : public QSqlQueryModel {
    Q_OBJECT
public:
    Model(QObject *parent = 0);
    virtual ~Model();
    int columnCount(const QModelIndex &) const {return 12;}
private:
    void adjust_query();
    QVariant fAuthor;
    QVariant fTitle;
    QVariant fYear;
    QVariant fCatId;
public slots:
    void cat_item_selected(QVariant id);
    void apply_filter(QObject *F);
};

/*********************************************************************/

class Model_EditOnServer : public QSqlTableModel {
    Q_OBJECT
public:
    Model_EditOnServer(QObject *parent = 0);
    virtual ~Model_EditOnServer();
    int columnCount(const QModelIndex &) const {return 12;}
private:
    void adjust_query();
    QVariant fAuthor;
    QVariant fTitle;
    QVariant fYear;
    QVariant fCatId;
public slots:
    void cat_item_selected(QVariant id);
    void apply_filter(QObject *F);
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
