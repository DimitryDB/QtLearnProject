#ifndef FILTER_H
#define FILTER_H
#include <QFrame>
#include "ui_filter.h"

namespace STORE {

class Filter : public QFrame {
    Q_OBJECT
    Q_PROPERTY(QVariant  author READ author)
    Q_PROPERTY(QVariant  title  READ title)
    Q_PROPERTY(QVariant  year   READ year)
public:
    Filter(QWidget *parent = 0);
    virtual ~Filter() {}
    QVariant author() const;
    QVariant title() const;
    QVariant year() const;

private:
    Ui::Filter ui;
private slots:
    void reset_filter_triggered();
    void apply_filter_triggered();
signals:
    void apply_filter(QObject *pFilter);
};

} // namespace STORE

#endif // FILTER_H
