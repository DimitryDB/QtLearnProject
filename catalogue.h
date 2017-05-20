#ifndef CATALOGUE_H
#define CATALOGUE_H
#include <QAbstractTableModel>
#include <QTableView>
#include <QList>
#include <QPoint>
#include "catitemedit.h"

class QAction;
namespace STORE {
    class PosAction;
}
namespace STORE {
namespace CATALOG {

/*********************************************************************/

class Model : public QAbstractTableModel {
    Q_OBJECT
public:
    Model(QObject *parent = 0);
    virtual ~Model();
    int rowCount(const QModelIndex &parent) const ;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &I, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
protected:
    virtual QVariant dataDisplay(const QModelIndex &I) const;
    virtual QVariant dataTextAlignment(const QModelIndex &I) const;
    virtual ITEM::Data* dataDataPointer(const QModelIndex &I) const;
private:
    QList<ITEM::Data*> Cat;
public slots:
    void editItem(const QModelIndex &i, QWidget *parent =0);
};

/*********************************************************************/

class TableView : public QTableView {
    Q_OBJECT

public:
    TableView(QWidget *parent = 0);
    virtual ~TableView();
private:
    PosAction *actEditItem;
private slots:
    void contextMenuRqusested(const QPoint &p);
};

/*********************************************************************/

} // namespace CATALOG
} // namespace STORE

#endif // CATALOGUE_H
