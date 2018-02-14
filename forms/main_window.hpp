#pragma once

#include <nana/gui/widgets/form.hpp>
#include <memory>

namespace Forms
{
    struct MainWindowImpl;

    class MainWindow : public nana::form
    {
    public:
        constexpr static const char* layoutString =
            #include "layouts/MainWindow.layout"
        ;

    public:
        MainWindow();
        ~MainWindow();

    private:
        void setLayout();
        void setupEvents();
        void setupListbox();

    private:
        std::unique_ptr <MainWindowImpl> elements_;
    };
}
