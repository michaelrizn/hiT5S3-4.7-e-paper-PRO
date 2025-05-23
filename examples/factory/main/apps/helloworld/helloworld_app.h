#pragma once

#include <epdiy.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "lvgl.h"

#include "firasans_12.h"
#include "firasans_20.h"

// Отображает текст Hello World на e-paper дисплее
void helloworld_app(lv_obj_t *parent); 