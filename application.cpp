#include <QtSql>
#include "application.h"

namespace STORE {


/*********************************************************************/

//
// Делаем проверку в конструкторе апликейшена?
// если да то при нажати кансел в диалоге ввода имени и пароля
// выходим из программы через эксепшн?
// при неправильном вводе имени и пароля каким ообразом мы это узнаем?
// через возвращаемый буль и lastError()? и опять же выходим через эксепшн
//

Application::Application(int argc , char *argv[]) : QApplication(argc , argv) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("store");
    // TO DO Make window to ask name and password
    while(true) {
        db.setUserName("admin");
        db.setPassword("1");
        if(!db.open("admin","1")) {
            QSqlError err =db.lastError();
            qCritical() << err.driverText();
            qCritical() << err.databaseText();
            continue;
        }
        break;
    }


}
Application::~Application() {}

/*********************************************************************/

} // namespace STORE
