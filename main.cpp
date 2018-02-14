#include "forms/main_window.hpp"

#include <iostream>
#include <nana/gui.hpp>

int main()
{
    Forms::MainWindow window{};
    window.show();
    nana::exec();
    return 0;
}
