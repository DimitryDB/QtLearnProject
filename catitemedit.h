#ifndef CATITEMEDIT_H
#define CATITEMEDIT_H
#include <QFrame>

#include "dialogtpl.h"
#include "ui_catitemframe.h"

class QSqlQuery;

namespace STORE {
namespace CATALOG {
namespace ITEM {

/*********************************************************************/

class Data : public QObject {
    Q_OBJECT
public:
    Data(QObject *parent= 0) : QObject(parent) {}
    Data(QObject *parent, QSqlQuery &qry);
    // pictogramm
    QVariant    id;
    QString     Code;
    QString     Title;
    QDateTime   From;
    QDateTime   To;
    QString     Comment;
    bool        isLocal;
    Data        *pParentItem;

};

/*********************************************************************/

class Frame : public QFrame {
    Q_OBJECT
public:
    Frame(QWidget *parent = 0);
    virtual ~Frame();
    void setDataBlock(Data *D) { Block = D; load(); }
private:
    Data *Block;
    Ui::CatItemFrame ui;
public slots:
    void isGood(bool *pOK);
    void load();
    bool save();
signals:
    void error_message(const QString &);
};

/*********************************************************************/

class Dialog : public CommonDialog {
    Q_OBJECT
public:
    Dialog(QWidget *parent = 0);
    virtual ~Dialog();
    void setDataBlock(Data *D) { pFrame->setDataBlock(D); }
private:
    Frame *pFrame;
};

/*********************************************************************/

} // namespace ITEM
} // namespace CATALOG
} // namespace STORE

#endif // CATITEMEDIT_H
