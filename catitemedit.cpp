#include "catitemedit.h"

namespace STORE {
namespace CATALOG {
namespace ITEM {

/*********************************************************************/

Frame::Frame(QWidget *parent) : QFrame(parent) {
    ui.setupUi(this);
    Block = 0;
}
Frame::~Frame() {}
/** TODO : Check data */
void Frame::isGood(bool *pOK) {
    *pOK = true;
}
void Frame::load() {
    if (!Block) return;
    ui.edtCode->setText(Block->Code);
    ui.edtTitle->setText(Block->Title);
    ui.cbxLocal->setChecked(Block->isLocal);
    ui.edtComment->setText(Block->Comment);
    ui.cbxFromEnabled->setChecked(Block->From.isValid());
    ui.edtFrom->setDateTime(Block->From);
    ui.cbxToEnabled->setChecked(Block->To.isValid());
    ui.edtTo->setDateTime(Block->To);
}

bool Frame::save(){
    if (!Block) return false;
    Block->Code = ui.edtCode->text().simplified();
    Block->Title = ui.edtTitle->text().simplified();
    Block->isLocal = ui.cbxLocal->isChecked();
    Block->Comment = ui.edtComment->toPlainText().trimmed();
    if (ui.cbxFromEnabled->isChecked())
        Block->From = ui.edtFrom->dateTime();
    else
        Block->From = QDateTime();
    if (ui.cbxToEnabled->isChecked())
        Block->To = ui.edtTo->dateTime();
    else
        Block->To = QDateTime();
    return true;
}

/*********************************************************************/

Dialog::Dialog(QWidget *parent) : CommonDialog(parent ){
    {
     Frame *F = pFrame = new Frame(this);
     setCentralFrame(F);
    }
}

Dialog::~Dialog() {}

/*********************************************************************/

} // namespace ITEM
} // namespace CATALOG
} // namespace STORE
