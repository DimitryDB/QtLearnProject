#include <QAction>
#include <QtDebug>
#include <Qmenu>
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

int Model::columnCount(const QModelIndex &parent) const {
        return 6 ;   
}

QVariant Model::dataDisplay(const QModelIndex &I) const {
    DATA_PTR(D,I,QVariant());
    switch (I.column()) {
    case 0 : return D->Code;
    case 1 : return D->Title;
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
    return F;
}
QVariant Model::dataForeground(const QModelIndex &I) const {
    DATA_PTR (D,I,QVariant());
    QColor Result = D->isLocal ?  QColor("blue") : QColor("black");
    if (!D->dataIsActive()) Result.setAlphaF(0.5);
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
    Result |= I.column() == 1 ? Qt::AlignLeft : Qt::AlignHCenter;
    return Result;
}

//ITEM::Data* Model::dataDataPointer (const QModelIndex &I) const {
//    int R =  I.row();
//    if (R <0 || R >= Cat.size()) return 0;
//    return Cat[R];
//}

QVariant Model::data(const QModelIndex &I, int role) const {
    switch(role) {
    case Qt::DisplayRole : return dataDisplay(I);
    case Qt::TextAlignmentRole : return dataTextAlignment(I);
    case Qt::ForegroundRole : return dataForeground(I);
    case Qt::FontRole : return dataFont(I);
    case Qt::ToolTipRole : return dataToolTip(I);
    case Qt::UserRole+1 : { DATA_PTR(D,I,false);
                            return D->deleted; }
    default : return QVariant();
    }
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal)
        return QAbstractItemModel::headerData(section,orientation,role);
    switch (role) {
    case Qt::DisplayRole :
       switch (section) {
       case 0 : return tr("Code");
       case 1 : return tr("Title");
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
        return createIndex(row, column,(void*)D );
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
        qWarning() <<"Cannot Finde Item";
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
    if (parentI.isValid()) {
        // TO DO Add new element
        qWarning() << tr("Cannot add non top level item");
        return;
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
        Cat.append(D);
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
    // Assume model linear
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

/*********************************************************************/

TableView::TableView(QWidget *parent) : QTableView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint))
            ,this,SLOT(contextMenuRqusested(QPoint)));
    Model *M = new Model(this);
    setModel(M);
    {
     PosAction *A = actEditItem = new PosAction(this);
     A->setText(tr("Edit"));
     connect (A,SIGNAL(editItem(QModelIndex,QWidget*))
              , M, SLOT(editItem(QModelIndex,QWidget*)));
     addAction(A);
    }
    {
     PosAction *A = actNewItem = new PosAction(this);
     A->setText(tr("Add"));
     connect (A,SIGNAL(editItem(QModelIndex,QWidget*))
              , M, SLOT(newItem(QModelIndex,QWidget*)));
     addAction(A);
    }
    {
     PosAction *A = actdellItem = new PosAction(this);
     A->setText(tr("Del"));
     connect (A,SIGNAL(editItem(QModelIndex,QWidget*))
              , M, SLOT(dellItem(QModelIndex,QWidget*)));
     addAction(A);
    }
    {
     QHeaderView *H = verticalHeader();
     H->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
    {
     QHeaderView *H = horizontalHeader();
     H->setSectionResizeMode(QHeaderView::ResizeToContents);
     H->setSectionResizeMode(1,QHeaderView::Stretch);
    }
    setColumnHidden(3,true);
    setColumnHidden(4,true);

}

TableView::~TableView(){}

void TableView::contextMenuRqusested(const QPoint &p) {
    QMenu M(this);
    QModelIndex i = indexAt(p);
    if (i.isValid()) {
        actdellItem->I =i;
        actdellItem->pWidget = this;
        actEditItem->I = i;
        actEditItem->pWidget = this;
        M.addAction(actEditItem);
        if (i.data(Qt::UserRole+1).toBool()) {
            actdellItem->setText(tr("Restore"));
        } else {
            actdellItem->setText(tr("Del"));
        }
        M.addAction(actdellItem);
    }
    actNewItem->I = QModelIndex();
    actNewItem->pWidget = this;
    M.addAction(actNewItem);
    M.exec(mapToGlobal(p));

}
/*********************************************************************/

} // namespace CATALOG
} // namespace STORE
