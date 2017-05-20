#include <QAction>
#include <Qmenu>
#include "catalogue.h"
#include "posaction.h"

namespace STORE {
namespace CATALOG {

/*********************************************************************/

Model::Model(QObject *parent) : QAbstractTableModel(parent) {
    //TO DO Replace test catalog
    {
     ITEM::Data *D = new ITEM::Data(this);
     D->Code    = "1111";
     D->Title   = "Physics";
     D->From    = QDateTime::currentDateTime();
     D->To      = QDateTime();
     D->isLocal = false;
     Cat.append(D);
    }
    {
     ITEM::Data *D = new ITEM::Data(this);
     D->Code    = "2222";
     D->Title   = "Math";
     D->From    = QDateTime::currentDateTime();
     D->To      = QDateTime();
     D->isLocal = false;
     Cat.append(D);
    }
    {
     ITEM::Data *D = new ITEM::Data(this);
     D->Code    = "3333";
     D->Title   = "Biology";
     D->From    = QDateTime::currentDateTime();
     D->To      = QDateTime();
     D->isLocal = false;
     Cat.append(D);
    }
    {
     ITEM::Data *D = new ITEM::Data(this);
     D->Code    = "4444";
     D->Title   = "Valeo";
     D->From    = QDateTime::currentDateTime();
     D->To      = QDateTime();
     D->isLocal = true;
     D ->Comment = "Trash";
     Cat.append(D);
    }
    {
     ITEM::Data *D = new ITEM::Data(this);
     D->Code    = "5555";
     D->Title   = "Literature";
     D->From    = QDateTime::currentDateTime();
     D->To      = QDateTime();
     D->isLocal = true;
     Cat.append(D);
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
    case 2 : return D->From;
    case 3 : return D->To;
    case 4 : return D->isLocal ? tr("LOCAL") : QString();
    case 5 : return D->Comment.isEmpty() ? QString() : tr("CMT") ;
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

/*********************************************************************/

TableView::TableView(QWidget *parent) : QTableView(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenuRqusested(QPoint)));
    Model *M = new Model(this);
    setModel(M);
    {
     PosAction *A = actEditItem = new PosAction(this);
     A->setText(tr("Edit"));
     connect (A,SIGNAL(editItem(QModelIndex,QWidget*)), M, SLOT(editItem(QModelIndex,QWidget*)));
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

}

TableView::~TableView(){}

void TableView::contextMenuRqusested(const QPoint &p) {
    QMenu M(this);
    QModelIndex i = indexAt(p);
    if (i.isValid()) {
        actEditItem->I = i;
        actEditItem->pWidget = this;
        M.addAction(actEditItem);
        M.exec(mapToGlobal(p));
    }

}
/*********************************************************************/

} // namespace CATALOG
} // namespace STORE
