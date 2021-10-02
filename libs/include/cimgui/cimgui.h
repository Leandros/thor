#ifndef _CIMGUI_H_
#define _CIMGUI_H_

#include <stdio.h>

#if defined _WIN32 || defined __CYGWIN__
#define API __declspec(dllexport)
#ifndef __GNUC__
/* NO, BAD! BAD! */
/* #define snprintf sprintf_s */
#endif
#else
#define API
#endif

#if defined __cplusplus
#define EXTERN extern "C"
#else
#include <stdarg.h>
#include <stdbool.h>
#define EXTERN extern
#endif

#define CIMGUI_API EXTERN API
#ifndef CDECL
#define CDECL __cdecl
#endif
#define CONST const

struct ImGuiIO;
struct ImGuiStyle;
struct ImDrawData;
struct ImVec2;
struct ImVec4;
struct ImGuiTextEditCallbackData;
struct ImGuiSizeConstraintCallbackData;
struct ImDrawList;
struct ImGuiStorage;
struct ImFont;
struct ImFontConfig;
struct ImFontAtlas;
struct ImDrawCmd;

typedef unsigned short ImDrawIdx;
typedef unsigned int ImU32;
typedef unsigned short ImWchar;
typedef void* ImTextureID;
typedef ImU32 ImGuiID;
typedef int ImGuiCol;
typedef int ImGuiStyleVar;
typedef int ImGuiKey;
typedef int ImGuiAlign;
typedef int ImGuiColorEditMode;
typedef int ImGuiMouseCursor;
typedef int ImGuiWindowFlags;
typedef int ImGuiSetCond;
typedef int ImGuiInputTextFlags;
typedef int ImGuiSelectableFlags;
typedef int ImGuiTreeNodeFlags;
typedef int (*ImGuiTextEditCallback)(struct ImGuiTextEditCallbackData *data);
typedef void (*ImGuiSizeConstraintCallback)(struct ImGuiSizeConstraintCallbackData* data);
typedef void (*ImDrawCallback)(CONST struct ImDrawList* parent_list, CONST struct ImDrawCmd* cmd);


CIMGUI_API struct ImGuiIO*         CDECL igGetIO();
CIMGUI_API struct ImGuiStyle*      CDECL igGetStyle();
CIMGUI_API struct ImDrawData*      CDECL igGetDrawData();
CIMGUI_API void                    CDECL igNewFrame();
CIMGUI_API void                    CDECL igRender();
CIMGUI_API void                    CDECL igShutdown();
CIMGUI_API void                    CDECL igShowUserGuide();
CIMGUI_API void                    CDECL igShowStyleEditor(struct ImGuiStyle* ref);
CIMGUI_API void                    CDECL igShowTestWindow(bool* opened);
CIMGUI_API void                    CDECL igShowMetricsWindow(bool* opened);

// Window
CIMGUI_API bool             CDECL igBegin(CONST char* name, bool* p_open, ImGuiWindowFlags flags);
CIMGUI_API bool             CDECL igBegin2(CONST char* name, bool* p_open, CONST struct ImVec2 size_on_first_use, float bg_alpha, ImGuiWindowFlags flags);
CIMGUI_API void             CDECL igEnd();
CIMGUI_API bool             CDECL igBeginChild(CONST char* str_id, CONST struct ImVec2 size, bool border, ImGuiWindowFlags extra_flags);
CIMGUI_API bool             CDECL igBeginChildEx(ImGuiID id, CONST struct ImVec2 size, bool border, ImGuiWindowFlags extra_flags);
CIMGUI_API void             CDECL igEndChild();
CIMGUI_API void             CDECL igGetContentRegionMax(struct ImVec2* out);
CIMGUI_API void             CDECL igGetContentRegionAvail(struct ImVec2* out);
CIMGUI_API float            CDECL igGetContentRegionAvailWidth();
CIMGUI_API void             CDECL igGetWindowContentRegionMin(struct ImVec2* out);
CIMGUI_API void             CDECL igGetWindowContentRegionMax(struct ImVec2* out);
CIMGUI_API float            CDECL igGetWindowContentRegionWidth();
CIMGUI_API struct ImDrawList*      CDECL igGetWindowDrawList();
CIMGUI_API void             CDECL igGetWindowPos(struct ImVec2* out);
CIMGUI_API void             CDECL igGetWindowSize(struct ImVec2* out);
CIMGUI_API float            CDECL igGetWindowWidth();
CIMGUI_API float            CDECL igGetWindowHeight();
CIMGUI_API bool             CDECL igIsWindowCollapsed();
CIMGUI_API void             CDECL igSetWindowFontScale(float scale);

CIMGUI_API void             CDECL igSetNextWindowPos(CONST struct ImVec2 pos, ImGuiSetCond cond);
CIMGUI_API void             CDECL igSetNextWindowPosCenter(ImGuiSetCond cond);
CIMGUI_API void             CDECL igSetNextWindowSize(CONST struct ImVec2 size, ImGuiSetCond cond);
CIMGUI_API void             CDECL igSetNextWindowSizeConstraints(CONST struct ImVec2 size_min, CONST struct ImVec2 size_max, ImGuiSizeConstraintCallback custom_callback, void* custom_callback_data);
CIMGUI_API void             CDECL igSetNextWindowContentSize(CONST struct ImVec2 size);
CIMGUI_API void             CDECL igSetNextWindowContentWidth(float width);
CIMGUI_API void             CDECL igSetNextWindowCollapsed(bool collapsed, ImGuiSetCond cond);
CIMGUI_API void             CDECL igSetNextWindowFocus();
CIMGUI_API void             CDECL igSetWindowPos(CONST struct ImVec2 pos, ImGuiSetCond cond);
CIMGUI_API void             CDECL igSetWindowSize(CONST struct ImVec2 size, ImGuiSetCond cond);
CIMGUI_API void             CDECL igSetWindowCollapsed(bool collapsed, ImGuiSetCond cond);
CIMGUI_API void             CDECL igSetWindowFocus();
CIMGUI_API void             CDECL igSetWindowPosByName(CONST char* name, CONST struct ImVec2 pos, ImGuiSetCond cond);
CIMGUI_API void             CDECL igSetWindowSize2(CONST char* name, CONST struct ImVec2 size, ImGuiSetCond cond);
CIMGUI_API void             CDECL igSetWindowCollapsed2(CONST char* name, bool collapsed, ImGuiSetCond cond);
CIMGUI_API void             CDECL igSetWindowFocus2(CONST char* name);

CIMGUI_API float            CDECL igGetScrollX();
CIMGUI_API float            CDECL igGetScrollY();
CIMGUI_API float            CDECL igGetScrollMaxX();
CIMGUI_API float            CDECL igGetScrollMaxY();
CIMGUI_API void             CDECL igSetScrollX(float scroll_x);
CIMGUI_API void             CDECL igSetScrollY(float scroll_y);
CIMGUI_API void             CDECL igSetScrollHere(float center_y_ratio);
CIMGUI_API void             CDECL igSetScrollFromPosY(float pos_y, float center_y_ratio);
CIMGUI_API void             CDECL igSetKeyboardFocusHere(int offset);
CIMGUI_API void             CDECL igSetStateStorage(struct ImGuiStorage* tree);
CIMGUI_API struct ImGuiStorage*    CDECL igGetStateStorage();

// Parameters stacks (shared)
CIMGUI_API void             CDECL igPushFont(struct ImFont* font);
CIMGUI_API void             CDECL igPopFont();
CIMGUI_API void             CDECL igPushStyleColor(ImGuiCol idx, CONST struct ImVec4 col);
CIMGUI_API void             CDECL igPopStyleColor(int count);
CIMGUI_API void             CDECL igPushStyleVar(ImGuiStyleVar idx, float val);
CIMGUI_API void             CDECL igPushStyleVarVec(ImGuiStyleVar idx, CONST struct ImVec2 val);
CIMGUI_API void             CDECL igPopStyleVar(int count);
CIMGUI_API struct ImFont*          CDECL igGetFont();
CIMGUI_API float            CDECL igGetFontSize();
CIMGUI_API void             CDECL igGetFontTexUvWhitePixel(struct ImVec2* pOut);
CIMGUI_API ImU32            CDECL igGetColorU32(ImGuiCol idx, float alpha_mul);
CIMGUI_API ImU32            CDECL igGetColorU32Vec(CONST struct ImVec4* col);


// Parameters stacks (current window)
CIMGUI_API void             CDECL igPushItemWidth(float item_width);
CIMGUI_API void             CDECL igPopItemWidth();
CIMGUI_API float            CDECL igCalcItemWidth();
CIMGUI_API void             CDECL igPushTextWrapPos(float wrap_pos_x);
CIMGUI_API void             CDECL igPopTextWrapPos();
CIMGUI_API void             CDECL igPushAllowKeyboardFocus(bool v);
CIMGUI_API void             CDECL igPopAllowKeyboardFocus();
CIMGUI_API void             CDECL igPushButtonRepeat(bool repeat);
CIMGUI_API void             CDECL igPopButtonRepeat();

// Layout
CIMGUI_API void             CDECL igSeparator();
CIMGUI_API void             CDECL igSameLine(float pos_x, float spacing_w);
CIMGUI_API void             CDECL igNewLine();
CIMGUI_API void             CDECL igSpacing();
CIMGUI_API void             CDECL igDummy(CONST struct ImVec2* size);
CIMGUI_API void             CDECL igIndent(float indent_w);
CIMGUI_API void             CDECL igUnindent(float indent_w);
CIMGUI_API void             CDECL igBeginGroup();
CIMGUI_API void             CDECL igEndGroup();
CIMGUI_API void             CDECL igGetCursorPos(struct ImVec2* pOut);
CIMGUI_API float            CDECL igGetCursorPosX();
CIMGUI_API float            CDECL igGetCursorPosY();
CIMGUI_API void             CDECL igSetCursorPos(CONST struct ImVec2 local_pos);
CIMGUI_API void             CDECL igSetCursorPosX(float x);
CIMGUI_API void             CDECL igSetCursorPosY(float y);
CIMGUI_API void             CDECL igGetCursorStartPos(struct ImVec2* pOut);
CIMGUI_API void             CDECL igGetCursorScreenPos(struct ImVec2* pOut);
CIMGUI_API void             CDECL igSetCursorScreenPos(CONST struct ImVec2 pos);
CIMGUI_API void             CDECL igAlignFirstTextHeightToWidgets();
CIMGUI_API float            CDECL igGetTextLineHeight();
CIMGUI_API float            CDECL igGetTextLineHeightWithSpacing();
CIMGUI_API float            CDECL igGetItemsLineHeightWithSpacing();

//Columns
CIMGUI_API void             CDECL igColumns(int count, CONST char* id, bool border);
CIMGUI_API void             CDECL igNextColumn();
CIMGUI_API int              CDECL igGetColumnIndex();
CIMGUI_API float            CDECL igGetColumnOffset(int column_index);
CIMGUI_API void             CDECL igSetColumnOffset(int column_index, float offset_x);
CIMGUI_API float            CDECL igGetColumnWidth(int column_index);
CIMGUI_API int              CDECL igGetColumnsCount();

// ID scopes
// If you are creating widgets in a loop you most likely want to push a unique identifier so ImGui can differentiate them
// You can also use "##extra" within your widget name to distinguish them from each others (see 'Programmer Guide')
CIMGUI_API void             CDECL igPushIdStr(CONST char* str_id);
CIMGUI_API void             CDECL igPushIdStrRange(CONST char* str_begin, CONST char* str_end);
CIMGUI_API void             CDECL igPushIdPtr(CONST void* ptr_id);
CIMGUI_API void             CDECL igPushIdInt(int int_id);
CIMGUI_API void             CDECL igPopId();
CIMGUI_API ImGuiID          CDECL igGetIdStr(CONST char* str_id);
CIMGUI_API ImGuiID          CDECL igGetIdStrRange(CONST char* str_begin,CONST char* str_end);
CIMGUI_API ImGuiID          CDECL igGetIdPtr(CONST void* ptr_id);

// Widgets
CIMGUI_API void             CDECL igText(CONST char* fmt, ...);
CIMGUI_API void             CDECL igTextV(CONST char* fmt, va_list args);
CIMGUI_API void             CDECL igTextColored(CONST struct ImVec4 col, CONST char* fmt, ...);
CIMGUI_API void             CDECL igTextColoredV(CONST struct ImVec4 col, CONST char* fmt, va_list args);
CIMGUI_API void             CDECL igTextDisabled(CONST char* fmt, ...);
CIMGUI_API void             CDECL igTextDisabledV(CONST char* fmt, va_list args);
CIMGUI_API void             CDECL igTextWrapped(CONST char* fmt, ...);
CIMGUI_API void             CDECL igTextWrappedV(CONST char* fmt, va_list args);
CIMGUI_API void             CDECL igTextUnformatted(CONST char* text, CONST char* text_end);
CIMGUI_API void             CDECL igLabelText(CONST char* label, CONST char* fmt, ...);
CIMGUI_API void             CDECL igLabelTextV(CONST char* label, CONST char* fmt, va_list args);
CIMGUI_API void             CDECL igBullet();
CIMGUI_API void             CDECL igBulletText(CONST char* fmt, ...);
CIMGUI_API void             CDECL igBulletTextV(CONST char* fmt, va_list args);
CIMGUI_API bool             CDECL igButton(CONST char* label, CONST struct ImVec2 size);
CIMGUI_API bool             CDECL igSmallButton(CONST char* label);
CIMGUI_API bool             CDECL igInvisibleButton(CONST char* str_id, CONST struct ImVec2 size);
CIMGUI_API void             CDECL igImage(ImTextureID user_texture_id, CONST struct ImVec2 size, CONST struct ImVec2 uv0, CONST struct ImVec2 uv1, CONST struct ImVec4 tint_col, CONST struct ImVec4 border_col);
CIMGUI_API bool             CDECL igImageButton(ImTextureID user_texture_id, CONST struct ImVec2 size, CONST struct ImVec2 uv0, CONST struct ImVec2 uv1, int frame_padding, CONST struct ImVec4 bg_col, CONST struct ImVec4 tint_col);
CIMGUI_API bool             CDECL igCheckbox(CONST char* label, bool* v);
CIMGUI_API bool             CDECL igCheckboxFlags(CONST char* label, unsigned int* flags, unsigned int flags_value);
CIMGUI_API bool             CDECL igRadioButtonBool(CONST char* label, bool active);
CIMGUI_API bool             CDECL igRadioButton(CONST char* label, int* v, int v_button);
CIMGUI_API bool             CDECL igCombo(CONST char* label, int* current_item, CONST char** items, int items_count, int height_in_items);
CIMGUI_API bool             CDECL igCombo2(CONST char* label, int* current_item, CONST char* items_separated_by_zeros, int height_in_items);
CIMGUI_API bool             CDECL igCombo3(CONST char* label, int* current_item, bool(*items_getter)(void* data, int idx, CONST char** out_text), void* data, int items_count, int height_in_items);
CIMGUI_API bool             CDECL igColorButton(CONST struct ImVec4 col, bool small_height, bool outline_border);
CIMGUI_API bool             CDECL igColorEdit3(CONST char* label, float col[3]);
CIMGUI_API bool             CDECL igColorEdit4(CONST char* label, float col[4], bool show_alpha);
CIMGUI_API void             CDECL igColorEditMode(ImGuiColorEditMode mode);
CIMGUI_API void             CDECL igPlotLines(CONST char* label, CONST float* values, int values_count, int values_offset, CONST char* overlay_text, float scale_min, float scale_max, struct ImVec2 graph_size, int stride);
CIMGUI_API void             CDECL igPlotLines2(CONST char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, CONST char* overlay_text, float scale_min, float scale_max, struct ImVec2 graph_size);
CIMGUI_API void             CDECL igPlotHistogram(CONST char* label, CONST float* values, int values_count, int values_offset, CONST char* overlay_text, float scale_min, float scale_max, struct ImVec2 graph_size, int stride);
CIMGUI_API void             CDECL igPlotHistogram2(CONST char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, CONST char* overlay_text, float scale_min, float scale_max, struct ImVec2 graph_size);
CIMGUI_API void             CDECL igProgressBar(float fraction, CONST struct ImVec2* size_arg, CONST char* overlay);


// Widgets: Sliders (tip: ctrl+click on a slider to input text)
CIMGUI_API bool             CDECL igSliderFloat(CONST char* label, float* v, float v_min, float v_max, CONST char* display_format, float power);
CIMGUI_API bool             CDECL igSliderFloat2(CONST char* label, float v[2], float v_min, float v_max, CONST char* display_format, float power);
CIMGUI_API bool             CDECL igSliderFloat3(CONST char* label, float v[3], float v_min, float v_max, CONST char* display_format, float power);
CIMGUI_API bool             CDECL igSliderFloat4(CONST char* label, float v[4], float v_min, float v_max, CONST char* display_format, float power);
CIMGUI_API bool             CDECL igSliderAngle(CONST char* label, float* v_rad, float v_degrees_min, float v_degrees_max);
CIMGUI_API bool             CDECL igSliderInt(CONST char* label, int* v, int v_min, int v_max, CONST char* display_format);
CIMGUI_API bool             CDECL igSliderInt2(CONST char* label, int v[2], int v_min, int v_max, CONST char* display_format);
CIMGUI_API bool             CDECL igSliderInt3(CONST char* label, int v[3], int v_min, int v_max, CONST char* display_format);
CIMGUI_API bool             CDECL igSliderInt4(CONST char* label, int v[4], int v_min, int v_max, CONST char* display_format);
CIMGUI_API bool             CDECL igVSliderFloat(CONST char* label, CONST struct ImVec2 size, float* v, float v_min, float v_max, CONST char* display_format, float power);
CIMGUI_API bool             CDECL igVSliderInt(CONST char* label, CONST struct ImVec2 size, int* v, int v_min, int v_max, CONST char* display_format);

// Widgets: Drags (tip: ctrl+click on a drag box to input text)
CIMGUI_API bool             CDECL igDragFloat(CONST char* label, float* v, float v_speed, float v_min, float v_max, CONST char* display_format, float power);     // If v_max >= v_max we have no bound
CIMGUI_API bool             CDECL igDragFloat2(CONST char* label, float v[2], float v_speed, float v_min, float v_max, CONST char* display_format, float power);
CIMGUI_API bool             CDECL igDragFloat3(CONST char* label, float v[3], float v_speed, float v_min, float v_max, CONST char* display_format, float power);
CIMGUI_API bool             CDECL igDragFloat4(CONST char* label, float v[4], float v_speed, float v_min, float v_max, CONST char* display_format, float power);
CIMGUI_API bool             CDECL igDragFloatRange2(CONST char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, CONST char* display_format, CONST char* display_format_max, float power);
CIMGUI_API bool             CDECL igDragInt(CONST char* label, int* v, float v_speed, int v_min, int v_max, CONST char* display_format);                                       // If v_max >= v_max we have no bound
CIMGUI_API bool             CDECL igDragInt2(CONST char* label, int v[2], float v_speed, int v_min, int v_max, CONST char* display_format);
CIMGUI_API bool             CDECL igDragInt3(CONST char* label, int v[3], float v_speed, int v_min, int v_max, CONST char* display_format);
CIMGUI_API bool             CDECL igDragInt4(CONST char* label, int v[4], float v_speed, int v_min, int v_max, CONST char* display_format);
CIMGUI_API bool             CDECL igDragIntRange2(CONST char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, CONST char* display_format, CONST char* display_format_max);


// Widgets: Input
CIMGUI_API bool             CDECL igInputText(CONST char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data);
CIMGUI_API bool             CDECL igInputTextMultiline(CONST char* label, char* buf, size_t buf_size, CONST struct ImVec2 size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data);
CIMGUI_API bool             CDECL igInputFloat(CONST char* label, float* v, float step, float step_fast, int decimal_precision, ImGuiInputTextFlags extra_flags);
CIMGUI_API bool             CDECL igInputFloat2(CONST char* label, float v[2], int decimal_precision, ImGuiInputTextFlags extra_flags);
CIMGUI_API bool             CDECL igInputFloat3(CONST char* label, float v[3], int decimal_precision, ImGuiInputTextFlags extra_flags);
CIMGUI_API bool             CDECL igInputFloat4(CONST char* label, float v[4], int decimal_precision, ImGuiInputTextFlags extra_flags);
CIMGUI_API bool             CDECL igInputInt(CONST char* label, int* v, int step, int step_fast, ImGuiInputTextFlags extra_flags);
CIMGUI_API bool             CDECL igInputInt2(CONST char* label, int v[2], ImGuiInputTextFlags extra_flags);
CIMGUI_API bool             CDECL igInputInt3(CONST char* label, int v[3], ImGuiInputTextFlags extra_flags);
CIMGUI_API bool             CDECL igInputInt4(CONST char* label, int v[4], ImGuiInputTextFlags extra_flags);

// Widgets: Trees
CIMGUI_API bool             CDECL igTreeNode(CONST char* label);
CIMGUI_API bool             CDECL igTreeNodeStr(CONST char* str_id, CONST char* fmt, ...);
CIMGUI_API bool             CDECL igTreeNodePtr(CONST void* ptr_id, CONST char* fmt, ...);
CIMGUI_API bool             CDECL igTreeNodeStrV(CONST char* str_id, CONST char* fmt, va_list args);
CIMGUI_API bool             CDECL igTreeNodePtrV(CONST void* ptr_id, CONST char* fmt, va_list args);
CIMGUI_API bool             CDECL igTreeNodeEx(CONST char* label, ImGuiTreeNodeFlags flags);
CIMGUI_API bool             CDECL igTreeNodeExStr(CONST char* str_id, ImGuiTreeNodeFlags flags, CONST char* fmt, ...);
CIMGUI_API bool             CDECL igTreeNodeExPtr(CONST void* ptr_id, ImGuiTreeNodeFlags flags, CONST char* fmt, ...);
CIMGUI_API bool             CDECL igTreeNodeExV(CONST char* str_id, ImGuiTreeNodeFlags flags, CONST char* fmt, va_list args);
CIMGUI_API bool             CDECL igTreeNodeExVPtr(CONST void* ptr_id, ImGuiTreeNodeFlags flags, CONST char* fmt, va_list args);
CIMGUI_API void             CDECL igTreePushStr(CONST char* str_id);
CIMGUI_API void             CDECL igTreePushPtr(CONST void* ptr_id);
CIMGUI_API void             CDECL igTreePop();
CIMGUI_API void             CDECL igTreeAdvanceToLabelPos();
CIMGUI_API float            CDECL igGetTreeNodeToLabelSpacing();
CIMGUI_API void             CDECL igSetNextTreeNodeOpen(bool opened, ImGuiSetCond cond);
CIMGUI_API bool             CDECL igCollapsingHeader(CONST char* label, ImGuiTreeNodeFlags flags);
CIMGUI_API bool             CDECL igCollapsingHeaderEx(CONST char* label, bool* p_open, ImGuiTreeNodeFlags flags);

// Widgets: Selectable / Lists
CIMGUI_API bool             CDECL igSelectable(CONST char* label, bool selected, ImGuiSelectableFlags flags, CONST struct ImVec2 size);
CIMGUI_API bool             CDECL igSelectableEx(CONST char* label, bool* p_selected, ImGuiSelectableFlags flags, CONST struct ImVec2 size);
CIMGUI_API bool             CDECL igListBox(CONST char* label, int* current_item, CONST char** items, int items_count, int height_in_items);
CIMGUI_API bool             CDECL igListBox2(CONST char* label, int* current_item, bool(*items_getter)(void* data, int idx, CONST char** out_text), void* data, int items_count, int height_in_items);
CIMGUI_API bool             CDECL igListBoxHeader(CONST char* label, CONST struct ImVec2 size);
CIMGUI_API bool             CDECL igListBoxHeader2(CONST char* label, int items_count, int height_in_items);
CIMGUI_API void             CDECL igListBoxFooter();

// Widgets: Value() Helpers. Output single value in "name: value" format (tip: freely declare your own within the ImGui namespace!)
CIMGUI_API void             CDECL igValueBool(CONST char* prefix, bool b);
CIMGUI_API void             CDECL igValueInt(CONST char* prefix, int v);
CIMGUI_API void             CDECL igValueUInt(CONST char* prefix, unsigned int v);
CIMGUI_API void             CDECL igValueFloat(CONST char* prefix, float v, CONST char* float_format);
CIMGUI_API void             CDECL igValueColor(CONST char* prefix, CONST struct ImVec4 v);
CIMGUI_API void             CDECL igValueColor2(CONST char* prefix, unsigned int v);

// Tooltip
CIMGUI_API void             CDECL igSetTooltip(CONST char* fmt, ...);
CIMGUI_API void             CDECL igSetTooltipV(CONST char* fmt, va_list args);
CIMGUI_API void             CDECL igBeginTooltip();
CIMGUI_API void             CDECL igEndTooltip();

// Widgets: Menus
CIMGUI_API bool             CDECL igBeginMainMenuBar();
CIMGUI_API void             CDECL igEndMainMenuBar();
CIMGUI_API bool             CDECL igBeginMenuBar();
CIMGUI_API void             CDECL igEndMenuBar();
CIMGUI_API bool             CDECL igBeginMenu(CONST char* label, bool enabled);
CIMGUI_API void             CDECL igEndMenu();
CIMGUI_API bool             CDECL igMenuItem(CONST char* label, CONST char* shortcut, bool selected, bool enabled);
CIMGUI_API bool             CDECL igMenuItemPtr(CONST char* label, CONST char* shortcut, bool* p_selected, bool enabled);

// Popup
CIMGUI_API void             CDECL igOpenPopup(CONST char* str_id);
CIMGUI_API bool             CDECL igBeginPopup(CONST char* str_id);
CIMGUI_API bool             CDECL igBeginPopupModal(CONST char* name, bool* p_open, ImGuiWindowFlags extra_flags);
CIMGUI_API bool             CDECL igBeginPopupContextItem(CONST char* str_id, int mouse_button);
CIMGUI_API bool             CDECL igBeginPopupContextWindow(bool also_over_items, CONST char* str_id, int mouse_button);
CIMGUI_API bool             CDECL igBeginPopupContextVoid(CONST char* str_id, int mouse_button);
CIMGUI_API void             CDECL igEndPopup();
CIMGUI_API void             CDECL igCloseCurrentPopup();

// Logging: all text output from interface is redirected to tty/file/clipboard. Tree nodes are automatically opened.
CIMGUI_API void             CDECL igLogToTTY(int max_depth);
CIMGUI_API void             CDECL igLogToFile(int max_depth, CONST char* filename);
CIMGUI_API void             CDECL igLogToClipboard(int max_depth);
CIMGUI_API void             CDECL igLogFinish();
CIMGUI_API void             CDECL igLogButtons();
CIMGUI_API void             CDECL igLogText(CONST char* fmt, ...);

// Clipping
CIMGUI_API void             CDECL igPushClipRect(CONST struct ImVec2 clip_rect_min, CONST struct ImVec2 clip_rect_max, bool intersect_with_current_clip_rect);
CIMGUI_API void             CDECL igPopClipRect();

// Utilities
CIMGUI_API bool             CDECL igIsItemHovered();
CIMGUI_API bool             CDECL igIsItemHoveredRect();
CIMGUI_API bool             CDECL igIsItemActive();
CIMGUI_API bool             CDECL igIsItemClicked(int mouse_button);
CIMGUI_API bool             CDECL igIsItemVisible();
CIMGUI_API bool             CDECL igIsAnyItemHovered();
CIMGUI_API bool             CDECL igIsAnyItemActive();
CIMGUI_API void             CDECL igGetItemRectMin(struct ImVec2* pOut);
CIMGUI_API void             CDECL igGetItemRectMax(struct ImVec2* pOut);
CIMGUI_API void             CDECL igGetItemRectSize(struct ImVec2* pOut);
CIMGUI_API void             CDECL igSetItemAllowOverlap();
CIMGUI_API bool             CDECL igIsWindowHovered();
CIMGUI_API bool             CDECL igIsWindowFocused();
CIMGUI_API bool             CDECL igIsRootWindowFocused();
CIMGUI_API bool             CDECL igIsRootWindowOrAnyChildFocused();
CIMGUI_API bool             CDECL igIsRootWindowOrAnyChildHovered();
CIMGUI_API bool             CDECL igIsRectVisible(CONST struct ImVec2 item_size);
CIMGUI_API bool             CDECL igIsPosHoveringAnyWindow(CONST struct ImVec2 pos);
CIMGUI_API float            CDECL igGetTime();
CIMGUI_API int              CDECL igGetFrameCount();
CIMGUI_API CONST char*      CDECL igGetStyleColName(ImGuiCol idx);
CIMGUI_API void             CDECL igCalcItemRectClosestPoint(struct ImVec2* pOut, CONST struct ImVec2 pos, bool on_edge, float outward);
CIMGUI_API void             CDECL igCalcTextSize(struct ImVec2* pOut, CONST char* text, CONST char* text_end, bool hide_text_after_double_hash, float wrap_width);
CIMGUI_API void             CDECL igCalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end);

CIMGUI_API bool             CDECL igBeginChildFrame(ImGuiID id, CONST struct ImVec2 size, ImGuiWindowFlags extra_flags);
CIMGUI_API void             CDECL igEndChildFrame();

CIMGUI_API void             CDECL igColorConvertU32ToFloat4(struct ImVec4* pOut, ImU32 in);
CIMGUI_API ImU32            CDECL igColorConvertFloat4ToU32(CONST struct ImVec4 in);
CIMGUI_API void             CDECL igColorConvertRGBtoHSV(float r, float g, float b, float* out_h, float* out_s, float* out_v);
CIMGUI_API void             CDECL igColorConvertHSVtoRGB(float h, float s, float v, float* out_r, float* out_g, float* out_b);

CIMGUI_API int              CDECL igGetKeyIndex(ImGuiKey key);
CIMGUI_API bool             CDECL igIsKeyDown(int key_index);
CIMGUI_API bool             CDECL igIsKeyPressed(int key_index, bool repeat);
CIMGUI_API bool             CDECL igIsKeyReleased(int key_index);
CIMGUI_API bool             CDECL igIsMouseDown(int button);
CIMGUI_API bool             CDECL igIsMouseClicked(int button, bool repeat);
CIMGUI_API bool             CDECL igIsMouseDoubleClicked(int button);
CIMGUI_API bool             CDECL igIsMouseReleased(int button);
CIMGUI_API bool             CDECL igIsMouseHoveringWindow();
CIMGUI_API bool             CDECL igIsMouseHoveringAnyWindow();
CIMGUI_API bool             CDECL igIsMouseHoveringRect(CONST struct ImVec2 r_min, CONST struct ImVec2 r_max, bool clip);
CIMGUI_API bool             CDECL igIsMouseDragging(int button, float lock_threshold);
CIMGUI_API void             CDECL igGetMousePos(struct ImVec2* pOut);
CIMGUI_API void             CDECL igGetMousePosOnOpeningCurrentPopup(struct ImVec2* pOut);
CIMGUI_API void             CDECL igGetMouseDragDelta(struct ImVec2* pOut, int button, float lock_threshold);
CIMGUI_API void             CDECL igResetMouseDragDelta(int button);
CIMGUI_API ImGuiMouseCursor CDECL igGetMouseCursor();
CIMGUI_API void             CDECL igSetMouseCursor(ImGuiMouseCursor type);
CIMGUI_API void             CDECL igCaptureKeyboardFromApp(bool capture);
CIMGUI_API void             CDECL igCaptureMouseFromApp(bool capture);

// Helpers functions to access functions pointers in ImGui::GetIO()
CIMGUI_API void*            CDECL igMemAlloc(size_t sz);
CIMGUI_API void             CDECL igMemFree(void* ptr);
CIMGUI_API CONST char*      CDECL igGetClipboardText();
CIMGUI_API void             CDECL igSetClipboardText(CONST char* text);

// Internal state access - if you want to share ImGui state between modules (e.g. DLL) or allocate it yourself
CIMGUI_API CONST char*             CDECL igGetVersion();
CIMGUI_API struct ImGuiContext*    CDECL igCreateContext(void* (*malloc_fn)(size_t), void (*free_fn)(void*));
CIMGUI_API void                    CDECL igDestroyContext(struct ImGuiContext* ctx);
CIMGUI_API struct ImGuiContext*    CDECL igGetCurrentContext();
CIMGUI_API void                    CDECL igSetCurrentContext(struct ImGuiContext* ctx);

CIMGUI_API void             CDECL ImFontConfig_DefaultConstructor(struct ImFontConfig* config);

CIMGUI_API void             CDECL ImFontAtlas_GetTexDataAsRGBA32(struct ImFontAtlas* atlas, unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel);
CIMGUI_API void             CDECL ImFontAtlas_GetTexDataAsAlpha8(struct ImFontAtlas* atlas, unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel);
CIMGUI_API void             CDECL ImFontAtlas_SetTexID(struct ImFontAtlas* atlas, void* tex);
CIMGUI_API struct ImFont*   CDECL ImFontAtlas_AddFont(struct ImFontAtlas* atlas, CONST struct ImFontConfig* font_cfg);
CIMGUI_API struct ImFont*   CDECL ImFontAtlas_AddFontDefault(struct ImFontAtlas* atlas, CONST struct ImFontConfig* font_cfg);
CIMGUI_API struct ImFont*   CDECL ImFontAtlas_AddFontFromFileTTF(struct ImFontAtlas* atlas, CONST char* filename, float size_pixels, CONST struct ImFontConfig* font_cfg, CONST ImWchar* glyph_ranges);
CIMGUI_API struct ImFont*   CDECL ImFontAtlas_AddFontFromMemoryTTF(struct ImFontAtlas* atlas, void* ttf_data, int ttf_size, float size_pixels, CONST struct ImFontConfig* font_cfg, CONST ImWchar* glyph_ranges);
CIMGUI_API struct ImFont*   CDECL ImFontAtlas_AddFontFromMemoryCompressedTTF(struct ImFontAtlas* atlas, CONST void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, CONST struct ImFontConfig* font_cfg, CONST ImWchar* glyph_ranges);
CIMGUI_API struct ImFont*   CDECL ImFontAtlas_AddFontFromMemoryCompressedBase85TTF(struct ImFontAtlas* atlas, CONST char* compressed_ttf_data_base85, float size_pixels, CONST struct ImFontConfig* font_cfg, CONST ImWchar* glyph_ranges);
CIMGUI_API void             CDECL ImFontAtlas_ClearTexData(struct ImFontAtlas* atlas);
CIMGUI_API void             CDECL ImFontAtlas_Clear(struct ImFontAtlas* atlas);

CIMGUI_API void             CDECL ImGuiIO_AddInputCharacter(unsigned short c);
CIMGUI_API void             CDECL ImGuiIO_AddInputCharactersUTF8(CONST char* utf8_chars);
CIMGUI_API void             CDECL ImGuiIO_ClearInputCharacters();

//ImDrawData
CIMGUI_API void                 CDECL ImDrawData_DeIndexAllBuffers(struct ImDrawData* drawData);

//ImDrawList
CIMGUI_API int                  CDECL ImDrawList_GetVertexBufferSize(struct ImDrawList* list);
CIMGUI_API struct ImDrawVert*   CDECL ImDrawList_GetVertexPtr(struct ImDrawList* list, int n);
CIMGUI_API int                  CDECL ImDrawList_GetIndexBufferSize(struct ImDrawList* list);
CIMGUI_API ImDrawIdx*           CDECL ImDrawList_GetIndexPtr(struct ImDrawList* list, int n);
CIMGUI_API int                  CDECL ImDrawList_GetCmdSize(struct ImDrawList* list);
CIMGUI_API struct ImDrawCmd*    CDECL ImDrawList_GetCmdPtr(struct ImDrawList* list, int n);

CIMGUI_API void             CDECL ImDrawList_Clear(struct ImDrawList* list);
CIMGUI_API void             CDECL ImDrawList_ClearFreeMemory(struct ImDrawList* list);
CIMGUI_API void             CDECL ImDrawList_PushClipRect(struct ImDrawList* list, struct ImVec2 clip_rect_min, struct ImVec2 clip_rect_max, bool intersect_with_current_clip_rect);
CIMGUI_API void             CDECL ImDrawList_PushClipRectFullScreen(struct ImDrawList* list);
CIMGUI_API void             CDECL ImDrawList_PopClipRect(struct ImDrawList* list);
CIMGUI_API void             CDECL ImDrawList_PushTextureID(struct ImDrawList* list, CONST ImTextureID texture_id);
CIMGUI_API void             CDECL ImDrawList_PopTextureID(struct ImDrawList* list);

// Primitives
CIMGUI_API void             CDECL ImDrawList_AddLine(struct ImDrawList* list, CONST struct ImVec2 a, CONST struct ImVec2 b, ImU32 col, float thickness);
CIMGUI_API void             CDECL ImDrawList_AddRect(struct ImDrawList* list, CONST struct ImVec2 a, CONST struct ImVec2 b, ImU32 col, float rounding, int rounding_corners, float thickness);
CIMGUI_API void             CDECL ImDrawList_AddRectFilled(struct ImDrawList* list, CONST struct ImVec2 a, CONST struct ImVec2 b, ImU32 col, float rounding, int rounding_corners);
CIMGUI_API void             CDECL ImDrawList_AddRectFilledMultiColor(struct ImDrawList* list, CONST struct ImVec2 a, CONST struct ImVec2 b, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
CIMGUI_API void             CDECL ImDrawList_AddQuad(struct ImDrawList* list, CONST struct ImVec2 a, CONST struct ImVec2 b, CONST struct ImVec2 c, CONST struct ImVec2 d, ImU32 col, float thickness);
CIMGUI_API void             CDECL ImDrawList_AddQuadFilled(struct ImDrawList* list, CONST struct ImVec2 a, CONST struct ImVec2 b, CONST struct ImVec2 c, CONST struct ImVec2 d, ImU32 col);
CIMGUI_API void             CDECL ImDrawList_AddTriangle(struct ImDrawList* list, CONST struct ImVec2 a, CONST struct ImVec2 b, CONST struct ImVec2 c, ImU32 col, float thickness);
CIMGUI_API void             CDECL ImDrawList_AddTriangleFilled(struct ImDrawList* list, CONST struct ImVec2 a, CONST struct ImVec2 b, CONST struct ImVec2 c, ImU32 col);
CIMGUI_API void             CDECL ImDrawList_AddCircle(struct ImDrawList* list, CONST struct ImVec2 centre, float radius, ImU32 col, int num_segments, float thickness);
CIMGUI_API void             CDECL ImDrawList_AddCircleFilled(struct ImDrawList* list, CONST struct ImVec2 centre, float radius, ImU32 col, int num_segments);
CIMGUI_API void             CDECL ImDrawList_AddText(struct ImDrawList* list, CONST struct ImVec2 pos, ImU32 col, CONST char* text_begin, CONST char* text_end);
CIMGUI_API void             CDECL ImDrawList_AddTextExt(struct ImDrawList* list, CONST struct ImFont* font, float font_size, CONST struct ImVec2 pos, ImU32 col, CONST char* text_begin, CONST char* text_end, float wrap_width, CONST struct ImVec4* cpu_fine_clip_rect);
CIMGUI_API void             CDECL ImDrawList_AddImage(struct ImDrawList* list, ImTextureID user_texture_id, CONST struct ImVec2 a, CONST struct ImVec2 b, CONST struct ImVec2 uv0, CONST struct ImVec2 uv1, ImU32 col);
CIMGUI_API void             CDECL ImDrawList_AddPolyline(struct ImDrawList* list, CONST struct ImVec2* points, CONST int num_points, ImU32 col, bool closed, float thickness, bool anti_aliased);
CIMGUI_API void             CDECL ImDrawList_AddConvexPolyFilled(struct ImDrawList* list, CONST struct ImVec2* points, CONST int num_points, ImU32 col, bool anti_aliased);
CIMGUI_API void             CDECL ImDrawList_AddBezierCurve(struct ImDrawList* list, CONST struct ImVec2 pos0, CONST struct ImVec2 cp0, CONST struct ImVec2 cp1, CONST struct ImVec2 pos1, ImU32 col, float thickness, int num_segments);

// Stateful path API, add points then finish with PathFill() or PathStroke()
CIMGUI_API void             CDECL ImDrawList_PathClear(struct ImDrawList* list);
CIMGUI_API void             CDECL ImDrawList_PathLineTo(struct ImDrawList* list, CONST struct ImVec2 pos);
CIMGUI_API void             CDECL ImDrawList_PathLineToMergeDuplicate(struct ImDrawList* list, CONST struct ImVec2 pos);
CIMGUI_API void             CDECL ImDrawList_PathFill(struct ImDrawList* list, ImU32 col);
CIMGUI_API void             CDECL ImDrawList_PathStroke(struct ImDrawList* list, ImU32 col, bool closed, float thickness);
CIMGUI_API void             CDECL ImDrawList_PathArcTo(struct ImDrawList* list, CONST struct ImVec2 centre, float radius, float a_min, float a_max, int num_segments);
CIMGUI_API void             CDECL ImDrawList_PathArcToFast(struct ImDrawList* list, CONST struct ImVec2 centre, float radius, int a_min_of_12, int a_max_of_12); // Use precomputed angles for a 12 steps circle
CIMGUI_API void             CDECL ImDrawList_PathBezierCurveTo(struct ImDrawList* list, CONST struct ImVec2 p1, CONST struct ImVec2 p2, CONST struct ImVec2 p3, int num_segments);
CIMGUI_API void             CDECL ImDrawList_PathRect(struct ImDrawList* list, CONST struct ImVec2 rect_min, CONST struct ImVec2 rect_max, float rounding, int rounding_corners);

// Channels
CIMGUI_API void             CDECL ImDrawList_ChannelsSplit(struct ImDrawList* list, int channels_count);
CIMGUI_API void             CDECL ImDrawList_ChannelsMerge(struct ImDrawList* list);
CIMGUI_API void             CDECL ImDrawList_ChannelsSetCurrent(struct ImDrawList* list, int channel_index);

// Advanced
CIMGUI_API void             CDECL ImDrawList_AddCallback(struct ImDrawList* list, ImDrawCallback callback, void* callback_data); // Your rendering function must check for 'UserCallback' in ImDrawCmd and call the function instead of rendering triangles.
CIMGUI_API void             CDECL ImDrawList_AddDrawCmd(struct ImDrawList* list); // This is useful if you need to forcefully create a new draw call (to allow for dependent rendering / blending). Otherwise primitives are merged into the same draw-call as much as possible

// Internal helpers
CIMGUI_API void             CDECL ImDrawList_PrimReserve(struct ImDrawList* list, int idx_count, int vtx_count);
CIMGUI_API void             CDECL ImDrawList_PrimRect(struct ImDrawList* list, CONST struct ImVec2 a, CONST struct ImVec2 b, ImU32 col);
CIMGUI_API void             CDECL ImDrawList_PrimRectUV(struct ImDrawList* list, CONST struct ImVec2 a, CONST struct ImVec2 b, CONST struct ImVec2 uv_a, CONST struct ImVec2 uv_b, ImU32 col);
CIMGUI_API void             CDECL ImDrawList_PrimQuadUV(struct ImDrawList* list,CONST struct ImVec2 a, CONST struct ImVec2 b, CONST struct ImVec2 c, CONST struct ImVec2 d, CONST struct ImVec2 uv_a, CONST struct ImVec2 uv_b, CONST struct ImVec2 uv_c, CONST struct ImVec2 uv_d, ImU32 col);
CIMGUI_API void             CDECL ImDrawList_PrimWriteVtx(struct ImDrawList* list, CONST struct ImVec2 pos, CONST struct ImVec2 uv, ImU32 col);
CIMGUI_API void             CDECL ImDrawList_PrimWriteIdx(struct ImDrawList* list, ImDrawIdx idx);
CIMGUI_API void             CDECL ImDrawList_PrimVtx(struct ImDrawList* list, CONST struct ImVec2 pos, CONST struct ImVec2 uv, ImU32 col);
CIMGUI_API void             CDECL ImDrawList_UpdateClipRect(struct ImDrawList* list);
CIMGUI_API void             CDECL ImDrawList_UpdateTextureID(struct ImDrawList* list);


/* ========================================================================= */
/* Implementation                                                            */
/* ========================================================================= */
#ifndef IMGUI_VERSION

struct ImVec2
{
    float x, y;
};

struct ImVec4
{
    float x, y, z, w;
};

struct ImDrawVert
{
    struct ImVec2   pos;
    struct ImVec2   uv;
    ImU32           col;
};

struct ImDrawData
{
    bool            Valid;
    struct ImDrawList** CmdLists;
    int             CmdListsCount;
    int             TotalVtxCount;
    int             TotalIdxCount;
};

typedef void (*ImDrawCallback)(const struct ImDrawList*, const struct ImDrawCmd*);

// Typically, 1 command = 1 gpu draw call (unless command is a callback)
struct ImDrawCmd
{
    unsigned int    ElemCount;
    struct ImVec4   ClipRect;
    ImTextureID     TextureId;
    ImDrawCallback  UserCallback;
    void*           UserCallbackData;
};

// Flags for ImGui::Begin()
typedef enum ImGuiWindowFlags_
{
    // Default: 0
    ImGuiWindowFlags_NoTitleBar             = 1 << 0,
    ImGuiWindowFlags_NoResize               = 1 << 1,
    ImGuiWindowFlags_NoMove                 = 1 << 2,
    ImGuiWindowFlags_NoScrollbar            = 1 << 3,
    ImGuiWindowFlags_NoScrollWithMouse      = 1 << 4,
    ImGuiWindowFlags_NoCollapse             = 1 << 5,
    ImGuiWindowFlags_AlwaysAutoResize       = 1 << 6,
    ImGuiWindowFlags_ShowBorders            = 1 << 7,
    ImGuiWindowFlags_NoSavedSettings        = 1 << 8,
    ImGuiWindowFlags_NoInputs               = 1 << 9,
    ImGuiWindowFlags_MenuBar                = 1 << 10,
    ImGuiWindowFlags_HorizontalScrollbar    = 1 << 11,
    ImGuiWindowFlags_NoFocusOnAppearing     = 1 << 12,
    ImGuiWindowFlags_NoBringToFrontOnFocus  = 1 << 13,
    ImGuiWindowFlags_AlwaysVerticalScrollbar= 1 << 14,
    ImGuiWindowFlags_AlwaysHorizontalScrollbar=1<< 15,
    ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 16,
    // [Internal]
    ImGuiWindowFlags_ChildWindow            = 1 << 20,
    ImGuiWindowFlags_ChildWindowAutoFitX    = 1 << 21,
    ImGuiWindowFlags_ChildWindowAutoFitY    = 1 << 22,
    ImGuiWindowFlags_ComboBox               = 1 << 23,
    ImGuiWindowFlags_Tooltip                = 1 << 24,
    ImGuiWindowFlags_Popup                  = 1 << 25,
    ImGuiWindowFlags_Modal                  = 1 << 26,
    ImGuiWindowFlags_ChildMenu              = 1 << 27
} ImGuiWindowFlags_;

// Flags for ImGui::InputText()
enum ImGuiInputTextFlags_
{
    // Default: 0
    ImGuiInputTextFlags_CharsDecimal        = 1 << 0,
    ImGuiInputTextFlags_CharsHexadecimal    = 1 << 1,
    ImGuiInputTextFlags_CharsUppercase      = 1 << 2,
    ImGuiInputTextFlags_CharsNoBlank        = 1 << 3,
    ImGuiInputTextFlags_AutoSelectAll       = 1 << 4,
    ImGuiInputTextFlags_EnterReturnsTrue    = 1 << 5,
    ImGuiInputTextFlags_CallbackCompletion  = 1 << 6,
    ImGuiInputTextFlags_CallbackHistory     = 1 << 7,
    ImGuiInputTextFlags_CallbackAlways      = 1 << 8,
    ImGuiInputTextFlags_CallbackCharFilter  = 1 << 9,
    ImGuiInputTextFlags_AllowTabInput       = 1 << 10,
    ImGuiInputTextFlags_CtrlEnterForNewLine = 1 << 11,
    ImGuiInputTextFlags_NoHorizontalScroll  = 1 << 12,
    ImGuiInputTextFlags_AlwaysInsertMode    = 1 << 13,
    ImGuiInputTextFlags_ReadOnly            = 1 << 14,
    ImGuiInputTextFlags_Password            = 1 << 15,
    // [Internal]
    ImGuiInputTextFlags_Multiline           = 1 << 20
};

// Flags for ImGui::TreeNodeEx(), ImGui::CollapsingHeader*()
enum ImGuiTreeNodeFlags_
{
    ImGuiTreeNodeFlags_Selected             = 1 << 0,
    ImGuiTreeNodeFlags_Framed               = 1 << 1,
    ImGuiTreeNodeFlags_AllowOverlapMode     = 1 << 2,
    ImGuiTreeNodeFlags_NoTreePushOnOpen     = 1 << 3,
    ImGuiTreeNodeFlags_NoAutoOpenOnLog      = 1 << 4,
    ImGuiTreeNodeFlags_DefaultOpen          = 1 << 5,
    ImGuiTreeNodeFlags_OpenOnDoubleClick    = 1 << 6,
    ImGuiTreeNodeFlags_OpenOnArrow          = 1 << 7,
    ImGuiTreeNodeFlags_Leaf                 = 1 << 8,
    ImGuiTreeNodeFlags_Bullet               = 1 << 9,
    //ImGuITreeNodeFlags_SpanAllAvailWidth  = 1 << 10,
    //ImGuiTreeNodeFlags_NoScrollOnOpen     = 1 << 11,
    ImGuiTreeNodeFlags_CollapsingHeader     = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoAutoOpenOnLog
};

// Flags for ImGui::Selectable()
enum ImGuiSelectableFlags_
{
    // Default: 0
    ImGuiSelectableFlags_DontClosePopups    = 1 << 0,
    ImGuiSelectableFlags_SpanAllColumns     = 1 << 1,
    ImGuiSelectableFlags_AllowDoubleClick   = 1 << 2
};

// User fill ImGuiIO.KeyMap[] array with indices into the ImGuiIO.KeysDown[512] array
enum ImGuiKey_
{
    ImGuiKey_Tab,       // for tabbing through fields
    ImGuiKey_LeftArrow, // for text edit
    ImGuiKey_RightArrow,// for text edit
    ImGuiKey_UpArrow,   // for text edit
    ImGuiKey_DownArrow, // for text edit
    ImGuiKey_PageUp,
    ImGuiKey_PageDown,
    ImGuiKey_Home,      // for text edit
    ImGuiKey_End,       // for text edit
    ImGuiKey_Delete,    // for text edit
    ImGuiKey_Backspace, // for text edit
    ImGuiKey_Enter,     // for text edit
    ImGuiKey_Escape,    // for text edit
    ImGuiKey_A,         // for text edit CTRL+A: select all
    ImGuiKey_C,         // for text edit CTRL+C: copy
    ImGuiKey_V,         // for text edit CTRL+V: paste
    ImGuiKey_X,         // for text edit CTRL+X: cut
    ImGuiKey_Y,         // for text edit CTRL+Y: redo
    ImGuiKey_Z,         // for text edit CTRL+Z: undo
    ImGuiKey_COUNT
};

// Enumeration for PushStyleColor() / PopStyleColor()
enum ImGuiCol_
{
    ImGuiCol_Text,
    ImGuiCol_TextDisabled,
    ImGuiCol_WindowBg,              // Background of normal windows
    ImGuiCol_ChildWindowBg,         // Background of child windows
    ImGuiCol_PopupBg,               // Background of popups, menus, tooltips windows
    ImGuiCol_Border,
    ImGuiCol_BorderShadow,
    ImGuiCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
    ImGuiCol_FrameBgHovered,
    ImGuiCol_FrameBgActive,
    ImGuiCol_TitleBg,
    ImGuiCol_TitleBgCollapsed,
    ImGuiCol_TitleBgActive,
    ImGuiCol_MenuBarBg,
    ImGuiCol_ScrollbarBg,
    ImGuiCol_ScrollbarGrab,
    ImGuiCol_ScrollbarGrabHovered,
    ImGuiCol_ScrollbarGrabActive,
    ImGuiCol_ComboBg,
    ImGuiCol_CheckMark,
    ImGuiCol_SliderGrab,
    ImGuiCol_SliderGrabActive,
    ImGuiCol_Button,
    ImGuiCol_ButtonHovered,
    ImGuiCol_ButtonActive,
    ImGuiCol_Header,
    ImGuiCol_HeaderHovered,
    ImGuiCol_HeaderActive,
    ImGuiCol_Column,
    ImGuiCol_ColumnHovered,
    ImGuiCol_ColumnActive,
    ImGuiCol_ResizeGrip,
    ImGuiCol_ResizeGripHovered,
    ImGuiCol_ResizeGripActive,
    ImGuiCol_CloseButton,
    ImGuiCol_CloseButtonHovered,
    ImGuiCol_CloseButtonActive,
    ImGuiCol_PlotLines,
    ImGuiCol_PlotLinesHovered,
    ImGuiCol_PlotHistogram,
    ImGuiCol_PlotHistogramHovered,
    ImGuiCol_TextSelectedBg,
    ImGuiCol_ModalWindowDarkening,  // darken entire screen when a modal window is active
    ImGuiCol_COUNT
};

// Enumeration for PushStyleVar() / PopStyleVar()
// NB: the enum only refers to fields of ImGuiStyle() which makes sense to be pushed/poped in UI code. Feel free to add others.
enum ImGuiStyleVar_
{
    ImGuiStyleVar_Alpha,               // float
    ImGuiStyleVar_WindowPadding,       // ImVec2
    ImGuiStyleVar_WindowRounding,      // float
    ImGuiStyleVar_WindowMinSize,       // ImVec2
    ImGuiStyleVar_ChildWindowRounding, // float
    ImGuiStyleVar_FramePadding,        // ImVec2
    ImGuiStyleVar_FrameRounding,       // float
    ImGuiStyleVar_ItemSpacing,         // ImVec2
    ImGuiStyleVar_ItemInnerSpacing,    // ImVec2
    ImGuiStyleVar_IndentSpacing,       // float
    ImGuiStyleVar_GrabMinSize          // float
};

enum ImGuiAlign_
{
    ImGuiAlign_Left     = 1 << 0,
    ImGuiAlign_Center   = 1 << 1,
    ImGuiAlign_Right    = 1 << 2,
    ImGuiAlign_Top      = 1 << 3,
    ImGuiAlign_VCenter  = 1 << 4,
    ImGuiAlign_Default  = ImGuiAlign_Left | ImGuiAlign_Top
};

// Enumeration for ColorEditMode()
// FIXME-OBSOLETE: Will be replaced by future color/picker api
enum ImGuiColorEditMode_
{
    ImGuiColorEditMode_UserSelect = -2,
    ImGuiColorEditMode_UserSelectShowButton = -1,
    ImGuiColorEditMode_RGB = 0,
    ImGuiColorEditMode_HSV = 1,
    ImGuiColorEditMode_HEX = 2
};

// Enumeration for GetMouseCursor()
enum ImGuiMouseCursor_
{
    ImGuiMouseCursor_Arrow = 0,
    ImGuiMouseCursor_TextInput,
    ImGuiMouseCursor_Move,
    ImGuiMouseCursor_ResizeNS,
    ImGuiMouseCursor_ResizeEW,
    ImGuiMouseCursor_ResizeNESW,
    ImGuiMouseCursor_ResizeNWSE,
    ImGuiMouseCursor_Count_
};

// Condition flags for ImGui::SetWindow***(), SetNextWindow***(), SetNextTreeNode***() functions
// All those functions treat 0 as a shortcut to ImGuiSetCond_Always
enum ImGuiSetCond_
{
    ImGuiSetCond_Always        = 1 << 0,
    ImGuiSetCond_Once          = 1 << 1,
    ImGuiSetCond_FirstUseEver  = 1 << 2,
    ImGuiSetCond_Appearing     = 1 << 3
};

struct ImGuiStyle
{
    float               Alpha;
    struct ImVec2       WindowPadding;
    struct ImVec2       WindowMinSize;
    float               WindowRounding;
    ImGuiAlign          WindowTitleAlign;
    float               ChildWindowRounding;
    struct ImVec2       FramePadding;
    float               FrameRounding;
    struct ImVec2       ItemSpacing;
    struct ImVec2       ItemInnerSpacing;
    struct ImVec2       TouchExtraPadding;
    float               IndentSpacing;
    float               ColumnsMinSpacing;
    float               ScrollbarSize;
    float               ScrollbarRounding;
    float               GrabMinSize;
    float               GrabRounding;
    struct ImVec2       DisplayWindowPadding;
    struct ImVec2       DisplaySafeAreaPadding;
    bool                AntiAliasedLines;
    bool                AntiAliasedShapes;
    float               CurveTessellationTol;
    struct ImVec4       Colors[ImGuiCol_COUNT];
};

// This is where your app communicate with ImGui. Access via ImGui::GetIO().
// Read 'Programmer guide' section in .cpp file for general usage.
struct ImGuiIO
{
    //------------------------------------------------------------------
    // Settings (fill once)                 // Default value:
    //------------------------------------------------------------------

    struct ImVec2 DisplaySize;              // <unset>
    float         DeltaTime;                // = 1.0f/60.0f
    float         IniSavingRate;            // = 5.0f
    const char*   IniFilename;              // = "imgui.ini"
    const char*   LogFilename;              // = "imgui_log.txt"
    float         MouseDoubleClickTime;     // = 0.30f
    float         MouseDoubleClickMaxDist;  // = 6.0f
    float         MouseDragThreshold;       // = 6.0f
    int           KeyMap[ImGuiKey_COUNT];   // <unset>
    float         KeyRepeatDelay;           // = 0.250f
    float         KeyRepeatRate;            // = 0.020f
    void*         UserData;                 // = NULL

    struct ImFontAtlas* Fonts;              // <auto>
    float         FontGlobalScale;          // = 1.0f
    bool          FontAllowUserScaling;     // = false
    struct ImVec2 DisplayFramebufferScale;  // = (1.0f,1.0f)
    struct ImVec2 DisplayVisibleMin;        // <unset> (0.0f,0.0f)
    struct ImVec2 DisplayVisibleMax;        // <unset> (0.0f,0.0f)

    // Advanced/subtle behavior
    bool          WordMovementUsesAltKey;   // = defined(__APPLE__)
    bool          ShortcutsUseSuperKey;     // = defined(__APPLE__)
    bool          DoubleClickSelectsWord;   // = defined(__APPLE__)
    bool          MultiSelectUsesSuperKey;  // = defined(__APPLE__)

    //------------------------------------------------------------------
    // User Functions
    //------------------------------------------------------------------

    // Rendering function, will be called in Render().
    // Alternatively you can keep this to NULL and call GetDrawData() after Render() to get the same pointer.
    // See example applications if you are unsure of how to implement this.
    void        (*RenderDrawListsFn)(struct ImDrawData* data);

    // Optional: access OS clipboard
    // (default to use native Win32 clipboard on Windows, otherwise uses a private clipboard. Override to access OS clipboard on other architectures)
    const char* (*GetClipboardTextFn)();
    void        (*SetClipboardTextFn)(const char* text);

    // Optional: override memory allocations. MemFreeFn() may be called with a NULL pointer.
    // (default to posix malloc/free)
    void*       (*MemAllocFn)(size_t sz);
    void        (*MemFreeFn)(void* ptr);

    // Optional: notify OS Input Method Editor of the screen position of your cursor for text input position (e.g. when using Japanese/Chinese IME in Windows)
    // (default to use native imm32 api on Windows)
    void        (*ImeSetInputScreenPosFn)(int x, int y);
    void*       ImeWindowHandle;            // (Windows) Set this to your HWND to get automatic IME cursor positioning.

    //------------------------------------------------------------------
    // Input - Fill before calling NewFrame()
    //------------------------------------------------------------------

    struct ImVec2 MousePos;                 // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    bool        MouseDown[5];               // Mouse buttons: left, right, middle + extras. ImGui itself mostly only uses left button (BeginPopupContext** are using right button). Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
    float       MouseWheel;                 // Mouse wheel: 1 unit scrolls about 5 lines text.
    bool        MouseDrawCursor;            // Request ImGui to draw a mouse cursor for you (if you are on a platform without a mouse cursor).
    bool        KeyCtrl;                    // Keyboard modifier pressed: Control
    bool        KeyShift;                   // Keyboard modifier pressed: Shift
    bool        KeyAlt;                     // Keyboard modifier pressed: Alt
    bool        KeySuper;                   // Keyboard modifier pressed: Cmd/Super/Windows
    bool        KeysDown[512];              // Keyboard keys that are pressed (in whatever storage order you naturally have access to keyboard data)
    ImWchar     InputCharacters[16+1];      // List of characters input (translated by user from keypress+keyboard state). Fill using AddInputCharacter() helper.

    //------------------------------------------------------------------
    // Output - Retrieve after calling NewFrame(), you can use them to discard inputs or hide them from the rest of your application
    //------------------------------------------------------------------

    bool        WantCaptureMouse;           // Mouse is hovering a window or widget is active (= ImGui will use your mouse input)
    bool        WantCaptureKeyboard;        // Widget is active (= ImGui will use your keyboard input)
    bool        WantTextInput;              // Some text input widget is active, which will read input characters from the InputCharacters array.
    float       Framerate;                  // Framerate estimation, in frame per second. Rolling average estimation based on IO.DeltaTime over 120 frames
    int         MetricsAllocs;              // Number of active memory allocations
    int         MetricsRenderVertices;      // Vertices output during last call to Render()
    int         MetricsRenderIndices;       // Indices output during last call to Render() = number of triangles * 3
    int         MetricsActiveWindows;       // Number of visible windows (exclude child windows)

    //------------------------------------------------------------------
    // [Internal] ImGui will maintain those fields for you
    //------------------------------------------------------------------

    struct ImVec2 MousePosPrev;               // Previous mouse position
    struct ImVec2 MouseDelta;                 // Mouse delta. Note that this is zero if either current or previous position are negative to allow mouse enabling/disabling.
    bool        MouseClicked[5];            // Mouse button went from !Down to Down
    struct ImVec2 MouseClickedPos[5];         // Position at time of clicking
    float       MouseClickedTime[5];        // Time of last click (used to figure out double-click)
    bool        MouseDoubleClicked[5];      // Has mouse button been double-clicked?
    bool        MouseReleased[5];           // Mouse button went from Down to !Down
    bool        MouseDownOwned[5];          // Track if button was clicked inside a window. We don't request mouse capture from the application if click started outside ImGui bounds.
    float       MouseDownDuration[5];       // Duration the mouse button has been down (0.0f == just clicked)
    float       MouseDownDurationPrev[5];   // Previous time the mouse button has been down
    float       MouseDragMaxDistanceSqr[5]; // Squared maximum distance of how much mouse has traveled from the click point
    float       KeysDownDuration[512];      // Duration the keyboard key has been down (0.0f == just pressed)
    float       KeysDownDurationPrev[512];  // Previous duration the key has been down
};

#endif /* IMGUI_VERSION */

#endif /* _CIMGUI_H_ */

