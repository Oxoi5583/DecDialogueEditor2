#include "editor/components/start_up_popup.h"
#include "DecToolsBox/debug/messenger.h"
#include "editor/components/popup_window.h"
#include "engine/window.h"
#include "server/object_server.h"
#include "server/ui_text_bank.h"

void StartupPopup::process(){
    if(m_ran){
        return;
    }

    const std::string msg = "";

    PopupWindow* popup = ObjectServer::Ref()->queue_create<PopupWindow>();
    vec2 window_size = EngineWindow::Ref()->get_window_size();
    popup->set_content(msg);
    popup->set_size(window_size / 3.0f);
    popup->add_option(UiTextBank::Ref()->GotIt, [popup](){popup->close();});
    popup->show();

    m_ran = true;
}