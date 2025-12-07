#include "Button.hpp"

Button::Button(float x, float y, float width, float height, const std::string& text)
    : bounds{x, y, width, height}, text(text), hovered(false), pressed(false), clicked(false) {
}

void Button::update() {
    Vector2 mousePos = GetMousePosition();
    hovered = CheckCollisionPointRec(mousePos, bounds);
    
    bool mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    if (hovered && mousePressed) {
        pressed = true;
    }

    if (pressed && mouseReleased) {
        if (hovered) {
            clicked = true;
        }
        pressed = false;
    } else {
        clicked = false;
    }
}

void Button::draw() {
    Color bgColor = hovered ? (Color){100, 150, 255, 255} : (Color){60, 80, 120, 255};
    if (pressed) {
        bgColor = (Color){40, 60, 100, 255};
    }

    DrawRectangleRounded(bounds, 0.3f, 8, bgColor);
    DrawRectangleLinesEx(bounds, 2, (Color){255, 255, 255, 100});

    Vector2 textSize = MeasureTextEx(GetFontDefault(), text.c_str(), 20, 2);
    Vector2 textPos = {
        bounds.x + bounds.width / 2 - textSize.x / 2,
        bounds.y + bounds.height / 2 - textSize.y / 2
    };

    DrawTextEx(GetFontDefault(), text.c_str(), textPos, 20, 2, WHITE);
}

bool Button::isClicked() const {
    return clicked;
}

void Button::setPosition(float x, float y) {
    bounds.x = x;
    bounds.y = y;
}

void Button::setSize(float width, float height) {
    bounds.width = width;
    bounds.height = height;
}

void Button::setText(const std::string& newText) {
    text = newText;
}

