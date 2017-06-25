#include <QAction>
#include <QtDebug>
#include <Qmenu>
#include <QMessageBox>
#include <QtSql>
#include "catalogue.h"
#include "confdialog.h"
#include "posaction.h"

#define DATA_PTR(D,I,VALUE)  \
STORE::CATALOG::ITEM::Data *D = \
                (STORE::CATALOG::ITEM::Data*)(I.internalPointer()); \
        if(!D) return VALUE

#define DATE_STR(DATE) ( \
DATE.isValid() ? DATE.toString("dd.MM.yyyy") : QString() \
)
namespace STORE {
namespace CATALOG {

/*********************************************************************/

Model::Model(QObject *parent) : QAbstractItemModel(parent) , lastTempId(1) {
    QSqlQuery qry;
    qry.setForwardOnly(true);
    qry.prepare(
        "select                           \n"
        "       iid,                      \n"
        "       code,                     \n"
        "       title,                    \n"
        "       valid_from,               \n"
        "       valid_to,                 \n"
        "       islocal,                  \n"
        "       acomment,                 \n"
        "       rid_parent,               \n"
        "       alevel                    \n"
        "   from catalogue                \n"
        "   order by alevel;              \n"
    );
    if(qry.exec()) {
        while (qry.next()) {
            bool OK = false;
            QVariant PI =  qry.value("rid_parent");
            int parentId = PI.toInt(&OK);
            ITEM::Data *P = 0;
            if(OK && !PI.isNull())
                P = Cat.findPointer(parentId);
            if (P) {
                ITEM::Data *D = new ITEM::Data(P,qry);
                P->Children.append(D);
                D->pParentItem = P;
            } else {
            ITEM::Data *D = new ITEM::Data(this,qry);
            Cat.append(D);
            D->pParentItem = 0;
            }
        }
    } else {
        QSqlError err = qry.lastError();
        qCritical() << err.driverText();
        qCritical() << err.databaseText();
    }
}
Model::~Model() {}
int Model::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return Cat.size() ;
    }
    else {
        DATA_PTR (P,parent,0);
        return P->Children.size();
    }
}
int Model::columnCount(const QModelIndex&) const {
    return 6;
}
QVariant Model::dataDisplay(const QModelIndex &I) const {
    DATA_PTR(D,I,QVariant());
    switch (I.column()) {
    case 1 : return D->Code;
    case 0 : return D->Title;
    case 2 : return DATE_STR(D->From);
    case 3 : return DATE_STR(D->To);
    case 4 : return D->isLocal ? tr("LOCAL") : QString();
    case 5 : return D->Comment.isEmpty() ? QString() : tr("CMT") ;
    default : return QVariant();
    }
}
QVariant Model::dataFont(const QModelIndex &I) const {
    DATA_PTR (D,I,QVariant());
    if (!D) return QVariant();
    QFont F;
    if (D->deleted) F.setStrikeOut(true);
    if (D->changed) F.setItalic(true);
    return F;
}
QVariant Model::dataForeground(const QModelIndex &I) const {
    DATA_PTR (D,I,QVariant());
    QColor Result = D->isLocal ?  QColor("blue") : QColor("black");
    if (!D->dataIsActive()) Result.setAlphaF(0.5);
    return Result;
}
QVariant Model::dataBackground(const QModelIndex &I) const {
    DATA_PTR (D,I,QVariant());
    if (!D->isNew()) return QVariant();
    QColor Result = QColor("green");
    if (!D->dataIsActive()) {
        Result.setAlphaF(0.1);
    } else {
        Result.setAlphaF(0.4);
    }

    return Result;
}
QVariant Model::dataToolTip(const QModelIndex &I) const {
    DATA_PTR (D,I,QVariant());
    switch(I.column()) {
    case 2: { if(!D->To.isValid()) return QVariant();
              return tr("Valid to : %1").arg(D->To.toString("dd.MM.yyyy"));
    }
    default : return QVariant();
    }
}
QVariant Model::dataTextAlignment(const QModelIndex &I) const{
    int Result = Qt::AlignVCenter;
    Result |= I.column() == 0 ? Qt::AlignLeft : Qt::AlignHCenter;
    return Result;
}
QVariant Model::data(const QModelIndex &I, int role) const {
    switch(role) {
    case Qt::DisplayRole : return dataDisplay(I);
    case Qt::TextAlignmentRole : return dataTextAlignment(I);
    case Qt::ForegroundRole : return dataForeground(I);
    case Qt::FontRole : return dataFont(I);
    case Qt::ToolTipRole : return dataToolTip(I);
    case Qt::UserRole+1 : { DATA_PTR(D,I,false);
                            return D->deleted; }
    case Qt::BackgroundRole : return dataBackground(I);
    default : return QVariant();
    }
}
QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal)
        return QAbstractItemModel::headerData(section,orientation,role);
    switch (role) {
    case Qt::DisplayRole :
       switch (section) {
       case 1 : return tr("Code");
       case 0 : return tr("Title");
       case 2 : return tr("From");
       case 3 : return tr("To");
       case 4 : return tr("Local");
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
QModelIndex Model::index(int row, int column, const QModelIndex &parent) const {
    if(parent.isValid()) {
        DATA_PTR(D,parent,QModelIndex());
        if (row < 0 || row >= D->Children.size()) return QModelIndex();
        return createIndex(row, column,(void*)D->Children[row] );
    } else {
        if (row <0 || row >= Cat.size()) return QModelIndex();
        return createIndex(row, column, (void*)(Cat[row]) );
    }
}
QModelIndex Model::parent(const QModelIndex &I) const {
    DATA_PTR(D,I,QModelIndex());
    ITEM::Data *P = D->pParentItem;
    if(!P)
        return QModelIndex();
    int Row = -1;
    ITEM::Data *Gp = P->pParentItem;
    if (Gp) {
        for(int i = 0; i < Gp->Children.size(); i++) {
            if (Gp->Children[i]->isSameAs(P)) {
                Row = i;
                break;
             }
         }
    } else {
        for(int i = 0; i < Cat.size(); i++) {
            if (Cat[i]->isSameAs(P)) {
                Row = i;
                break;
            }
        }
    }
    if (Row < 0) {
        qWarning() <<"Cannot find Item";
        return QModelIndex();
        }
     return createIndex(Row,0,P);
}
void Model::editItem(const QModelIndex &i, QWidget *parent) {
    ITEM::Dialog dia(parent);
    DATA_PTR(D,i,);
    if (!D) return;
    beginResetModel();
    dia.setDataBlock(D);
    dia.exec();
    endResetModel();
}
void Model::newItem(const QModelIndex &parentI, QWidget *parent) {
    ITEM::Data *P=0;
    if (parentI.isValid()) {
        P=(ITEM::Data*)(parentI.internalPointer());
        if(!P) {
            qWarning()<< "Invalid internal pointer";
            return;
        }
    }
    ITEM::Data *D = new ITEM::Data(this);
    if (!D) {
        qWarning() << tr("Cannot create new Item");
        return;
    }
    ITEM::Dialog dia(parent);
    dia.setDataBlock(D);
    if(dia.exec() == QDialog::Accepted ) {
        beginResetModel();
        if(P) {
            P->Children.append(D);
            D->pParentItem = P;
        } else {
        Cat.append(D);
        }
        D->setProperty("tempId", tmpId() );
        endResetModel();
    } else {
        delete D;
    }
}
void Model::dellItem(const QModelIndex &i, QWidget *parent) {
    DATA_PTR(D,i,);
    if (!D) return;
    ConfDialog check(parent);
    if (check.exec() == QDialog::Accepted) {
        beginResetModel();
        if (D->isNew()) {
            ITEM::Data *P =D->pParentItem;
            if(P) {
                P->Children.removeAt(i.row());
            }
            Cat.removeAt(i.row());
            delete D;
        } else {
            D->deleted = !D->deleted;
        }
        endResetModel();
    }
}
bool Model::deleteAllfromDb(ITEM::Data *D) {
    ITEM::List *Children = D ? &(D->Children) : &Cat;
    ITEM::Data *X;
    foreach (X, *Children) {
       if (!deleteAllfromDb(X))
           return false;
    }
    if(!D) return true;
    if(!D->deleted) return true;
    {
    QSqlQuery DEL;
    DEL.setForwardOnly(true);
    DEL.prepare("delete from catalogue where iid = :IID ;");
    DEL.bindValue(":IID",D->id);
    if (DEL.exec()) return true;
    qCritical() << DEL.lastError().databaseText().toUtf8().data();
    qCritical() << DEL.lastError().nativeErrorCode();
    }
    return false;
}
bool Model::deleteAllfromModel(ITEM::Data *D) {
    ITEM::List *Children = D ? &(D->Children) : &Cat;
    bool result = true;
    beginResetModel();
    for (int k = Children->size()-1 ; k>=0 ; k--) {
        if (Children->at(k)->deleted) {
            ITEM::Data *X = Children->takeAt(k);
            delete X;
        } else {
            if (!deleteAllfromModel(Children->at(k))) {
                    result = false;
                    break;
            }
        }
    }
    endResetModel();
    return result;
}
bool Model::deleteAll() {
    QSqlDatabase DB = QSqlDatabase::database();
    DB.transaction();
    if (deleteAllfromDb()) {
        DB.commit();
    }else{
        DB.rollback();
        return false;
    }
    return deleteAllfromModel();
}
bool Model::saveAlltoDb(ITEM::Data *D) {
    ITEM::List *Children = D ? &(D->Children) : &Cat;
    ITEM::Data *X;
    foreach (X, *Children) {
        if (!saveAlltoDb(X))
            return false;
    }
    if(!D) return true;
    if(!D->changed) return true;

    QSqlQuery UPD;
    UPD.setForwardOnly(true);
    UPD.prepare(
                "update catalogue set    \n"
                "  code       = :CODE ,  \n"
                "  title      = :TITLE , \n"
                "  valid_from = :FROM ,  \n"
                "  valid_to   = :TO ,    \n"
                "  islocal    = :LOCAL , \n"
                "  acomment   = :COMMENT \n"
                "  where iid  = :IID ;   \n"
                );
    UPD.bindValue(":CODE",      D->Code);
    UPD.bindValue(":TITLE",    D->Title);
    UPD.bindValue(":FROM",     D->From);
    UPD.bindValue(":TO",          D->To);
    UPD.bindValue(":LOCAL",  D->isLocal);
    UPD.bindValue(":COMMENT",D->Comment);
    UPD.bindValue(":IID",         D->id);
    if (UPD.exec()) return true;
    qCritical() << UPD.lastError().databaseText().toUtf8().data();
    qCritical() << UPD.lastError().nativeErrorCode();
    return false;
}
bool Model::dropChangedMark(ITEM::Data *D) {
    ITEM::List *Children = D ? &(D->Children) : &Cat;
    ITEM::Data *X;
    foreach (X, *Children) {
        X->changed = false;
        if (!dropChangedMark(X))
            return false;
    }
return true;
}
bool Model::saveAll() {
    QSqlDatabase DB = QSqlDatabase::database();
    DB.transaction();
    if (saveAlltoDb()) {
        DB.commit();
        dropChangedMark();
    }else{
        DB.rollback();
        return false;
    }
    return true;
}
bool Model::insertNewToDb(ITEM::Data *D) {
    bool mustBeSaved = D ? D->isNew() : false;
    if(mustBeSaved) {
        QSqlQuery INS;
        INS.setForwardOnly(true);
        INS.prepare(
                    "insert into catalogue ( "
                    "code                   ,\n"
                    "title                  ,\n"
                    "valid_from             ,\n"
                    "valid_to               ,\n"
                    "islocal                ,\n"
                    "acomment               ,\n"
                    "rid_parent              \n"
                    "    ) values (          \n"
                    ":CODE                  ,\n"
                    ":TITLE                 ,\n"
                    ":FROM                  ,\n"
                    ":TO                    ,\n"
                    ":LOCAL                 ,\n"
                    ":COMMENT               ,\n"
                    ":PARENT                 \n"
                    "    ) returning         \n"
                    "iid                    ,\n"
                    "code                   ,\n"
                    "title                  ,\n"
                    "valid_from             ,\n"
                    "valid_to               ,\n"
                    "islocal                ,\n"
                    "acomment               ,\n"
                    "rid_parent             ,\n"
                    "alevel                 ;\n"
                    );
        INS.bindValue(":CODE",      D->Code);
        INS.bindValue(":TITLE",    D->Title);
        INS.bindValue(":FROM",     D->From);
        INS.bindValue(":TO",          D->To);
        INS.bindValue(":LOCAL",  D->isLocal);
        INS.bindValue(":COMMENT",D->Comment);
        QVariant idParent = QVariant();
        if(D->pParentItem) {
            if(D->pParentItem->isNew()) {
                idParent = D->pParentItem->property("new_id");
            } else
                idParent = D->pParentItem->id;
        }
        INS.bindValue(":PARENT", idParent);
        if (!INS.exec()) {
             qCritical() << INS.lastError().databaseText().toUtf8().data();
            qCritical() << INS.lastError().nativeErrorCode();
            return false;
        }
        while(INS.next()) {
            D->Code = INS.value("code").toString();
            D->Title = INS.value("title").toString();
            D->From = INS.value("valid_from").toDateTime();
            D->To = INS.value("valid_to").toDateTime();
            D->Comment = INS.value("acomment").toString();
            D->isLocal = INS.value("islocal").toBool();
            qDebug() << INS.value("rid_parent") << INS.value("alevel");
            D->setProperty("new_id",INS.value("iid"));
        }
    }
    ITEM::List *Children = D ? &(D->Children) : &Cat;
    ITEM::Data *X;
    foreach (X, *Children) {
        if (!insertNewToDb(X))
            return false;
    }
    return true;
}
bool Model::adjustIdForNew(ITEM::Data *D) {
    bool mustBeSaved = D ? D->isNew() : false;
    if(mustBeSaved) {
        D->id = D->property("new_id");
    }
    ITEM::List *Children = D ? &(D->Children) : &Cat;
    ITEM::Data *X;
    foreach (X, *Children) {
        if (!adjustIdForNew(X))
        return false;
    }
    return true;
}
bool Model::insertAll() {
    QSqlDatabase DB = QSqlDatabase::database();
    DB.transaction();
    if (insertNewToDb()) {
        DB.commit();
        adjustIdForNew();
    }else{
        DB.rollback();
        return false;
    }
    return true;
}
void Model::save(void) {
    try {
    if(! deleteAll()) throw (int)1;
    if(! saveAll()) throw(int)2;
    if(! insertAll()) throw(int)3;
    QMessageBox::information(0,tr("Info"), tr("Succsessfuly Chabged"));
    } catch (int) {
            QMessageBox::critical(0,tr("Error"),tr("Cannot save changes"));
    }
}

/*********************************************************************/

TableView::TableView(QWidget *parent, Model *xModel) : QTableView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(contextMenuRqusested(QPoint)));
    Model *M = xModel ? xModel : new Model(this);
    setModel(M);
    {
     PosAction *A = actEditItem = new PosAction(this);
     A->setText(tr("Edit"));
     connect (A,SIGNAL(editItem(QModelIndex,QWidget*)),
              M, SLOT(editItem(QModelIndex,QWidget*)));
     addAction(A);
    }
    {
     PosAction *A = actNewItem = new PosAction(this);
     A->setText(tr("Add"));
     connect (A,SIGNAL(editItem(QModelIndex,QWidget*)),
              M, SLOT(newItem(QModelIndex,QWidget*)));
     addAction(A);
    }
    {
     PosAction *A = actdellItem = new PosAction(this);
     A->setText(tr("Del"));
     connect (A,SIGNAL(editItem(QModelIndex,QWidget*)),
              M, SLOT(dellItem(QModelIndex,QWidget*)));
     addAction(A);
    }
    {
     PosAction *A = actRootItem = new PosAction(this);
     A->setText(tr("Show Children"));
     connect (A,SIGNAL(editItem(QModelIndex,QWidget*)),
              this, SLOT(showChildren(QModelIndex,QWidget*)));
     addAction(A);
    }
    {
     QAction *A = actParentRootItem = new QAction(this);
     A->setText(tr("Show Parent"));
     connect (A,SIGNAL(triggered()), this, SLOT(showParent()));
     addAction(A);
    }
    {
     QAction *A = actSave = new QAction(this);
     A->setText(tr("Save"));
     connect (A,SIGNAL(triggered()), M, SLOT(save()));
     addAction(A);
    }
    {
     QHeaderView *H = verticalHeader();
     H->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
    {
     QHeaderView *H = horizontalHeader();
     H->setSectionResizeMode(QHeaderView::ResizeToContents);
     H->setSectionResizeMode(0,QHeaderView::Stretch);
    }
    setColumnHidden(3,true);
    setColumnHidden(4,true);

}
TableView::~TableView(){}
void TableView::contextMenuRqusested(const QPoint &p) {
    QMenu M(this);
    QModelIndex i = indexAt(p);
    if (i.isValid()) {

        actEditItem->I = i;
        actEditItem->pWidget = this;
        M.addAction(actEditItem);
        actdellItem->I =i;
        actdellItem->pWidget = this;
        if (i.data(Qt::UserRole+1).toBool()) {
            actdellItem->setText(tr("Restore"));
        } else {
            actdellItem->setText(tr("Del"));
        }
        M.addAction(actdellItem);     
    }
    actNewItem->I = rootIndex();
    actNewItem->pWidget = this;
    M.addAction(actNewItem);
    if (i.isValid()) {
        actRootItem->I = i;
        actRootItem->pWidget = this;
        M.addAction(actRootItem);
    }
    if(rootIndex().isValid()) {
        M.addAction(actParentRootItem);
    }
    M.addAction(actSave);
    M.exec(mapToGlobal(p));
}
void TableView::showChildren(const QModelIndex I, QWidget *) {
    setRootIndex(I);
}
void TableView::showParent(void) {
    if (rootIndex().isValid())
        setRootIndex(rootIndex().parent());
}

/*********************************************************************/

TreeView::TreeView(QWidget *parent, Model *xModel) : QTreeView(parent) {
    Model *M = xModel ? xModel : new Model(this);
    setModel(M);
    setColumnHidden(3,true);
    setColumnHidden(4,true);
    {
     QHeaderView *H = header();
     H->setSectionResizeMode(QHeaderView::ResizeToContents);
     H->setSectionResizeMode(1,QHeaderView::Stretch);
    }
}
TreeView::~TreeView() {}

/*********************************************************************/

ColumnView::ColumnView(QWidget *parent, Model *xModel) :QColumnView(parent) {
    Model *M = xModel ? xModel : new Model(this);
    setModel(M);
    QList<int> L;
    for(int k=0;k<10;k++)
        L<<150;
    setColumnWidths(L);
}
ColumnView::~ColumnView() {}

void ColumnView::currentChanged(const QModelIndex &current,
                    const QModelIndex &previous) {
    QColumnView::currentChanged(current,previous);
    if(! current.isValid()) {
        emit item_selected(QVariant());
        return;
    }
    STORE::CATALOG::ITEM::Data *D =
                    (STORE::CATALOG::ITEM::Data*)(current.internalPointer());
    if(!D) {
        emit item_selected(QVariant());
        return;
    } else {
        emit item_selected(D->id);
    }
//    qDebug() << previous << current;
}

/*********************************************************************/

} // namespace CATALOG
} // namespace STORE
