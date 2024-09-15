#include "sAPP_UI.h"



void sAPP_UI_InitWidghts(){
    label = lv_label_create( lv_scr_act() );
    lv_obj_set_style_text_font(label,&lv_font_montserrat_18,0);
    lv_label_set_text( label, "sGenCARCtrller" );
    lv_obj_align( label, LV_ALIGN_TOP_LEFT, 10, 10 );

    label1 = lv_label_create( lv_scr_act() );
    lv_obj_set_style_text_font(label1,&lv_font_montserrat_18,0);
    lv_label_set_text( label1, "Jie LOVE HHR" );
    lv_obj_align( label1, LV_ALIGN_TOP_LEFT, 10, 50 );
}

