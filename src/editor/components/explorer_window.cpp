#include "editor/components/explorer_window.h"
#include "SDL3/SDL_dialog.h"
#include <DecToolsBox/abstract/singleton.h>
#include <DecToolsBox/debug/messenger.h>
#include <engine/window.h>
#include <server/file_server.h>
#include <server/object_server.h>
#include <server/ui_text_bank.h>
#include <theme/theme_loader.h>

ExplorerWindow::ExplorerWindow(){
    BIND_CLASS(ExplorerWindow);
}
ExplorerWindow::~ExplorerWindow(){

}

void SDLCALL ExplorerWindow::on_open_file(void* userdata, const char* const* filelist, int filter){
    ExplorerWindow* window = (ExplorerWindow*)userdata;

    if (*filelist == NULL) {
        DEBUG_MSG("User canceled the dialog.");
        window->m_state = ExplorerWindow::State::CLOSE;
        return;
    }

    window->m_selected_files.clear();
    while (*filelist) {
        window->m_selected_files.push_back(*filelist);
        ++filelist;
    }
    window->m_state = ExplorerWindow::State::DONE;
}

void ExplorerWindow::m_refresh_sdl_filters(){
    m_sdl_filters.clear();

    for(FilterOption& option : m_filter_options){
        m_sdl_filters.push_back(m_filter_map[option]);
    }
}

void ExplorerWindow::m_open_file_dialog(){
    m_refresh_sdl_filters();

    SDL_DialogFileFilter* filters = m_sdl_filters.data();
    switch (m_mode) {
        case Mode::FILE:
            SDL_ShowOpenFileDialog(ExplorerWindow::on_open_file, this, m_window, filters, SDL_arraysize(filters), NULL, m_allow_multi_select);
            break;
        case Mode::FOLDER:
            SDL_ShowOpenFolderDialog(ExplorerWindow::on_open_file, this, m_window, NULL, m_allow_multi_select);
            break;
    }
}

void ExplorerWindow::ready(){
    if(!m_window){
        ERROR_MSG("Explorer Window need to be created from Engine Window.");
        return;
    }

    m_refresh_sdl_filters();
    m_selected_files.clear();
    m_open_file_dialog();
}
void ExplorerWindow::pre_process(){

}
void ExplorerWindow::process(){

}
void ExplorerWindow::post_process(){
    if(is_finished()){
        auto ret = get_result();
        for(std::string r : ret){
            DEBUG_MSG(r);
        }
        this->close();
    }
}
void ExplorerWindow::draw(){

}

void ExplorerWindow::close(){
    this->queue_free();
}

void ExplorerWindow::set_sdl_window(SDL_Window* p_window){
    m_window = p_window;
}
void ExplorerWindow::set_mode(Mode p_mode){
    m_mode = p_mode;
}
void ExplorerWindow::set_allow_multi_select(bool p_option){
    m_allow_multi_select = p_option;
}
void ExplorerWindow::add_filter(FilterOption p_option){
    m_filter_options.push_back(p_option);
}

bool ExplorerWindow::is_finished(){
    return m_state == State::CLOSE || m_state == State::DONE;
}
bool ExplorerWindow::have_result(){
    return m_state == State::DONE;
}
std::vector<std::string> ExplorerWindow::get_result(){
    return m_selected_files;
}