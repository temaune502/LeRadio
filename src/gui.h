#ifndef GUI_H
#define GUI_H

#include "raylib.h"
#include <stdbool.h>

// ==========================================
// Layout Engine (Сітка та контейнери)
// ==========================================

typedef enum {
    LAYOUT_VERTICAL,
    LAYOUT_HORIZONTAL
} LayoutDir;

typedef struct {
    Rectangle bounds;
    LayoutDir dir;
    float spacing;
    float cursor;
} Layout;

// Ініціалізує новий контейнер у заданих межах
Layout LayoutBegin(Rectangle bounds, LayoutDir dir, float spacing);

// Виділяє місце під наступний віджет.
// size > 0: точна кількість пікселів.
// size < 0: відсоток (наприклад, -0.5f це 50%).
// size == 0: займає весь вільний залишок контейнера.
Rectangle LayoutNext(Layout *l, float size);


// ==========================================
// Immediate Mode Віджети
// ==========================================

// Простий текст (центрується у вказаному Rectangle)
void GuiLabel(Rectangle bounds, const char *text, Color color);

// Кнопка (повертає true в момент відпускання ЛКМ)
bool GuiButton(Rectangle bounds, const char *text);

// Чекбокс із підписом (змінює значення за вказівником, повертає true при кліку)
bool GuiCheckbox(Rectangle bounds, const char *text, bool *checked);

// Інтерактивний повзунок (повертає нове значення)
float GuiSlider(Rectangle bounds, float value, float min, float max);

// Візуальний індикатор прогресу (без інтеракції)
void GuiProgressBar(Rectangle bounds, float value, float min, float max);

#endif // GUI_H