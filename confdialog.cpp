#include "confdialog.h"
namespace STORE {


ConfDialog::ConfDialog(QWidget *parent) :  CommonDialog(parent)
{
    connect(this,SIGNAL(error_message(const QString &)),getButtonsPtr(),SLOT(error_message(const QString &)));
    emit(error_message(tr("Are you Sure")));
}

} // namespace STORE
