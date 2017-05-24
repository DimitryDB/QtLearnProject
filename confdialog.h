#ifndef CONFDIALOG_H
#define CONFDIALOG_H
#include "dialogtpl.h"

namespace STORE {

class ConfDialog : public CommonDialog {
    Q_OBJECT
public:
    ConfDialog(QWidget *parent = 0);
    virtual ~ConfDialog() {}
signals:
    void error_message(const QString &);
};

} // namespace STORE
#endif // CONFDIALOG_H
