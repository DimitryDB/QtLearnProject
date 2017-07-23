#include <QComboBox>
#include <QHeaderView>
#include <QPainter>
#include <QSqlError>
#include <QSqlQuery>
#include <QtDebug>
#include "books.h"


namespace  {
    void REPORT_ERROR(QSqlQuery &QUERY ) {
        qDebug() <<  QUERY.executedQuery().toUtf8().data();
        qCritical() << QUERY.lastError().databaseText().toUtf8().data();
    }
} //namespace

namespace STORE {
namespace BOOKS {

/*********************************************************************/
StatusDelegate::StatusDelegate(QObject *parent, const QMap<int, QString> &AllStatus)
    : QItemDelegate(parent), fAllStatus(AllStatus){}

QWidget *StatusDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &,
                      const QModelIndex &) const {
    QComboBox *CB = new QComboBox(parent);
    QMapIterator<int,QString> K(fAllStatus);
    CB->addItem(QString(), QVariant());
    while (K.hasNext()) {
        K.next();
        CB->addItem(K.value(), K.key());
    }
    return CB;
}
/*********************************************************************/

void StatusDelegate::setEditorData(QWidget *editor, const QModelIndex &I) const {
    QComboBox *CB = qobject_cast<QComboBox*>(editor);
    if(!CB) return;
    QVariant idStatus = I.data(Qt::EditRole);

    if (!idStatus.isValid()) {
        CB->setCurrentIndex(0);
        return;
    }
    for (int k = 1; k < CB->count(); k++) {
        if (CB->itemData(k) == idStatus) {
            CB->setCurrentIndex(k);
            break;
        }
    }
}
/*********************************************************************/

void StatusDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                  const QModelIndex &I) const {
    QComboBox *CB = qobject_cast<QComboBox*>(editor);
    if(!CB) return;
    model->setData(I, CB->currentData(), Qt::EditRole);
}

/*********************************************************************/

void StatusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &I) const {
    QItemDelegate::paint(painter,option,I);
    if (I.data(Qt::EditRole) != -2 ) return;

    painter->setBrush(QBrush(QColor("black"), Qt::DiagCrossPattern));
    painter->setPen(Qt::NoPen);
    painter->drawRect(option.rect);
}
/*********************************************************************/

Model::Model(QObject *parent) : QSqlQueryModel(parent) {}

Model::~Model() {}

/*********************************************************************/

 void Model::adjust_query() {

     QString QueryText =
                "select                        \n"
                "b.iid,                        \n"
                "b.rid_catalogue,              \n"
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
                "where b.rid_catalogue = :CID  \n";     ;
     if (fAuthor.isValid())
         QueryText += "     and b.author ~  :AUTHOR \n";
     if (fTitle.isValid())
         QueryText += "     and b.title  ~  :TITLE   \n";
     if (fYear.isValid())
         QueryText += "     and b.eyear  =  :YEAR   \n";

     QSqlQuery qry;
     qry.prepare(QueryText);

     qry.bindValue(":CID",fCatId);
     if (fAuthor.isValid())
        qry.bindValue(":AUTHOR","^"+fAuthor.toString());
     if (fTitle.isValid())
        qry.bindValue(":TITLE",fTitle);
     if (fYear.isValid())
        qry.bindValue(":YEAR",fYear);

     if(!qry.exec()) {
         REPORT_ERROR(qry);
         return;
     }
     setQuery(qry);
 }

/*********************************************************************/

void Model::cat_item_selected(QVariant id) {
     fCatId = id;
     adjust_query();
}

/*********************************************************************/

void Model::apply_filter(QObject *F) {
     fAuthor = F-> property("author");
     fTitle  = F-> property("title");
     fYear   = F-> property("year");
     adjust_query();
}

/*********************************************************************/

Model_EditOnServer::Model_EditOnServer(QObject *parent) : QSqlTableModel(parent) {
    setEditStrategy(OnFieldChange);
    {
     QAction *A = actDeleteRow = new QAction(this);
     A->setText(tr("Delete"));
     connect(A,SIGNAL(triggered()),this, SLOT(on_delete_row()));
     AllActions << A;
    }
    {
     QAction *A = actNewRow = new QAction(this);
     A->setText(tr("New"));
     connect(A,SIGNAL(triggered()),this, SLOT(on_new_row()));
     AllActions << A;
    }
    {
     QAction *A = actSaveAll = new QAction(this);
     A->setText(tr("Save"));
     connect(A,SIGNAL(triggered()),this, SLOT(on_save_all()));
     AllActions << A;
    }
    {
     QAction *A = actRestoreAll =new QAction(this);
     A->setText(tr("Restore"));
     connect(A,SIGNAL(triggered()),this, SLOT(on_restore_all()));
     AllActions << A;
    }
    {
     QSqlQuery QUERY;
     QUERY.prepare("select  iid, title from status;");
     bool OK = QUERY.exec();
     if(!OK) {
         REPORT_ERROR(QUERY);
         return;
     }
         while(QUERY.next()) {
             int v = QUERY.value("iid").toInt() ;
             QString  t = QUERY.value("title").toString();
             AllStatus[v] = t;

     }
     }

}

Model_EditOnServer::~Model_EditOnServer() {}

/*********************************************************************/

void Model_EditOnServer::on_save_all() {
    qDebug() <<"on_save_all";
}

/*********************************************************************/

void Model_EditOnServer::on_restore_all() {
    qDebug() <<"on_restore_all";
}

/*********************************************************************/

void Model_EditOnServer::on_delete_row() {
    qDebug() <<"on_delete_row";
}

/*********************************************************************/

void Model_EditOnServer::on_new_row() {
    insertRow(0);
    setData(index(0,1), fCatId, Qt::EditRole);
    qDebug() <<"on_new_row";
}

/*********************************************************************/

 void Model_EditOnServer::adjust_query() {
     setTable(QString());
     {
      QSqlQuery DROP;
      bool OK = DROP.exec("drop table if exists my_books;");
      if(!OK) {
           REPORT_ERROR(DROP);
           return;
      }
     }
     {
      QSqlQuery CREATE;
      bool OK = CREATE.exec(
                  "create temporary table my_books (\n"
                  "iid bigint primary key,          \n"
                  "rid_catalogue bigint,            \n"
                  "author text,                     \n"
                  "title text,                      \n"
                  "eyear int,                       \n"
                  "elocation text,                  \n"
                  "publisher text,                  \n"
                  "pages int,                       \n"
                  "annote text,                     \n"
                  "rid_status bigint,               \n"
                  "status_title text,               \n"
                  "aconnent text                    \n"
                  ");                               \n"
                  );
      if(!OK) {
          REPORT_ERROR(CREATE);
          return;
      }
     }
     {
      QString QueryText =
                 "insert into my_books (             \n"
                 "iid,                               \n"
                 "rid_catalogue,                     \n"
                 "author,                            \n"
                 "title,                             \n"
                 "eyear,                             \n"
                 "elocation,                         \n"
                 "publisher,                         \n"
                 "pages,                             \n"
                 "annote,                            \n"
                 "rid_status,                        \n"
                 "status_title,                      \n"
                 "aconnent                           \n"
                 ")                                  \n"
                 "select                             \n"
                 "b.iid,                             \n"
                 "b.rid_catalogue,                   \n"
                 "b.author,                          \n"
                 "b.title,                           \n"
                 "b.eyear,                           \n"
                 "b.elocation,                       \n"
                 "b.publisher,                       \n"
                 "b.pages,                           \n"
                 "b.annote,                          \n"
                 "b.rid_status,                      \n"
                 "s.title,                           \n"
                 "b.aconnent                         \n"
                 "from books b                       \n"
                 "left outer join status s           \n"
                 "       on b.rid_status =s.iid      \n"
                 "where b.rid_catalogue = :CID       \n";
      if (fAuthor.isValid())
          QueryText += "     and b.author ~  :AUTHOR \n";
      if (fTitle.isValid())
          QueryText += "     and b.title  ~  :TITLE   \n";
      if (fYear.isValid())
          QueryText += "     and b.eyear  =  :YEAR   \n";
      QSqlQuery qry;
      qry.prepare(QueryText);
      qry.bindValue(":CID",fCatId);
      if (fAuthor.isValid())
         qry.bindValue(":AUTHOR","^"+fAuthor.toString());
      if (fTitle.isValid())
         qry.bindValue(":TITLE",fTitle);
      if (fYear.isValid())
         qry.bindValue(":YEAR",fYear);
      if(!qry.exec())
          REPORT_ERROR(qry);
     }
     setTable("my_books");
     if(!select()) {
         qCritical() << "Error selecting";
     }
}

/*********************************************************************/

void Model_EditOnServer::cat_item_selected(QVariant id) {
     fCatId = id;
     adjust_query();
}

/*********************************************************************/

void Model_EditOnServer::apply_filter(QObject *F) {
     fAuthor = F-> property("author");
     fTitle  = F-> property("title");
     fYear   = F-> property("year");
     adjust_query();
}

/*********************************************************************/

QVariant Model_EditOnServer::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal)
        return QAbstractItemModel::headerData(section,orientation,role);
    switch (role) {
    case Qt::DisplayRole :
       switch (section) {
       case 2 : return tr("Author");
       case 3 : return tr("Title");
       case 4 : return tr("Year");
       case 5 : return tr("Location");
       case 6 : return tr("Publisher");
       case 7 : return tr("Pages");
       case 8 : return tr("Note");
       case 10 : return tr("Status");
       case 11 : return tr("Comment");
       default : return QVariant();
       }
    case Qt::TextAlignmentRole :
       return QVariant(Qt::AlignBaseline | Qt::AlignCenter);
    case Qt::ForegroundRole :
       { // TO DO  Fix Font
        QFont F;
        F.setBold(true);
        return F;
       }
    default : return QVariant();
    }
}
/*********************************************************************/

QVariant Model_EditOnServer::data(const QModelIndex &I, int role) const {
    if (role == Qt::EditRole && I.column() == 10)
        return QSqlTableModel::data(index(I.row(),9),role);
    else
        return QSqlTableModel::data(I,role);
}
/*********************************************************************/

  bool Model_EditOnServer::setData(const QModelIndex &I, const QVariant &val, int role) {
      bool Result = true;
      if (role == Qt::EditRole) {
          if (val.isValid()) {
              bool OK;
              int statusId = val.toInt(&OK);
              if (!OK) {
                  qWarning()<< "invalid Status to Int" << val;
                  return false;
              } else if(!AllStatus.contains(statusId)) {
                  qWarning()<< "invalid Status contains" << statusId;;
                  return false;
              }
               Result |= QSqlTableModel::setData(index(I.row(),9),val,role);
               Result |= QSqlTableModel::setData(I,AllStatus[statusId],role);
          } else {
              Result |= QSqlTableModel::setData(index(I.row(),9),QVariant(),role);
              Result |= QSqlTableModel::setData(I,QString(),role);
          }
      }
      else
          return QSqlTableModel::setData(I,val,role);
      return Result;
  }

/*********************************************************************/

Qt::ItemFlags Model_EditOnServer::flags(const QModelIndex &I) const {
    Qt::ItemFlags Result =  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (I.column()!=0 )
        Result = Result | Qt::ItemIsEditable;
    return Result;
}

/*********************************************************************/

QVariant Model_EditOnServer::dataBackground(const QModelIndex &) const {
    //TO DO : Set background to new created objects;
    return QVariant();
}

/*********************************************************************/

    View::View (QWidget *parent) : QTableView (parent) {
    Model_EditOnServer *M = new Model_EditOnServer(this);
    setModel(M);
    addActions(M->AllActions);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    setColumnHidden(0,true);
    setColumnHidden(1,true);
    setColumnHidden(9,true);
    setWordWrap(false);
    setAlternatingRowColors(true);
    {
     QHeaderView *H = verticalHeader();
     H->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
    {
     QHeaderView *H = horizontalHeader();
     H->setSectionResizeMode(QHeaderView::ResizeToContents);
     H->setSectionResizeMode(3, QHeaderView::Stretch);
    }
    setItemDelegateForColumn(10, new StatusDelegate(this,M->AllStatus));
}

    View::~View() {}

/*********************************************************************/
} // namespace BOOKS
} // namespace STORE
