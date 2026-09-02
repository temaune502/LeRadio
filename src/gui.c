#include "gui.h"

// --- Layout System ---

Layout LayoutBegin(Rectangle bounds, LayoutDir dir, float spacing) {
    return (Layout){ bounds, dir, spacing, 0.0f };
}

Rectangle LayoutNext(Layout *l, float size) {
    Rectangle rect = l->bounds;

    if (l->dir == LAYOUT_VERTICAL) {
        float h = size;
        if (size < 0.0f) h = l->bounds.height * (-size);
        else if (size == 0.0f) h = (l->bounds.height - l->cursor > 0) ? (l->bounds.height - l->cursor) : 0;

        rect.y += l->cursor;
        rect.height = h;
        l->cursor += h + l->spacing;
    } else {
        float w = size;
        if (size < 0.0f) w = l->bounds.width * (-size);
        else if (size == 0.0f) w = (l->bounds.width - l->cursor > 0) ? (l->bounds.width - l->cursor) : 0;

        rect.x += l->cursor;
        rect.width = w;
        l->cursor += w + l->spacing;
    }

    return rect;
}

// --- Widgets ---

void GuiLabel(Rectangle bounds, const char *text, Color color) {
    int fontSize = 18;
    int textWidth = MeasureText(text, fontSize);
    
    // Центрування тексту по осях X та Y
    int posX = bounds.x + (bounds.width - textWidth) / 2;
    int posY = bounds.y + (bounds.height - fontSize) / 2;
    
    DrawText(text, posX, posY, fontSize, color);
}

bool GuiButton(Rectangle bounds, const char *text) {
    Vector2 mousePos = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, bounds);
    
    // Дія спрацьовує, коли кнопку відпустили над віджетом
    bool clicked = hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT); 
    bool down = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    Color bg = hovered ? (down ? MAROON : GRAY) : DARKGRAY;
    
    DrawRectangleRec(bounds, bg);
    DrawRectangleLinesEx(bounds, 1, LIGHTGRAY);
    GuiLabel(bounds, text, WHITE); // Використовуємо наш Label для тексту

    return clicked;
}

bool GuiCheckbox(Rectangle bounds, const char *text, bool *checked) {
    Vector2 mousePos = GetMousePosition();
    int fontSize = 18;
    int textWidth = MeasureText(text, fontSize);
    
    // Робимо клікабельною і зону самого квадратика, і тексту поруч
    Rectangle clickArea = { bounds.x, bounds.y, bounds.width + 10 + textWidth, bounds.height };
    
    bool hovered = CheckCollisionPointRec(mousePos, clickArea);
    bool clicked = hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    if (clicked) {
        *checked = !(*checked); // Інвертуємо стан
    }

    DrawRectangleRec(bounds, hovered ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(bounds, 1, LIGHTGRAY);

    // Малюємо заповнений квадрат, якщо активно
    if (*checked) {
        Rectangle checkRect = { bounds.x + 4, bounds.y + 4, bounds.width - 8, bounds.height - 8 };
        DrawRectangleRec(checkRect, RED);
    }

    // Текст справа від чекбокса
    DrawText(text, bounds.x + bounds.width + 10, bounds.y + (bounds.height - fontSize) / 2, fontSize, LIGHTGRAY);

    return clicked;
}

float GuiSlider(Rectangle bounds, float value, float min, float max) {
    Vector2 mousePos = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, bounds);
    
    // Оновлюємо значення поки затиснута кнопка над слайдером
    if (hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        value = min + ((mousePos.x - bounds.x) / bounds.width) * (max - min);
        // Захист від виходу за межі
        if (value < min) value = min;
        if (value > max) value = max;
    }

    DrawRectangleRec(bounds, DARKGRAY);
    
    // Смуга прогресу слайдера
    float progress = ((value - min) / (max - min)) * bounds.width;
    DrawRectangle(bounds.x, bounds.y, progress, bounds.height, RED);
    DrawRectangleLinesEx(bounds, 1, LIGHTGRAY);

    return value;
}

void GuiProgressBar(Rectangle bounds, float value, float min, float max) {
    if (value < min) value = min;
    if (value > max) value = max;

    DrawRectangleRec(bounds, BLACK);
    float progress = ((value - min) / (max - min)) * bounds.width;
    DrawRectangle(bounds.x, bounds.y, progress, bounds.height, DARKBLUE);
    DrawRectangleLinesEx(bounds, 1, GRAY);
}