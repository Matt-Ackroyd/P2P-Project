#include "CppInterface.h"

void CppInterface::test(QString Message) {
    qDebug(qUtf8Printable(Message));
}