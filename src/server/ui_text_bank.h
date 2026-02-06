#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "engine/font_loader.h"

#include <array>
#include <cstddef>
#include <string>

/* ============================================================
 * UiText
 * ============================================================ */
struct UiText {
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
    // constructor
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

    UiText(const UiText& other) = default;
    UiText& operator=(const UiText& other) = default;
    UiText(UiText&& other) noexcept = default;
    UiText& operator=(UiText&& other) noexcept = default;
    ~UiText() = default;

    const char* get() const;
    const char* c_str() const;

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
public:
    UiLocale(
        size_t p_id,
        const char* p_name
    );

    UiLocale(const UiLocale& other);
    UiLocale& operator=(const UiLocale& other);
    ~UiLocale() = default;

    operator size_t() const;

    size_t get_locale_id() const;
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
};

const UiLocale UI_LOCALE_ZH = {
    UI_LOCALE_ID_ZH, "繁體中文",
};

const UiLocale UI_LOCALE_JP = {
    UI_LOCALE_ID_JP, "日本語",
};

const UiLocale UI_LOCALE_CN = {
    UI_LOCALE_ID_CN, "简体中文",
};

const UiLocale UI_LOCALE_KO = {
    UI_LOCALE_ID_KO, "한국어",
};

const UiLocale UI_LOCALE_FR = {
    UI_LOCALE_ID_FR, "Français",
};

const UiLocale UI_LOCALE_DE = {
    UI_LOCALE_ID_DE, "Deutsch",
};

const UiLocale UI_LOCALE_ES = {
    UI_LOCALE_ID_ES, "Español",
};

const UiLocale UI_LOCALE_ID = {
    UI_LOCALE_ID_ID, "Bahasa Indonesia",
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
    void refresh_locale_setting();
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

    UiText GotIt = {
        "Got it.", "了解。", "了解", "了解。", "알겠습니다.",
        "Compris.", "Verstanden.", "Entendido.", "Mengerti."
    };

    UiText Confirm = {
        "Confirm", "確認", "確認", "确认", "확인",
        "Confirmer", "Bestätigen", "Confirmar", "Konfirmasi"
    };

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

    UiText SaveAll = {
        "Save All", "全部儲存", "すべて保存", "全部保存", "모두 저장",
        "Tout enregistrer", "Alles speichern", "Guardar todo", "Simpan semua"
    };

    UiText SaveAs = {
        "Save As...", "另存新檔…", "名前を付けて保存…", "另存为…", "다른 이름으로 저장…",
        "Enregistrer sous…", "Speichern unter…", "Guardar como…", "Simpan sebagai..."
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
        
    UiText RenameWorkspace = {
        "Please enter the new name:",
        "請輸入新的名稱：",
        "新しい名前を入力してください：",
        "请输入新的名称：",
        "새 이름을 입력하세요:",
        "Veuillez saisir le nouveau nom :",
        "Bitte geben Sie den neuen Namen ein:",
        "Introduzca el nuevo nombre:",
        "Masukkan nama baru:"
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


    UiText WorkspaceTitle = {
        "Workspace :", "工作區：", "ワークスペース：", "工作区：",
        "작업 공간:",
        "Espace de travail :", "Arbeitsbereich:", "Espacio de trabajo:",
        "Ruang kerja:"
    };

    UiText ConfirmDelete = {
        "Are you sure you want to delete this?",
        "確定要刪除嗎？",
        "削除してもよろしいですか？",
        "确定要删除吗？",
        "삭제하시겠습니까?",
        "Êtes-vous sûr de vouloir supprimer ceci ?",
        "Möchten Sie dies wirklich löschen?",
        "¿Está seguro de que desea eliminar esto?",
        "Apakah Anda yakin ingin menghapus ini?"
    };

    UiText Yes = {
        "Yes",
        "是",
        "はい",
        "是",
        "예",
        "Oui",
        "Ja",
        "Sí",
        "Ya"
    };

    UiText No = {
        "No",
        "不用",
        "いいえ",
        "不用",
        "아니요",
        "Non",
        "Nein",
        "No",
        "Tidak"
    };

    UiText OptionEntry = {
        "Entry", "入口", "エントリー", "入口",
        "엔트리",
        "Entrée", "Einstieg", "Entrada", "Entri"
    };

    UiText OptionNode = {
        "Node", "節點", "ノード", "节点",
        "노드",
        "Nœud", "Knoten", "Nodo", "Node"
    };

    UiText OptionOption = {
        "Option", "選項", "オプション", "选项",
        "옵션",
        "Option", "Option", "Opción", "Opsi"
    };

    UiText OptionRepeater = {
        "Repeater", "重複器", "リピーター", "重复器",
        "리피터",
        "Répéteur", "Wiederholer", "Repetidor", "Pengulang"
    };

    UiText OptionModuleEntry = {
        "Module Entry", "模組入口", "モジュールエントリー", "模块入口",
        "모듈 엔트리",
        "Entrée du module", "Moduleinstieg", "Entrada del módulo", "Entri Modul"
    };

    UiText OptionModuleNode = {
        "Module Node", "模組節點", "モジュールノード", "模块节点",
        "모듈 노드",
        "Nœud du module", "Modulknoten", "Nodo de módulo", "Node Modul"
    };

    UiText OptionCreateNode = {
        "Create Node", "建立節點", "ノードを作成", "创建节点",
        "노드 생성",
        "Créer un nœud", "Knoten erstellen", "Crear nodo", "Buat Node"
    };

    UiText OptionConnect = {
        "Connect...", "連接…", "接続…", "连接…",
        "연결…",
        "Connecter…", "Verbinden…", "Conectar…", "Hubungkan…"
    };

    UiText OptionEdit = {
        "Edit", "編輯", "編集", "编辑",
        "편집",
        "Modifier", "Bearbeiten", "Editar", "Edit"
    };

    UiText OptionDeleteNode = {
        "Delete node", "刪除節點", "ノードを削除", "删除节点",
        "노드 삭제",
        "Supprimer le nœud", "Knoten löschen", "Eliminar nodo", "Hapus Node"
    };

    UiText OptionDeleteNodes = {
        "Delete nodes", "刪除多個節點", "複数ノードを削除", "删除多个节点",
        "여러 노드 삭제",
        "Supprimer les nœuds", "Knoten löschen", "Eliminar nodos", "Hapus Node"
    };

    UiText OptionAlignNodes = {
        "Align nodes", "對齊節點", "ノードを整列", "对齐节点",
        "노드 정렬",
        "Aligner les nœuds", "Knoten ausrichten", "Alinear nodos", "Ratakan Node"
    };

    UiText OptionAlignToRectangle = {
        "To rectangle", "排列成矩形", "矩形に整列", "排列成矩形",
        "사각형 정렬",
        "En rectangle", "Als Rechteck", "En rectángulo", "Ke persegi"
    };

    UiText OptionAlignToVertical = {
        "To vertical", "垂直排列", "縦に整列", "垂直排列",
        "수직 정렬",
        "Verticalement", "Vertikal", "Vertical", "Vertikal"
    };

    UiText OptionAlignToHorizontal = {
        "To horizontal", "水平排列", "横に整列", "水平排列",
        "수평 정렬",
        "Horizontalement", "Horizontal", "Horizontal", "Horizontal"
    };

    UiText OptionDeleteWorkspace = {
        "Delete Workspace", "刪除工作區", "ワークスペースを削除", "删除工作区",
        "작업 공간 삭제",
        "Supprimer l’espace de travail", "Arbeitsbereich löschen", "Eliminar espacio de trabajo", "Hapus Ruang Kerja"
    };

    UiText OptionRenameWorkspace = {
        "Rename Workspace", "重新命名工作區", "ワークスペースの名前を変更", "重命名工作区",
        "작업 공간 이름 변경",
        "Renommer l’espace de travail", "Arbeitsbereich umbenennen", "Renombrar espacio de trabajo", "Ganti Nama Ruang Kerja"
    };

    UiText OptionSaveWorkspace = {
        "Save Workspace", "儲存工作區", "ワークスペースを保存", "保存工作区",
        "작업 공간 저장",
        "Enregistrer l’espace de travail", "Arbeitsbereich speichern", "Guardar espacio de trabajo", "Simpan Ruang Kerja"
    };

    UiText OptionRoot = {
        "Root", "根節點", "ルート", "根节点",
        "루트",
        "Racine", "Wurzel", "Raíz", "Akar"
    };

};


#define FONT_SIZE_BIG 32.0f
#define FONT_SIZE_MIDDLE 18.0f
#define FONT_SIZE_SMALL 12.0f
