#include "main_window.hpp"
#include "diff.hpp"

#include <nana/gui/place.hpp>
#include <nana/gui/msgbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <iostream>

namespace Forms
{
//#####################################################################################################################
    struct MainWindowImpl
    {
        MainWindowImpl(MainWindow*);

        // Layout
        nana::place layout;

        // Widgets
        nana::label projectListLabel;
        nana::listbox projects;
        nana::button runAll;
    };
//---------------------------------------------------------------------------------------------------------------------
    MainWindowImpl::MainWindowImpl(MainWindow* form)
        : layout{*form}
        , projectListLabel{*form, "Select from one of the following projects"}
        , projects{*form}
        , runAll{*form, "Synchronize and Build"}
    {
    }
//#####################################################################################################################
    MainWindow::MainWindow()
        : nana::form{nana::API::make_center(600, 400)}
        , elements_{new MainWindowImpl(this)}
    {
        setupListbox();
        setLayout();
        setupEvents();
    }
//---------------------------------------------------------------------------------------------------------------------
    MainWindow::~MainWindow() = default;
//---------------------------------------------------------------------------------------------------------------------
    void MainWindow::setLayout()
    {
        auto& layout = elements_->layout;

        layout.field("ProjectListLabel") << elements_->projectListLabel;
        layout.field("ProjectList") << elements_->projects;
        layout.field("Buttons") << elements_->runAll;

        layout.div(layoutString);
        layout.collocate();
    }
//---------------------------------------------------------------------------------------------------------------------
    void MainWindow::setupListbox()
    {
        elements_->projects.append_header("Project ID", 400);
        elements_->projects.enable_single(true, false);
        for (auto const& entry : boost::make_iterator_range(boost::filesystem::directory_iterator("."), {}))
        {
            if (
                boost::filesystem::exists(entry.status()) &&
                boost::filesystem::is_directory(entry.path()) &&
                boost::filesystem::exists(entry.path() / "config.json")
            )
            {
                elements_->projects.at(0).append(entry.path().filename().string());
            }
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void MainWindow::setupEvents()
    {
        elements_->runAll.events().click([this](auto const& event)
        {
            auto selected = elements_->projects.selected();
            if (selected.empty())
            {
                nana::msgbox box(*this, "Missing Selection", nana::msgbox::ok);
                box.icon(nana::msgbox::icon_error) << "Please select a project first.";
                box.show();
                return;
            }
            else if (selected.size() > 1)
            {
                nana::msgbox box(*this, "Multiselect invalid.", nana::msgbox::ok);
                box.icon(nana::msgbox::icon_error) << "Do not select more than one project (not implemented)";
                box.show();
                return;
            }
            else
            {
                auto elem = boost::filesystem::path{elements_->projects.at(0).at(selected.front().item).text(0)} / "config.json";
                std::cout << elem << "\n";
                DiffWindow diffy{*this, elem.string()};
                nana::API::modal_window(diffy);
                diffy.show();
            }
        });
    }
//#####################################################################################################################
}
