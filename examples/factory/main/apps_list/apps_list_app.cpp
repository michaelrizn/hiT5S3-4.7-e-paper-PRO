#include "apps_list_app.h"
#include "esp_log.h"
#include "scr_mrg.h"
#include "ui.h"
#include "src/assets.h"
#include <string.h>
#include <ctype.h>

#define TAG "APPS_LIST"

// Структура для хранения информации о приложении
typedef struct {
    const char* name;  // Имя приложения для отображения
    const char* folder; // Имя папки приложения
    int screen_id;     // ID экрана для запуска приложения (если известен)
} app_info_t;

// Обработчик событий при клике на элемент списка
static void app_click_handler(lv_event_t *e)
{
    app_info_t *app_info = (app_info_t *)e->user_data;
    if (app_info) {
        ESP_LOGI(TAG, "Запуск приложения: %s из папки %s, ID=%d", app_info->name, app_info->folder, app_info->screen_id);
        
        // Если ID экрана известен, запускаем приложение через scr_mgr_push
        if (app_info->screen_id > 0) {
            scr_mgr_push(app_info->screen_id, false);
        } else {
            ESP_LOGW(TAG, "Приложение %s не имеет зарегистрированного экрана", app_info->name);
        }
    }
}

// Преобразование имени папки в удобочитаемое имя
static const char* folder_to_display_name(const char* folder)
{
    // Преобразуем snake_case в Title Case
    static char display_name[64];
    strncpy(display_name, folder, sizeof(display_name) - 1);
    display_name[sizeof(display_name) - 1] = '\0';
    
    // Преобразование первой буквы в заглавную
    if (display_name[0])
        display_name[0] = toupper(display_name[0]);
    
    // Замена подчеркиваний на пробелы и вставка заглавных букв
    for (int i = 0; display_name[i]; i++) {
        if (display_name[i] == '_') {
            display_name[i] = ' ';
            if (display_name[i+1])
                display_name[i+1] = toupper(display_name[i+1]);
        }
    }
    
    return display_name;
}

void apps_list_app(lv_obj_t *parent)
{
    // Статический список известных приложений с их ID экранов
    static app_info_t predefined_apps[] = {
        {"Hello World", "helloworld", SCREEN11_ID}
        // Можно добавить другие статические приложения
    };
    
    const int predefined_count = sizeof(predefined_apps) / sizeof(predefined_apps[0]);
    const int max_apps = 20;  // Максимальное количество приложений
    
    // Массив для хранения информации о приложениях
    static app_info_t apps[max_apps]; // Статический массив для сохранения ссылок
    int app_count = 0;
    
    // Копируем предопределенные приложения
    for (int i = 0; i < predefined_count && app_count < max_apps; i++) {
        apps[app_count++] = predefined_apps[i];
    }
    
    // Создаем контейнер для содержимого
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(88)); // Уменьшаем высоту до ~88% экрана
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0); // Выравниваем по нижнему краю
    
    // Пробуем сканировать директорию с приложениями
    DIR *dir;
    struct dirent *entry;
    
    // Пути для поиска приложений
    const char* paths[] = {
        "../apps", // относительный путь от apps_list
        "/examples/factory/main/apps", // абсолютный путь для совместимости
        "/factory/main/apps",
        "/apps"
    };
    
    // Пробуем каждый путь
    bool dir_found = false;
    for (int p = 0; p < sizeof(paths)/sizeof(paths[0]) && !dir_found; p++) {
        dir = opendir(paths[p]);
        if (dir != nullptr) {
            dir_found = true;
            ESP_LOGI(TAG, "Найдена директория приложений: %s", paths[p]);
            
            // Сканируем директорию
            while ((entry = readdir(dir)) != nullptr && app_count < max_apps) {
                ESP_LOGI(TAG, "Найден файл: %s", entry->d_name);
                
                // Пропускаем ".", ".." и "apps_list" (наше собственное приложение)
                if (strcmp(entry->d_name, ".") == 0 || 
                    strcmp(entry->d_name, "..") == 0 || 
                    strcmp(entry->d_name, "apps_list") == 0) {
                    continue;
                }
                
                // Проверяем, не является ли это уже известным приложением
                bool is_known = false;
                for (int i = 0; i < app_count; i++) {
                    if (strcmp(entry->d_name, apps[i].folder) == 0) {
                        is_known = true;
                        break;
                    }
                }
                
                if (!is_known) {
                    // Выделяем память и копируем имя папки
                    char *folder_copy = (char *)malloc(strlen(entry->d_name) + 1);
                    if (folder_copy) {
                        strcpy(folder_copy, entry->d_name);
                        
                        // Добавляем новое приложение (без известного ID экрана)
                        apps[app_count].folder = folder_copy;
                        apps[app_count].name = folder_to_display_name(folder_copy);
                        apps[app_count].screen_id = -1; // Неизвестный ID
                        app_count++;
                    }
                }
            }
            
            closedir(dir);
        }
    }
    
    if (!dir_found) {
        ESP_LOGW(TAG, "Не удалось найти директорию приложений");
    }
    
    // Создаем список
    lv_obj_t *list = lv_list_create(cont);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(list, 10, LV_PART_MAIN);
    
    // Добавляем элементы списка для каждого приложения
    for (int i = 0; i < app_count; i++) {
        // Создаем кнопку в списке
        lv_obj_t *btn = lv_list_add_btn(list, NULL, apps[i].name);
        
        // Настраиваем стиль кнопки
        lv_obj_set_style_text_font(btn, &Font_Mono_Bold_30, LV_PART_MAIN);
        
        // Добавляем обработчик события
        lv_obj_add_event_cb(btn, app_click_handler, LV_EVENT_CLICKED, &apps[i]);
    }
    
    // Если приложения не найдены, показываем сообщение
    if (app_count == 0) {
        lv_obj_t *msg = lv_label_create(cont);
        lv_label_set_text(msg, "Приложения не найдены");
        lv_obj_set_style_text_font(msg, &Font_Mono_Bold_30, LV_PART_MAIN);
        lv_obj_align(msg, LV_ALIGN_CENTER, 0, 0);
    }
} 