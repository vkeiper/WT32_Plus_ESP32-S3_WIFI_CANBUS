// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.2.0
// Project name: SquareLine_Project

#include "../ui.h"

void ui_scrnCanbus_screen_init(void)
{
    ui_scrnCanbus = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_scrnCanbus, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_scrnCanbus, lv_color_hex(0x464B55), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_scrnCanbus, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_scrnCanbus, lv_color_hex(0x2D323C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_scrnCanbus, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pnlCanRx = lv_obj_create(ui_scrnCanbus);
    lv_obj_set_width(ui_pnlCanRx, 466);
    lv_obj_set_height(ui_pnlCanRx, 130);
    lv_obj_set_x(ui_pnlCanRx, 7);
    lv_obj_set_y(ui_pnlCanRx, 50);
    lv_obj_clear_flag(ui_pnlCanRx, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_pnlCanRx, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_pnlCanRx, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_pnlCanRx, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_pnlCanRx, lv_color_hex(0x202226), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_pnlCanRx, 155, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_pnlCanRx, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_pnlCanRx, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_pnlCanRx, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_pnlCanRx, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_pnlCanRx, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pnlCanRx, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pnlCanRx, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pnlCanRx, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pnlCanRxHdr = lv_obj_create(ui_pnlCanRx);
    lv_obj_set_height(ui_pnlCanRxHdr, 25);
    lv_obj_set_width(ui_pnlCanRxHdr, lv_pct(100));
    lv_obj_clear_flag(ui_pnlCanRxHdr, LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_pnlCanRxHdr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_pnlCanRxHdr, lv_color_hex(0x545A66), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_pnlCanRxHdr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_pnlCanRxHdr, lv_color_hex(0x6D6E77), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_pnlCanRxHdr, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_pnlCanRxHdr, lv_color_hex(0x313131), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_pnlCanRxHdr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_pnlCanRxHdr, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_pad_left(ui_pnlCanRxHdr, 1, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pnlCanRxHdr, 1, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pnlCanRxHdr, 1, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pnlCanRxHdr, 1, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

    ui_lblCanRxHdr = lv_label_create(ui_pnlCanRxHdr);
    lv_obj_set_width(ui_lblCanRxHdr, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblCanRxHdr, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_lblCanRxHdr, LV_ALIGN_CENTER);
    lv_label_set_text(ui_lblCanRxHdr, "Receive");
    lv_obj_set_style_text_color(ui_lblCanRxHdr, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_lblCanRxHdr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_lblCanRxHdr, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_lblCanRxHdr, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pnlCanRxBody = lv_obj_create(ui_pnlCanRx);
    lv_obj_set_height(ui_pnlCanRxBody, lv_pct(100));
    lv_obj_set_width(ui_pnlCanRxBody, lv_pct(100));
    lv_obj_set_x(ui_pnlCanRxBody, 0);
    lv_obj_set_y(ui_pnlCanRxBody, 26);
    lv_obj_set_align(ui_pnlCanRxBody, LV_ALIGN_TOP_MID);
    lv_obj_set_style_radius(ui_pnlCanRxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_pnlCanRxBody, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_pnlCanRxBody, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_pnlCanRxBody, lv_color_hex(0x202226), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_pnlCanRxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_pnlCanRxBody, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_pnlCanRxBody, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_pnlCanRxBody, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_pnlCanRxBody, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_pnlCanRxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pnlCanRxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pnlCanRxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pnlCanRxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pnlScrnSelCanbus = lv_obj_create(ui_scrnCanbus);
    lv_obj_set_width(ui_pnlScrnSelCanbus, 140);
    lv_obj_set_height(ui_pnlScrnSelCanbus, 135);
    lv_obj_set_x(ui_pnlScrnSelCanbus, 0);
    lv_obj_set_y(ui_pnlScrnSelCanbus, -100);
    lv_obj_clear_flag(ui_pnlScrnSelCanbus, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_pnlScrnSelCanbus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_pnlScrnSelCanbus, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_pnlScrnSelCanbus, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_pnlScrnSelCanbus, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_pnlScrnSelCanbus, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_pnlScrnSelCanbus, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_pnlScrnSelCanbus, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_pnlScrnSelCanbus, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_pnlScrnSelCanbus, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_pnlScrnSelCanbus, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_pnlScrnSelCanbus, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_pnlScrnSelCanbus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pnlScrnSelCanbus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pnlScrnSelCanbus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pnlScrnSelCanbus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_pad_left(ui_pnlScrnSelCanbus, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pnlScrnSelCanbus, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pnlScrnSelCanbus, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pnlScrnSelCanbus, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

    ui_btnToMainCan = lv_btn_create(ui_pnlScrnSelCanbus);
    lv_obj_set_width(ui_btnToMainCan, lv_pct(100));
    lv_obj_set_height(ui_btnToMainCan, lv_pct(33));
    lv_obj_add_flag(ui_btnToMainCan, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_btnToMainCan, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_btnToMainCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_btnToMainCan, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_btnToMainCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_btnToMainCan, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_btnToMainCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_lblToMainCan = lv_label_create(ui_btnToMainCan);
    lv_obj_set_width(ui_lblToMainCan, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblToMainCan, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_lblToMainCan, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_lblToMainCan, "Main");

    ui_btnToManuCan = lv_btn_create(ui_pnlScrnSelCanbus);
    lv_obj_set_width(ui_btnToManuCan, lv_pct(100));
    lv_obj_set_height(ui_btnToManuCan, lv_pct(33));
    lv_obj_set_x(ui_btnToManuCan, lv_pct(0));
    lv_obj_set_y(ui_btnToManuCan, lv_pct(33));
    lv_obj_add_flag(ui_btnToManuCan, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_btnToManuCan, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_btnToManuCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_btnToManuCan, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_btnToManuCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_btnToManuCan, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_btnToManuCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_lblToManuCan = lv_label_create(ui_btnToManuCan);
    lv_obj_set_width(ui_lblToManuCan, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblToManuCan, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_lblToManuCan, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_lblToManuCan, "Manual");

    ui_btnToFltCan = lv_btn_create(ui_pnlScrnSelCanbus);
    lv_obj_set_width(ui_btnToFltCan, lv_pct(100));
    lv_obj_set_height(ui_btnToFltCan, lv_pct(33));
    lv_obj_set_x(ui_btnToFltCan, lv_pct(0));
    lv_obj_set_y(ui_btnToFltCan, lv_pct(66));
    lv_obj_add_flag(ui_btnToFltCan, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_btnToFltCan, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_btnToFltCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_btnToFltCan, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_btnToFltCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_btnToFltCan, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_btnToFltCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_lblToFltCan = lv_label_create(ui_btnToFltCan);
    lv_obj_set_width(ui_lblToFltCan, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblToFltCan, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_lblToFltCan, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_lblToFltCan, "Faults");

    ui_pnlHdrCan = lv_obj_create(ui_scrnCanbus);
    lv_obj_set_height(ui_pnlHdrCan, 46);
    lv_obj_set_width(ui_pnlHdrCan, lv_pct(100));
    lv_obj_clear_flag(ui_pnlHdrCan, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_pnlHdrCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_pnlHdrCan, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_pnlHdrCan, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_pnlHdrCan, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_pnlHdrCan, lv_color_hex(0x3B3B3B), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_pnlHdrCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_pnlHdrCan, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_pnlHdrCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_pnlHdrCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pnlHdrCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pnlHdrCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pnlHdrCan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_lblHdrCan = lv_label_create(ui_pnlHdrCan);
    lv_obj_set_width(ui_lblHdrCan, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblHdrCan, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblHdrCan, 43);
    lv_obj_set_y(ui_lblHdrCan, 0);
    lv_obj_set_align(ui_lblHdrCan, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_lblHdrCan, "Astrodyne TDI eLink2");
    lv_obj_set_style_text_color(ui_lblHdrCan, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_lblHdrCan, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_swchScrnCanbus = lv_switch_create(ui_pnlHdrCan);
    lv_obj_set_width(ui_swchScrnCanbus, 40);
    lv_obj_set_height(ui_swchScrnCanbus, 40);
    lv_obj_set_style_bg_color(ui_swchScrnCanbus, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_swchScrnCanbus, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_swchScrnCanbus, &ui_img_hamburger_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_swchScrnCanbus, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_swchScrnCanbus, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_swchScrnCanbus, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_swchScrnCanbus, 0, LV_PART_INDICATOR | LV_STATE_CHECKED);

    lv_obj_set_style_bg_color(ui_swchScrnCanbus, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_swchScrnCanbus, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_lblModeSelTx = lv_label_create(ui_pnlHdrCan);
    lv_obj_set_width(ui_lblModeSelTx, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblModeSelTx, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblModeSelTx, 215);
    lv_obj_set_y(ui_lblModeSelTx, 0);
    lv_obj_set_align(ui_lblModeSelTx, LV_ALIGN_CENTER);
    lv_label_set_text(ui_lblModeSelTx, "TX");
    lv_obj_set_style_text_color(ui_lblModeSelTx, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_lblModeSelTx, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_lblModeSelTx, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_sldrModeSel = lv_slider_create(ui_pnlHdrCan);
    lv_slider_set_range(ui_sldrModeSel, 0, 2);
    lv_slider_set_mode(ui_sldrModeSel, LV_SLIDER_MODE_SYMMETRICAL);
    lv_slider_set_value(ui_sldrModeSel, 1, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_sldrModeSel) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_sldrModeSel, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_sldrModeSel, 100);
    lv_obj_set_height(ui_sldrModeSel, 10);
    lv_obj_set_x(ui_sldrModeSel, -50);
    lv_obj_set_y(ui_sldrModeSel, 0);
    lv_obj_set_align(ui_sldrModeSel, LV_ALIGN_RIGHT_MID);
    lv_obj_set_style_bg_color(ui_sldrModeSel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_sldrModeSel, 120, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_sldrModeSel, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_sldrModeSel, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_lblModeSelRx = lv_label_create(ui_pnlHdrCan);
    lv_obj_set_width(ui_lblModeSelRx, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblModeSelRx, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblModeSelRx, 60);
    lv_obj_set_y(ui_lblModeSelRx, 0);
    lv_obj_set_align(ui_lblModeSelRx, LV_ALIGN_CENTER);
    lv_label_set_text(ui_lblModeSelRx, "RX");
    lv_obj_set_style_text_color(ui_lblModeSelRx, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_lblModeSelRx, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_lblModeSelRx, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pnlCanTx = lv_obj_create(ui_scrnCanbus);
    lv_obj_set_width(ui_pnlCanTx, 466);
    lv_obj_set_height(ui_pnlCanTx, 130);
    lv_obj_set_x(ui_pnlCanTx, 7);
    lv_obj_set_y(ui_pnlCanTx, 185);
    lv_obj_clear_flag(ui_pnlCanTx, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_pnlCanTx, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_pnlCanTx, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_pnlCanTx, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_pnlCanTx, lv_color_hex(0x202226), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_pnlCanTx, 155, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_pnlCanTx, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_pnlCanTx, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_pnlCanTx, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_pnlCanTx, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_pnlCanTx, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pnlCanTx, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pnlCanTx, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pnlCanTx, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pnlCanTxHdr = lv_obj_create(ui_pnlCanTx);
    lv_obj_set_height(ui_pnlCanTxHdr, 25);
    lv_obj_set_width(ui_pnlCanTxHdr, lv_pct(100));
    lv_obj_clear_flag(ui_pnlCanTxHdr, LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_pnlCanTxHdr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_pnlCanTxHdr, lv_color_hex(0x545A66), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_pnlCanTxHdr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_pnlCanTxHdr, lv_color_hex(0x6D6E77), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_pnlCanTxHdr, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_pnlCanTxHdr, lv_color_hex(0x313131), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_pnlCanTxHdr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_pnlCanTxHdr, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_pad_left(ui_pnlCanTxHdr, 1, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pnlCanTxHdr, 1, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pnlCanTxHdr, 1, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pnlCanTxHdr, 1, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

    ui_lblCanTxHdr = lv_label_create(ui_pnlCanTxHdr);
    lv_obj_set_width(ui_lblCanTxHdr, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblCanTxHdr, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_lblCanTxHdr, LV_ALIGN_CENTER);
    lv_label_set_text(ui_lblCanTxHdr, "Transmit");
    lv_obj_set_style_text_color(ui_lblCanTxHdr, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_lblCanTxHdr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_lblCanTxHdr, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_lblCanTxHdr, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pnlCanTxBody = lv_obj_create(ui_pnlCanTx);
    lv_obj_set_height(ui_pnlCanTxBody, lv_pct(100));
    lv_obj_set_width(ui_pnlCanTxBody, lv_pct(100));
    lv_obj_set_x(ui_pnlCanTxBody, 0);
    lv_obj_set_y(ui_pnlCanTxBody, 26);
    lv_obj_set_align(ui_pnlCanTxBody, LV_ALIGN_TOP_MID);
    lv_obj_set_style_radius(ui_pnlCanTxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_pnlCanTxBody, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_pnlCanTxBody, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_pnlCanTxBody, lv_color_hex(0x202226), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_pnlCanTxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_pnlCanTxBody, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_pnlCanTxBody, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_pnlCanTxBody, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_pnlCanTxBody, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_pnlCanTxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pnlCanTxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pnlCanTxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pnlCanTxBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);


    ui_tblCanRx = lv_table_create(ui_pnlCanRxBody);
    lv_obj_set_width(ui_tblCanRx, lv_pct(100));
    lv_obj_set_height(ui_tblCanRx, lv_pct(100));
    lv_obj_set_align(ui_tblCanRx, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_bg_color(ui_tblCanRx, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_tblCanRx, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_tblCanRx, 1, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_tblCanRx, 1, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_tblCanRx, 1, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_tblCanRx, 1, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_table_set_cell_value(ui_tblCanRx, 0, 0, "CAN ID");
    lv_table_set_cell_value(ui_tblCanRx, 0, 1, "DATA");
    lv_table_set_cell_value(ui_tblCanRx, 0, 2, "CYCLE TIME");
    lv_table_set_cell_value(ui_tblCanRx, 0, 3, "COUNT");
    lv_table_set_cell_value(ui_tblCanRx, 1, 0, "0000103A");
    lv_table_set_cell_value(ui_tblCanRx, 1, 1, "00 00 00 00 00 00 00 00");
    lv_table_set_cell_value(ui_tblCanRx, 1, 2, "1000");
    lv_table_set_cell_value(ui_tblCanRx, 1, 3, "119");
    lv_table_set_cell_value(ui_tblCanRx, 2, 0, "0000103A");
    lv_table_set_cell_value(ui_tblCanRx, 2, 1, "00 00 00 00 00 00 00 00");
    lv_table_set_cell_value(ui_tblCanRx, 2, 2, "1000");
    lv_table_set_cell_value(ui_tblCanRx, 2, 3, "119");
    lv_table_set_col_width(ui_tblCanRx, 0, 74);
    lv_table_set_col_width(ui_tblCanRx, 1, 174);
    lv_table_set_col_width(ui_tblCanRx, 2, 140);
    lv_table_set_col_width(ui_tblCanRx, 3, 60);


    ui_tblCanTx = lv_table_create(ui_pnlCanTxBody);
    lv_obj_set_width(ui_tblCanTx, lv_pct(100));
    lv_obj_set_height(ui_tblCanTx, lv_pct(100));
    lv_obj_set_align(ui_tblCanTx, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_bg_color(ui_tblCanTx, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_tblCanTx, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_tblCanTx, 1, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_tblCanTx, 1, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_tblCanTx, 1, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_tblCanTx, 1, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_table_set_cell_value(ui_tblCanTx, 0, 0, "CAN ID");
    lv_table_set_cell_value(ui_tblCanTx, 0, 1, "DATA");
    lv_table_set_cell_value(ui_tblCanTx, 0, 2, "CYCLE TIME");
    lv_table_set_cell_value(ui_tblCanTx, 0, 3, "COUNT");
    lv_table_set_cell_value(ui_tblCanTx, 1, 0, "0000103A");
    lv_table_set_cell_value(ui_tblCanTx, 1, 1, "00 00 00 00 00 00 00 00");
    lv_table_set_cell_value(ui_tblCanTx, 1, 2, "1000");
    lv_table_set_cell_value(ui_tblCanTx, 1, 3, "119");
    lv_table_set_cell_value(ui_tblCanTx, 2, 0, "0000103A");
    lv_table_set_cell_value(ui_tblCanTx, 2, 1, "00 00 00 00 00 00 00 00");
    lv_table_set_cell_value(ui_tblCanTx, 2, 2, "1000");
    lv_table_set_cell_value(ui_tblCanTx, 2, 3, "119");
    lv_table_set_col_width(ui_tblCanTx, 0, 74);
    lv_table_set_col_width(ui_tblCanTx, 1, 174);
    lv_table_set_col_width(ui_tblCanTx, 2, 38);
    lv_table_set_col_width(ui_tblCanTx, 3, 38);


    lv_obj_add_event_cb(ui_btnToMainCan, ui_event_btnToMainCan, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnToManuCan, ui_event_btnToManuCan, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnToFltCan, ui_event_btnToFltCan, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_swchScrnCanbus, ui_event_swchScrnCanbus, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_sldrModeSel, ui_event_sldrModeSel, LV_EVENT_ALL, NULL);

}
