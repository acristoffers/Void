#include <iostream>

#include "Store.h"

int main(int, char **)
{
    QString password = "haha";
    Store   store("/Users/Alan/void", password, true);

    return 0;
}
