#include "mgos.h"
#include "led_master.h"

static int direction = 0;

static void mgos_intern_rotate_columns_init( mgos_rgbleds* leds) {
    leds->timeout = mgos_sys_config_get_ledeffects_rotate_columns_timeout();
    leds->dim_all = mgos_sys_config_get_ledeffects_rotate_columns_dim_all();
    direction = mgos_sys_config_get_ledeffects_rotate_columns_direction();
    leds->pix_pos = direction > 0 ? 0 : leds->pic_width - 1 - leds->panel_width;
    leds->fit_horz = mgos_sys_config_get_ledeffects_rotate_columns_fit_horz();
    leds->fit_vert = mgos_sys_config_get_ledeffects_rotate_columns_fit_vert();
    leds->color_file = mgos_sys_config_get_ledeffects_rotate_columns_color_file();
    
    mgos_universal_led_get_bitmap(leds);
    mgos_universal_led_scale_bitmap(leds);
    
    mgos_universal_led_log_data(leds, LL_DEBUG, "[DATA] Rotate Columns LEDs");
}

static void mgos_intern_rotate_columns_loop(mgos_rgbleds* leds)
{
    LOG(LL_VERBOSE_DEBUG, ("Rotate columns: offset = %d", leds->pix_pos));
    mgos_universal_led_set_from_buffer(leds, leds->pix_pos, 0, true, mgos_sys_config_get_ledeffects_rotate_columns_toggle_odd());
    mgos_universal_led_show(leds);
    
    leds->pix_pos += direction;
    leds->pix_pos = leds->pix_pos >= leds->pic_width ? 0 : leds->pix_pos < 0 ? leds->pic_width - 1 : leds->pix_pos;
}

void mgos_ledeffects_rotate_columns(void* param, mgos_rgbleds_action action)
{
    static bool do_time = false;
    static uint32_t max_time = 0;
    uint32_t time = (mgos_uptime_micros() / 1000);
    mgos_rgbleds* leds = (mgos_rgbleds*)param;

    switch (action) {
    case MGOS_RGBLEDS_ACT_INIT:
        LOG(LL_INFO, ("led_master_rotate_columns: called (init)"));
        mgos_intern_rotate_columns_init(leds);
        break;
    case MGOS_RGBLEDS_ACT_EXIT:
        LOG(LL_INFO, ("led_master_rotate_columns: called (exit)"));
        break;
    case MGOS_RGBLEDS_ACT_LOOP:
        mgos_intern_rotate_columns_loop(leds);
        if (do_time) {
            time = (mgos_uptime_micros() /1000) - time;
            max_time = (time > max_time) ? time : max_time;
            LOG(LL_VERBOSE_DEBUG, ("Rotate columns loop duration: %d milliseconds, max: %d ...", time / 1000, max_time / 1000));
        }
        break;
    }
}

bool mgos_rotate_columns_init(void) {
  LOG(LL_INFO, ("mgos_rotate_columns_init ..."));
  ledmaster_add_effect("ANIM_ROTATE_COLUMNS", mgos_ledeffects_rotate_columns);
  return true;
}
