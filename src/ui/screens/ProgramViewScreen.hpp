#ifndef PROGRAM_VIEW_SCREEN_HPP
#define PROGRAM_VIEW_SCREEN_HPP

#include "../Screen.hpp"
#include "../widgets/Button.hpp"
#include "../widgets/SimpleTable.hpp"
#include "../../app/App.hpp"
#include "../../core/models/Program.hpp"
#include "../../core/models/Course.hpp"
#include "../../core/repositories/ProgramRepository.hpp"
#include <vector>
#include <string>

class ProgramViewScreen : public Screen {
public:
    explicit ProgramViewScreen(App& app);
    void handleEvent() override;
    void update(float dt) override;
    void draw() override;

private:
    App& app;

    std::vector<Program> programs;
    int selectedProgramIndex;
    int selectedLevel;

    SimpleTable curriculumTable;
    Button backButton;
    Button levelButtons[4];

    std::vector<ProgramCourseEntry> currentEntries;

    void loadCurriculum();
};

#endif // PROGRAM_VIEW_SCREEN_HPP