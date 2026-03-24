#pragma once

#include "SDL3/SDL_video.h"
#include "server/object_base.h"
#include <DecToolsBox/abstract/singleton.h>
#include <imgui.h>
#include <map>
#include <string>
#include <vector>
#include "SDL3/SDL_dialog.h"
#include "SDL3/SDL_log.h"


struct FilterOption{
    std::string name;
    std::vector<std::string> exts; 
};

class ExplorerWindow : public ObjectBase{
public:
    enum class Mode{
        FILE,
        FOLDER,
    };
    enum class FilterOption{
        TEXT_FILES,
        IMAGES,
        AUDIO,
        VIDEO,
        DOCUMENTS,
        CODE_FILES,
        ARCHIVES,
        EXECUTABLES,
        FONTS,
        MODELS,
        MATERIALS,
        SCENES,
        PREFABS,
        SHADERS,
        DEC_DIALOGUE,
        ALL_FILES,
    };
    enum class State{
        IDLE,
        DONE,
        CLOSE,
    };
private:
    static void SDLCALL on_open_file(void* userdata, const char* const* filelist, int filter);
    SDL_Window* m_window;
    std::vector<SDL_DialogFileFilter> m_sdl_filters;
    bool m_allow_multi_select = false;
    void m_refresh_sdl_filters();

    std::vector<FilterOption> m_filter_options;
    std::vector<std::string> m_selected_files;

    Mode m_mode = Mode::FILE;

    std::map<FilterOption, SDL_DialogFileFilter> m_filter_map = {
        {FilterOption::TEXT_FILES, {"Text files", "txt;md;log"}},
        {FilterOption::IMAGES, {"Images", "png;jpg;jpeg;bmp"}},
        {FilterOption::AUDIO, {"Audio", "wav;mp3;flac"}},
        {FilterOption::VIDEO, {"Video", "mp4;avi;mkv"}},
        {FilterOption::DOCUMENTS, {"Documents", "pdf;docx;xlsx;pptx"}},
        {FilterOption::CODE_FILES, {"Code files", "cpp;h;py;js;html;css"}},
        {FilterOption::ARCHIVES, {"Archives", "zip;rar;7z;tar;gz"}},
        {FilterOption::EXECUTABLES, {"Executables", "exe;bat;sh;app"}},
        {FilterOption::FONTS, {"Fonts", "ttf;otf;woff;woff2"}},
        {FilterOption::MODELS, {"3D Models", "obj;fbx;dae;3ds"}},
        {FilterOption::MATERIALS, {"Materials", "mat;mtl"}},
        {FilterOption::SCENES, {"Scenes", "scene;scn"}},
        {FilterOption::PREFABS, {"Prefabs", "prefab"}},
        {FilterOption::SHADERS, {"Shaders", "vert;frag;geom;comp"}},
        {FilterOption::DEC_DIALOGUE, {"DecDialogue", "ddlg"}},
        {FilterOption::ALL_FILES, {"All files", "*"}},
    };

    void m_open_file_dialog();

    State m_state = State::IDLE;
public:
    ExplorerWindow();
    ~ExplorerWindow();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    void close();

    void set_sdl_window(SDL_Window* p_window);
    void set_mode(Mode p_mode);
    void set_allow_multi_select(bool p_option);
    void add_filter(FilterOption p_option);

    bool is_finished();
    bool have_result();
    std::vector<std::string> get_result();
};
