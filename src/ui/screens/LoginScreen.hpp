#ifndef LOGIN_SCREEN_HPP
#define LOGIN_SCREEN_HPP

#include "../Screen.hpp"
#include "../widgets/TextInput.hpp"
#include "../widgets/Button.hpp"
#include "../../app/App.hpp"
#include <string>

class LoginScreen : public Screen {
public:
    explicit LoginScreen(App& app);
    void handleEvent() override;
    void update(float dt) override;
    void draw() override;

private:
    App& app;
    TextInput usernameInput;
    TextInput passwordInput;
    Button loginButton;
    std::string errorMessage;
};

#endif // LOGIN_SCREEN_HPP

