#ifndef PASREQ_H
#define PASREQ_H

#include <QFrame>
#include <QString>
#include "dialogtpl.h"
#include "ui_pasreq.h"

namespace STORE {

/*********************************************************************/

class pasReq : public QFrame
{
    Q_OBJECT
public:
    pasReq(QWidget *parent = 0);
    virtual ~pasReq();
    QString getName() const;
    QString getPass() const;

private:
    Ui::pasReq ui;
// slots and signals added to avoid  compilation warnings
public slots:
    void isGood(bool *pOK) {pOK;}
    void load() {}
    bool save() {return false;}
signals:
    void error_message(const QString &);
};

/*********************************************************************/

class PassDialog : public CommonDialog {
    Q_OBJECT
public:
    PassDialog(QWidget *parent = 0);
    virtual ~PassDialog() {}
    QString getName() const;
    QString getPass() const;
private:
    pasReq *passFrame;

};

/*********************************************************************/

} // namespace STORE
#endif // PASREQ_H
