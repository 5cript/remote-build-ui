#pragma once

#include <nana/gui/widgets/form.hpp>
#include <memory>
#include <string>
#include <thread>

namespace Forms
{
    struct DiffWindowImpl;

    class DiffWindow : public nana::form
    {
    public:
        constexpr static const char* layoutString =
            #include "layouts/DiffWindow.layout"
        ;

    public:
        DiffWindow(nana::window owner, std::string const& configPath);
        ~DiffWindow();

    private:
        void setLayout();
        void setupEvents();
        void setupListbox();
        void setButtonEnabled(bool enabled);

    private:
        std::unique_ptr <DiffWindowImpl> elements_;
        std::thread builderThread_;
    };
}
