#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "engine/font_loader.h"

#include <array>
#include <cstddef>
#include <string>

typedef EngineFontLoader::FontId FontId;

/* ============================================================
 * UiText
 * ============================================================ */
struct UiText{
private:
    const char* en;
    const char* zh;
    const char* jp;
    const char* cn;
    const char* ko;
    const char* fr;
    const char* de;
    const char* es;
    const char* id;

public:
    UiText(
        const char* p_en,
        const char* p_zh,
        const char* p_jp,
        const char* p_cn,
        const char* p_ko,
        const char* p_fr,
        const char* p_de,
        const char* p_es,
        const char* p_id
    )
    : en(p_en), zh(p_zh), jp(p_jp),
      cn(p_cn), ko(p_ko),
      fr(p_fr), de(p_de), es(p_es),
      id(p_id) {}

    ~UiText() = default;

    const char* get() const;
    operator std::string() const;
    operator const char*() const;
};

/* ============================================================
 * UiLocale
 * ============================================================ */
struct UiLocale {
private:
    size_t id;
    const char* name;

    FontId big_font_id;
    FontId middle_font_id;
    FontId small_font_id;

public:
    UiLocale(
        size_t p_id,
        const char* p_name,
        FontId p_font_id1,
        FontId p_font_id2,
        FontId p_font_id3
    );

    UiLocale(const UiLocale& other);
    UiLocale& operator=(const UiLocale& other);
    ~UiLocale() = default;

    operator size_t() const;

    struct FontIds{
        FontId big;
        FontId middle;
        FontId small;
    };

    FontIds get_font_id() const;

    size_t get_id() const;
    const char* get_name() const;
};

/* ============================================================
 * Locale IDs
 * ============================================================ */
#define UI_LOCALE_ID_EN      0
#define UI_LOCALE_ID_ZH      1
#define UI_LOCALE_ID_JP      2
#define UI_LOCALE_ID_CN      3
#define UI_LOCALE_ID_KO      4
#define UI_LOCALE_ID_FR      5
#define UI_LOCALE_ID_DE      6
#define UI_LOCALE_ID_ES      7
#define UI_LOCALE_ID_ID      8

/* ============================================================
 * Locale Definitions
 * ============================================================ */
const UiLocale UI_LOCALE_EN = {
    UI_LOCALE_ID_EN, "English",
    EngineFontLoader::UI_FONT_BIG_EN,
    EngineFontLoader::UI_FONT_MIDDLE_EN,
    EngineFontLoader::UI_FONT_SMALL_EN
};

const UiLocale UI_LOCALE_ZH = {
    UI_LOCALE_ID_ZH, "繁體中文",
    EngineFontLoader::UI_FONT_BIG_ZH,
    EngineFontLoader::UI_FONT_MIDDLE_ZH,
    EngineFontLoader::UI_FONT_SMALL_ZH
};

const UiLocale UI_LOCALE_JP = {
    UI_LOCALE_ID_JP, "日本語",
    EngineFontLoader::UI_FONT_BIG_JP,
    EngineFontLoader::UI_FONT_MIDDLE_JP,
    EngineFontLoader::UI_FONT_SMALL_JP
};

const UiLocale UI_LOCALE_CN = {
    UI_LOCALE_ID_CN, "简体中文",
    EngineFontLoader::UI_FONT_BIG_CN,
    EngineFontLoader::UI_FONT_MIDDLE_CN,
    EngineFontLoader::UI_FONT_SMALL_CN
};

const UiLocale UI_LOCALE_KO = {
    UI_LOCALE_ID_KO, "한국어",
    EngineFontLoader::UI_FONT_BIG_KO,
    EngineFontLoader::UI_FONT_MIDDLE_KO,
    EngineFontLoader::UI_FONT_SMALL_KO
};

const UiLocale UI_LOCALE_FR = {
    UI_LOCALE_ID_FR, "Français",
    EngineFontLoader::UI_FONT_BIG_EN,
    EngineFontLoader::UI_FONT_MIDDLE_EN,
    EngineFontLoader::UI_FONT_SMALL_EN
};

const UiLocale UI_LOCALE_DE = {
    UI_LOCALE_ID_DE, "Deutsch",
    EngineFontLoader::UI_FONT_BIG_EN,
    EngineFontLoader::UI_FONT_MIDDLE_EN,
    EngineFontLoader::UI_FONT_SMALL_EN
};

const UiLocale UI_LOCALE_ES = {
    UI_LOCALE_ID_ES, "Español",
    EngineFontLoader::UI_FONT_BIG_EN,
    EngineFontLoader::UI_FONT_MIDDLE_EN,
    EngineFontLoader::UI_FONT_SMALL_EN
};

const UiLocale UI_LOCALE_ID = {
    UI_LOCALE_ID_ID, "Bahasa Indonesia",
    EngineFontLoader::UI_FONT_BIG_EN,
    EngineFontLoader::UI_FONT_MIDDLE_EN,
    EngineFontLoader::UI_FONT_SMALL_EN
};

/* ============================================================
 * UiTextBank
 * ============================================================ */
class UiTextBank : public Singleton<UiTextBank>{
private:
    const UiLocale* m_current_locale = &UI_LOCALE_JP;

public:
    UiTextBank();
    ~UiTextBank();

    const UiLocale* get_locale();
    void set_locale(size_t p_id);
    void refresh_locale();
    void init();

    std::array<const UiLocale*, 9> all_locales = {
        &UI_LOCALE_EN,
        &UI_LOCALE_ZH,
        &UI_LOCALE_JP,
        &UI_LOCALE_CN,
        &UI_LOCALE_KO,
        &UI_LOCALE_FR,
        &UI_LOCALE_DE,
        &UI_LOCALE_ES,
        &UI_LOCALE_ID,
    };

    /* ---------------- UI Texts ---------------- */

    UiText File = {
        "File", "檔案", "ファイル", "档案", "파일",
        "Fichier", "Datei", "Archivo", "Berkas"
    };

    UiText New = {
        "New", "新增", "新規", "新建", "새로 만들기",
        "Nouveau", "Neu", "Nuevo", "Baru"
    };

    UiText Open = {
        "Open", "開啟", "開く", "开启", "열기",
        "Ouvrir", "Öffnen", "Abrir", "Buka"
    };

    UiText Edit = {
        "Edit", "編輯", "編集", "编辑", "편집",
        "Éditer", "Bearbeiten", "Editar", "Edit"
    };

    UiText Cut = {
        "Cut", "剪下", "切り取り", "剪切", "잘라내기",
        "Couper", "Ausschneiden", "Cortar", "Potong"
    };

    UiText Copy = {
        "Copy", "複製", "コピー", "复制", "복사",
        "Copier", "Kopieren", "Copiar", "Salin"
    };

    UiText Themes = {
        "Themes", "主題", "テーマ", "主题", "테마",
        "Thèmes", "Themen", "Temas", "Tema"
    };

    UiText Language = {
        "Language", "語言", "言語", "语言", "언어",
        "Langue", "Sprache", "Idioma", "Bahasa"
    };

    UiText GoTo = {
        "Go To...", "前往…", "移動…", "前往…", "이동…",
        "Aller à…", "Gehe zu…", "Ir a…", "Pergi ke…"
    };
    
    UiText GoToContent = {
        "Please enter the target block id:\nExample: #AA, 10",
        "請輸入目標區塊 ID：\n例如：#AA、10",
        "対象のブロック ID を入力してください：\n例：#AA、10",
        "请输入目标区块 ID：\n例如：#AA、10",
        "대상 블록 ID를 입력하세요:\n예: #AA, 10",
        "Veuillez saisir l’identifiant du bloc cible :\nExemple : #AA, 10",
        "Bitte geben Sie die Zielblock-ID ein:\nBeispiel: #AA, 10",
        "Introduzca el ID del bloque objetivo:\nEjemplo: #AA, 10",
        "Masukkan ID blok target:\nContoh: #AA, 10"
    };

    UiText GoToNotExistsError = {
        "Entered ID does not exist.",
        "輸入的 ID 並不存在。",
        "対象のブロック ID は存在しません。",
        "输入的 ID 不存在。",
        "입력한 ID가 존재하지 않습니다.",
        "L’identifiant saisi n’existe pas.",
        "Die eingegebene ID existiert nicht.",
        "El ID ingresado no existe.",
        "ID yang dimasukkan tidak ada."
    };

    UiText Go = {
        "Go", "前往", "移動", "前往", "이동",
        "Aller", "Los", "Ir", "Pergi"
    };

    UiText BackToCenter = {
        "Back To Center", "返回中心", "中央に戻る", "返回中心",
        "중앙으로 돌아가기",
        "Retour au centre", "Zur Mitte zurück", "Volver al centro",
        "Kembali ke tengah"
    };

    UiText Inspector = {
        "Inspector", "檢視器", "インスペクター", "检查器",
        "인스펙터",
        "Inspecteur", "Inspektor", "Inspector",
        "Inspektur"
    };

    UiText Explorer = {
        "Explorer", "檔案總管", "エクスプローラー", "档案总管",
        "탐색기",
        "Explorateur", "Explorer", "Explorador",
        "Penjelajah"
    };
};
