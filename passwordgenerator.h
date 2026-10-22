#ifndef PASSWORDGENERATOR_H
#define PASSWORDGENERATOR_H

#include <QString>

class PasswordGenerator
{
public:
    // Static method allowing call without instantiating objects
    static QString generate(int length = 16,
                            bool includeUpper = true,
                            bool includeDigits = true,
                            bool includeSymbols = true);
};

#endif // PASSWORDGENERATOR_H