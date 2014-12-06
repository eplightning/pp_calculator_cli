#include "core/application.h"

#include <cstring>

int main(int argc, char *argv[])
{
    bool verbose = true;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0)
            verbose = false;
    }

    Calculator::Application app(verbose);

    return app.execute();
}
