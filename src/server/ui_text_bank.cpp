#include "server/ui_text_bank.h"
#include "DecToolsBox/debug/messenger.h"
#include "config/config_loader.h"
#include "engine/font_loader.h"
#include "imgui/imgui.h"
#include <cstddef>

UiLocale::UiLocale(size_t p_id, const char* p_name)
    : id(p_id), name(p_name){}
UiLocale::UiLocale(const UiLocale& other)
    : id(other.id), name(other.name){}
UiLocale& UiLocale::operator=(const UiLocale& other) {
    if (this != &other) {
        id   = other.id;
        name = other.name;
    }
    return *this;
}
UiLocale::operator size_t() const {
    return id;
}
size_t UiLocale::get_locale_id() const {
    return id;
}

const char* UiLocale::get_name() const {
    return name;
}


const char* UiText::get() const{
    switch (UiTextBank::Ref()->get_locale()->get_locale_id()) {
        case UI_LOCALE_ID_EN:      return en;
        case UI_LOCALE_ID_ZH:      return zh;
        case UI_LOCALE_ID_JP:      return jp;
        case UI_LOCALE_ID_CN:      return cn;
        case UI_LOCALE_ID_KO:      return ko;
        case UI_LOCALE_ID_FR:      return fr;
        case UI_LOCALE_ID_DE:      return de;
        case UI_LOCALE_ID_ES:      return es;
        case UI_LOCALE_ID_ID:      return id;
        default:                   return en;
    }
}

UiText::operator std::string() const{
    return get();
}
UiText::operator const char*() const{
    return get();
}

const UiLocale* UiTextBank::get_locale(){
    return m_current_locale;
}
void UiTextBank::set_locale(size_t p_id){
    for(auto locale : all_locales){
        if(locale->get_locale_id() == p_id){
            m_current_locale = locale;
            refresh_locale_setting();
            return;
        }
    }
    m_current_locale = &UI_LOCALE_EN;
    refresh_locale_setting();
}
void UiTextBank::refresh_locale_setting(){
    int locale_id = m_current_locale->get_locale_id();
    ConfigLoader::Ref()->set_config("UsingLocale", locale_id);
    ConfigLoader::Ref()->save();
}

void UiTextBank::init(){
    int locale_id = 0;
    ConfigLoader::Ref()->get_config("UsingLocale", locale_id);
    set_locale(locale_id);
}

UiTextBank::UiTextBank(){
}

UiTextBank::~UiTextBank(){
    int locale_id = m_current_locale->get_locale_id();
    ConfigLoader::Ref()->set_config("UsingLocale", locale_id);
    ConfigLoader::Ref()->save();
}