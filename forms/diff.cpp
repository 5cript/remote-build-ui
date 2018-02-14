#include "diff.hpp"
#include "remote-build/lib_remote_build.hpp"

#include <nana/gui/place.hpp>
#include <nana/gui/msgbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include <iostream>

namespace Forms
{
//#####################################################################################################################
    struct DiffWindowImpl
    {
        DiffWindowImpl(DiffWindow* form);

        // Layout
        nana::place layout;

        // Widgets
        nana::listbox diffBox;
        nana::button abort;
        nana::button upload;
        nana::button synchronize;
        nana::button all;
        nana::textbox log;

        // Other
        RemoteBuild::Context ctx;
    };
//---------------------------------------------------------------------------------------------------------------------
    DiffWindowImpl::DiffWindowImpl(DiffWindow* form)
        : layout{*form}
        , diffBox{*form}
        , abort{*form, "Abort"}
        , upload{*form, "Upload"}
        , synchronize{*form, "Synchronize"}
        , all{*form, "Upload and Build"}
        , log{*form}
        , ctx{}
    {
        log.editable(false);
    }
//#####################################################################################################################
    DiffWindow::DiffWindow(nana::window owner, std::string const& configPath)
        : nana::form{owner, nana::API::make_center(owner, 800, 600)}
        , elements_{new DiffWindowImpl(this)}
    {
        elements_->ctx = RemoteBuild::makeContext(configPath, false);

        setupListbox();
        setLayout();
        setupEvents();
    }
//---------------------------------------------------------------------------------------------------------------------
    DiffWindow::~DiffWindow()
    {
        if (builderThread_.joinable())
            builderThread_.join();
    }
//---------------------------------------------------------------------------------------------------------------------
    void DiffWindow::setLayout()
    {
        auto& layout = elements_->layout;

        layout.field("DiffBox") << elements_->diffBox;
        layout.field("Buttons") << elements_->abort << elements_->upload << elements_->synchronize << elements_->all;
        layout.field("Log") << elements_->log;

        layout.div(layoutString);
        layout.collocate();
    }
//---------------------------------------------------------------------------------------------------------------------
    void DiffWindow::setButtonEnabled(bool enabled)
    {
        elements_->abort.enabled(enabled);
        elements_->upload.enabled(enabled);
        elements_->synchronize.enabled(enabled);
        elements_->all.enabled(enabled);
    }
//---------------------------------------------------------------------------------------------------------------------
    void DiffWindow::setupEvents()
    {
        elements_->abort.events().click([this](auto const&) {close();});
        elements_->upload.events().click([this](auto const&)
        {
            RemoteBuild::makeDirectories(elements_->ctx, false);
            RemoteBuild::synchronize(elements_->ctx, false, false, false);
        });
        elements_->synchronize.events().click([this](auto const&)
        {
            RemoteBuild::synchronize(elements_->ctx, false, true, false);
        });
        elements_->all.events().click([this](auto const&)
        {
            RemoteBuild::prebuildSteps(elements_->ctx);
            RemoteBuild::makeDirectories(elements_->ctx, false);
            RemoteBuild::synchronize(elements_->ctx, false, false, false);
            RemoteBuild::build(elements_->ctx);
            setButtonEnabled(false);
            if (builderThread_.joinable())
                builderThread_.join();
            builderThread_ = std::thread([this](){
                RemoteBuild::waitForBuild(elements_->ctx, [this](std::string const& str){
                    std::cout << str;
                    elements_->log.append(str, false);
                }, 10000000);
                RemoteBuild::postbuildSteps(elements_->ctx);
                setButtonEnabled(true);
            });
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void DiffWindow::setupListbox()
    {
        elements_->diffBox.append_header("Filename", 550);

        elements_->diffBox.append("Uploads");
        elements_->diffBox.append("Deletions");
        elements_->diffBox.at(2).expanded(false);

        auto difference = RemoteBuild::getFileDifference(elements_->ctx);

        for (auto const& up : difference.clientUpload)
            elements_->diffBox.at(1).append(up);
        for (auto const& removal : difference.serverOnly)
            elements_->diffBox.at(2).append(removal);
    }
//#####################################################################################################################
}
