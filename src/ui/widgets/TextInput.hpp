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

    // Optional maximum length for input to avoid unbounded growth
    void setMaxLength(size_t max) { maxLength = max; }
    size_t getMaxLength() const { return maxLength; }

private:
    Rectangle bounds;
    std::string text;
    bool focused;
    bool isPassword;
    size_t maxLength;
};

#endif // TEXT_INPUT_HPP

