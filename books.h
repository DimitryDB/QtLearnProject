#ifndef BOOKS_H
#define BOOKS_H

#include <QAction>
#include <QItemDelegate>
#include <QList>
#include <QMap>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlQueryModel>

namespace STORE {
namespace BOOKS {

/*********************************************************************/

class StatusDelegate : public QItemDelegate {
    Q_OBJECT
public:
    StatusDelegate(QObject *parent, const QMap<int,QString> &AllStatus);
    virtual ~StatusDelegate(void) {}
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &) const;
    void setEditorData(QWidget *editor, const QModelIndex &I) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &I) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &I) const;
private:
    QMap<int,QString> fAllStatus;
};


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
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    QVariant data(const QModelIndex &I, int role) const;
    bool setData(const QModelIndex &I, const QVariant &val, int role);
    // public becouse i want to give presentation level direct
    // acess to this lists
    QList<QAction*> AllActions;
    QMap<int,QString> AllStatus;
protected:
    virtual QVariant dataBackground(const QModelIndex &I) const;
private:

    void adjust_query();
    QVariant fAuthor;
    QVariant fTitle;
    QVariant fYear;
    QVariant fCatId;

    QAction *actDeleteRow;
    QAction *actNewRow;
    QAction *actSaveAll;
    QAction *actRestoreAll;
public slots:
    void cat_item_selected(QVariant id);
    void apply_filter(QObject *F);
protected slots:
    void on_save_all();
    void on_restore_all();
    void on_delete_row();
    void on_new_row();
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
