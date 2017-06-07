#include "books.h"

namespace STORE {
namespace BOOKS {

/*********************************************************************/

    Model::Model(QObject *parent) : QSqlQueryModel(parent) {

    }

    Model::~Model() {}

/*********************************************************************/

    View::View (QWidget *parent) : QTableView (parent) {

    }

    View::~View() {}

/*********************************************************************/
} // namespace BOOKS
} // namespace STORE
