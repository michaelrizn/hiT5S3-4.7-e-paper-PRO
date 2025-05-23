#pragma once

#include <epdiy.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "lvgl.h"
#include <dirent.h>

// Отображает список приложений из папки /factory/main/apps
void apps_list_app(lv_obj_t *parent); 