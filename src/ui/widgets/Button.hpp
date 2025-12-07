#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "raylib.h"
#include <string>

class Button {
public:
    Button(float x, float y, float width, float height, const std::string& text);
    
    void update();
    void draw();
    bool isClicked() const;
    bool isHovered() const { return hovered; }

    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setText(const std::string& text);

private:
    Rectangle bounds;
    std::string text;
    bool hovered;
    bool pressed;
    bool clicked;
};

#endif // BUTTON_HPP

