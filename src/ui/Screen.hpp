#ifndef SCREEN_HPP
#define SCREEN_HPP

class Screen {
public:
    virtual ~Screen() = default;
    virtual void handleEvent() = 0;
    virtual void update(float dt) = 0;
    virtual void draw() = 0;
};

#endif // SCREEN_HPP

