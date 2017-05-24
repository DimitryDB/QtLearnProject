#include <QAction>
#include <QtDebug>
#include <Qmenu>
#include <QtSql>
#include "catalogue.h"
#include "confdialog.h"
#include "posaction.h"

namespace STORE {
namespace CATALOG {

/*********************************************************************/

Model::Model(QObject *parent) : QAbstractTableModel(parent) {

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
        "   from catalogue;               \n"
    );
    if(qry.exec()) {
        while (qry.next()) {
            ITEM::Data *D = new ITEM::Data(this,qry);
            Cat.append(D);
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
        return 0;
    }
}

int Model::columnCount(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return 6 ;
    }
    else {
        return 0;
    }
}

QVariant Model::dataDisplay(const QModelIndex &I) const {
    ITEM::Data *D = Cat[I.row()];
    switch (I.column()) {
    case 0 : return D->Code;
    case 1 : return D->Title;
    case 2 : return D->From.isValid() ?
                    D->From.toString("dd.MM.yyyy") : "";
    case 3 : return D->To.isValid() ?
                    D->To.toString("dd.MM.yyyy") : "";
    case 4 : return D->isLocal ? tr("LOCAL") : QString();
    case 5 : return D->Comment.isEmpty() ? QString() : tr("CMT") ;
    default : return QVariant();
    }
}
QVariant Model::dataFont(const QModelIndex &I) const {
    ITEM::Data *D = dataDataPointer(I);
    if (!D) return QVariant();
    QFont F;
    if (D->deleted) F.setStrikeOut(true);
    return F;
}
QVariant Model::dataForeground(const QModelIndex &I) const {
    ITEM::Data *D = dataDataPointer(I);
    if (!D) return QVariant();
    QColor Result = D->isLocal ?  QColor("blue") : QColor("black");
    if (!D->dataIsActive()) Result.setAlphaF(0.5);
    return Result;
}
QVariant Model::dataToolTip(const QModelIndex &I) const {
    ITEM::Data *D = dataDataPointer(I);
    if (!D) return QVariant();
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

ITEM::Data* Model::dataDataPointer (const QModelIndex &I) const {
    int R =  I.row();
    if (R <0 || R >= Cat.size()) return 0;
    return Cat[R];
}

QVariant Model::data(const QModelIndex &I, int role) const {
    switch(role) {
    case Qt::DisplayRole : return dataDisplay(I);
    case Qt::TextAlignmentRole : return dataTextAlignment(I);
    case Qt::ForegroundRole : return dataForeground(I);
    case Qt::FontRole : return dataFont(I);
    case Qt::ToolTipRole : return dataToolTip(I);
    //case Qt::UserRole : return QVariant(dataDataPointer(I));
    case Qt::UserRole+1 : { ITEM::Data *D = dataDataPointer(I);
                            if (!D) return false;
                            return D->deleted; }
    default : return QVariant();
    }
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal)
        return QAbstractTableModel::headerData(section,orientation,role);
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
void Model::editItem(const QModelIndex &i, QWidget *parent) {
    ITEM::Dialog dia(parent);
    ITEM::Data *D = dataDataPointer(i);
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
        endResetModel();
    } else {
        delete D;
    }
}

void Model::dellItem(const QModelIndex &i, QWidget *parent) {
    ITEM::Data *D = dataDataPointer(i);
    if (!D) return;
    ConfDialog check(parent);
    if (check.exec() == QDialog::Accepted) {
    // Assume model linear
        beginResetModel();
        if (D->id.isNull()|| !D->id.isValid()) {
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
