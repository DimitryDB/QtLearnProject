#include "pasreq.h"
#include "ui_pasreq.h"
namespace STORE {

/*********************************************************************/

pasReq::pasReq(QWidget *parent) : QFrame(parent) {
    ui.setupUi(this);

}
QString pasReq::getName() const {
    return ui.edtLogin->text().simplified();
}

QString pasReq::getPass() const {
    return ui.edtPass->text().simplified();
}
pasReq::~pasReq() {}

/*********************************************************************/

PassDialog::PassDialog(QWidget *parent) : CommonDialog(parent) {
    passFrame = new pasReq(this);
    setCentralFrame(passFrame);
}
QString PassDialog::getName() const {
    return passFrame->getName();
}

QString PassDialog::getPass() const {
    return passFrame->getPass();
}

/*********************************************************************/

} // namespace STORE
