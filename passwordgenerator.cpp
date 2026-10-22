#include "passwordgenerator.h"
#include <QRandomGenerator>

QString PasswordGenerator::generate(int length, bool includeUpper, bool includeDigits, bool includeSymbols)
{
    QString lowercase = "abcdefghijklmnopqrstuvwxyz";
    QString uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString digits    = "0123456789";
    QString symbols   = "!@#$%^&*()_+-=[]{}|;:,.<>?";

    QString pool = lowercase;
    if (includeUpper)   pool += uppercase;
    if (includeDigits)  pool += digits;
    if (includeSymbols) pool += symbols;

    QString result;
    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(pool.length());
        result.append(pool.at(index));
    }

    return result;
}