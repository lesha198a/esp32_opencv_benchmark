//
// Created by Oleksii on 03.04.2025.
//

#ifndef WDT_ABSTRACT_H
#define WDT_ABSTRACT_H

#define WDT_OFF


////////////////////
///
///
#ifdef WDT_OFF

#define my_task_wdt_reset() do { } while (0)
#define my_task_wdt_add(x) do { } while (0)
#define my_task_wdt_delete(x) do { } while (0)

#else

#include <esp_task_wdt.h>

#define my_task_wdt_reset esp_task_wdt_reset
#define my_task_wdt_add esp_task_wdt_add
#define my_task_wdt_delete esp_task_wdt_delete

#endif

#endif //WDT_ABSTRACT_H
