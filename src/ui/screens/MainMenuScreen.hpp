#ifndef MAIN_MENU_SCREEN_HPP
#define MAIN_MENU_SCREEN_HPP

#include "../Screen.hpp"
#include "../widgets/Button.hpp"
#include "../../app/App.hpp"

class MainMenuScreen : public Screen {
public:
    explicit MainMenuScreen(App& app);
    void handleEvent() override;
    void update(float dt) override;
    void draw() override;

private:
    App& app;
    Button studentsButton;
    Button coursesButton;
    Button quitButton;
};

#endif // MAIN_MENU_SCREEN_HPP

