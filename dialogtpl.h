#ifndef DIALOGTPL_H
#define DIALOGTPL_H
#include <QFrame>
#include <QDialog>
#include "ui_buttonsFrame.h"
namespace STORE {

/*********************************************************************/

class ButtonsDlg : public QFrame {
    Q_OBJECT
public:
    ButtonsDlg(QWidget *parent = 0);
    virtual ~ButtonsDlg();
private:
    Ui::ButtonsFrame ui;
public slots :
    void error_message(const QString &);
signals:
    void accepted();
    void rejected();
};

/*********************************************************************/

class CommonDialog : public QDialog {
    Q_OBJECT

public:
    CommonDialog(QWidget *parent=0) ;
    virtual ~CommonDialog();

protected:
    void setCentralFrame(QFrame *central);
    ButtonsDlg * getButtonsPtr() { return pButtons;}

private:
    ButtonsDlg *pButtons;
    QFrame *pFrame;
private slots:
    void accept_pressed();

signals:
    void check_data(bool *pOk);
    void save();
};

/*********************************************************************/

} // namespace STORE
#endif // DIALOGTPL_H
