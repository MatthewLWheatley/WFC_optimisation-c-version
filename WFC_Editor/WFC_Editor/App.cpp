#include "App.h"
#include "Program.h"



int main()
{
    Program* program = new Program();

    if (program->Init() != 0) {
        return -1;
    }

    program->Loop();
    program->CleanUp();

    return 0;
}
