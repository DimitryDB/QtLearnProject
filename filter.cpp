#include "filter.h"

namespace STORE {

Filter::Filter(QWidget *parent) : QFrame(parent) {
    ui.setupUi(this);
    ui.btnFilter->setDefaultAction(ui.actionApplyFilter);
    ui.btnRestFilter->setDefaultAction(ui.actionResetFilter);
    connect(ui.actionApplyFilter, SIGNAL(triggered()), this ,SLOT(apply_filter_triggered()));
    connect(ui.actionResetFilter, SIGNAL(triggered()), this ,SLOT(reset_filter_triggered()));
}

QVariant Filter::author() const {
    QString T= ui.edtAuthor->text().simplified();
    return T.isEmpty() ? QVariant() : T ;

}
QVariant Filter::title() const {
    QString T= ui.edtTitle->text().simplified();
    return T.isEmpty() ? QVariant() : T ;
}
QVariant Filter::year() const {
    QString T= ui.edtYear->text().simplified();
    if (T.isEmpty())
        return QVariant();
    bool ok;
    int result = T.toInt(&ok);
    if (ok)
        return result;
    else
        return QVariant();
}
void Filter::apply_filter_triggered() {
    emit apply_filter(this);
}
void Filter::reset_filter_triggered() {
    ui.edtTitle->clear();
    ui.edtYear->clear();
    ui.edtYear->clear();

    emit apply_filter(this);
}

} // namespace STORE
