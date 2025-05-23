#include "helloworld_app.h"

// Макросы WAVEFORM и DEMO_BOARD уже определены в main.cpp
// #define WAVEFORM EPD_BUILTIN_WAVEFORM
// #define DEMO_BOARD epd_board_v7

// Объявление глобальной переменной hl для доступа
extern EpdiyHighlevelState hl;

void helloworld_app(lv_obj_t *parent) {
    // Создаем текстовые метки с использованием LVGL
    
    // Основной контейнер для содержимого приложения
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(88)); // Уменьшаем высоту до ~88% экрана
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0); // Выравниваем по нижнему краю
    
    // Создаем метки для каждой строки текста
    const char* lines[] = {"Hello, World!", "My first Software", "on E-paper display"};
    
    // Вертикальное размещение строк в центре экрана
    for (int i = 0; i < 3; i++) {
        lv_obj_t *label = lv_label_create(cont);
        lv_label_set_text(label, lines[i]);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, -30 + i * 50); // Центрирование с вертикальным смещением
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    }
} 