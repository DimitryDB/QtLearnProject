#include <QtSql>
#include "application.h"
#include "dialogtpl.h"
#include "pasreq.h"
#include "confdialog.h"

namespace STORE {


/*********************************************************************/

Application::Application(int argc , char *argv[]) : QApplication(argc , argv) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("store");
    db.open("admin","1");
//    while(true) {
//        PassDialog check;
//        if (check.exec() == QDialog::Accepted) {
//            QString name = check.getName();
//            QString pass = check.getPass();
//            db.setUserName(name);
//            db.setPassword(pass);
//            if(!db.open(name,pass)) {
//                ConfDialog tmp;
//                QSqlError err =db.lastError();
//                tmp.error_message(err.databaseText());
//                tmp.exec();
//                continue;
//            }
//        }
//        break;
//    }
}
Application::~Application() {}

/*********************************************************************/

} // namespace STORE
