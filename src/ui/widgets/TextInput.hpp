#ifndef TEXT_INPUT_HPP
#define TEXT_INPUT_HPP

#include "raylib.h"
#include <string>

class TextInput {
public:
    TextInput(float x, float y, float width, float height);
    
    void handleEvent();
    void draw();
    
    bool isFocused() const { return focused; }
    void setFocused(bool f) { focused = f; }
    const std::string& getText() const { return text; }
    void setText(const std::string& t) { text = t; }
    void setIsPassword(bool p) { isPassword = p; }

private:
    Rectangle bounds;
    std::string text;
    bool focused;
    bool isPassword;
};

#endif // TEXT_INPUT_HPP

