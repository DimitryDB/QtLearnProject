#ifndef CATALOGUE_H
#define CATALOGUE_H
#include <QAbstractItemModel>
#include <QTableView>
#include <QTreeView>
#include <QColumnView>
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

class Model : public QAbstractItemModel {
    Q_OBJECT
public:
    Model(QObject *parent = 0);
    virtual ~Model();
    int rowCount(const QModelIndex &parent) const ;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &I, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &I) const;
protected:
    int tmpId() const {return ++lastTempId;}
    virtual QVariant dataDisplay(const QModelIndex &I) const;
    virtual QVariant dataTextAlignment(const QModelIndex &I) const;
    virtual QVariant dataFont(const QModelIndex &I) const;
    virtual QVariant dataForeground(const QModelIndex &I) const;
    virtual QVariant dataBackground(const QModelIndex &I) const;
    virtual QVariant dataToolTip(const QModelIndex &I) const;
    bool deleteAll();
    bool saveAll();
    bool insertAll();


private:
    bool deleteAllfromDb(ITEM::Data *D = 0);
    bool deleteAllfromModel(ITEM::Data *D = 0);
    bool saveAlltoDb(ITEM::Data *D = 0);
    bool dropChangedMark(ITEM::Data *D = 0);
    bool insertNewToDb(ITEM::Data *D = 0);
    bool adjustIdForNew(ITEM::Data *D = 0);
    mutable int lastTempId;
    ITEM::List Cat;
public slots:
    void editItem(const QModelIndex &i, QWidget *parent =0);
    void newItem(const QModelIndex &parentI, QWidget *parent=0);
    void dellItem(const QModelIndex &i, QWidget *parent =0);
    void save();
};

/*********************************************************************/

class TableView : public QTableView {
    Q_OBJECT

public:
    TableView(QWidget *parent = 0, Model *xModel = 0);
    virtual ~TableView();
private:
    PosAction *actNewItem;
    PosAction *actEditItem;
    PosAction *actdellItem;
    PosAction *actRootItem;
    QAction *actParentRootItem;
    QAction *actSave;
private slots:
    void contextMenuRqusested(const QPoint &p);
    void showChildren(const QModelIndex, QWidget*);
    void showParent(void);
};

/*********************************************************************/

class TreeView : public QTreeView {
    Q_OBJECT
public:
    TreeView(QWidget *parent = 0, Model *xModel = 0);
    virtual ~TreeView();
};

/*********************************************************************/

class ColumnView : public QColumnView {
    Q_OBJECT
public:
    ColumnView(QWidget *parent = 0, Model *xModel = 0);
    virtual ~ColumnView();
protected:
    void currentChanged(const QModelIndex &current,
                        const QModelIndex &previous);
signals:
    void item_selected(QVariant id);
};

/*********************************************************************/

} // namespace CATALOG
} // namespace STORE

#endif // CATALOGUE_H
