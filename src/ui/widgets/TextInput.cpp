#include "TextInput.hpp"

TextInput::TextInput(float x, float y, float width, float height)
    : bounds{x, y, width, height}, text(""), focused(false), isPassword(false), maxLength(64) {
}

void TextInput::handleEvent() {
    Vector2 mousePos = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        focused = CheckCollisionPointRec(mousePos, bounds);
    }

    if (focused) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125)) {
                if (text.length() < maxLength) {
                    text += (char)key;
                }
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
            text.pop_back();
        }
    }
}

void TextInput::draw() {
    Color bgColor = focused ? (Color){50, 50, 70, 255} : (Color){30, 30, 40, 255};
    DrawRectangleRounded(bounds, 0.2f, 8, bgColor);
    DrawRectangleLinesEx(bounds, 2, focused ? (Color){100, 150, 255, 255} : (Color){100, 100, 100, 255});

    if (!text.empty()) {
        std::string displayText = isPassword ? std::string(text.length(), '*') : text;
        DrawTextEx(GetFontDefault(), displayText.c_str(),
                   {bounds.x + 10, bounds.y + bounds.height / 2 - 10},
                   18, 1, WHITE);
    }

    if (focused) {
        std::string displayText = isPassword ? std::string(text.length(), '*') : text;
        float textWidth = MeasureTextEx(GetFontDefault(), displayText.c_str(), 18, 1).x;
        float cursorX = bounds.x + 10 + textWidth;
        float cursorY = bounds.y + bounds.height / 2 - 10;
        DrawLine((int)cursorX, (int)cursorY, (int)cursorX, (int)(cursorY + 18), WHITE);
    }
}

