# Missing Features & Incomplete Tasks

## 🔴 Critical Missing Features

### 1. Missing Screens
- [ ] **CourseListScreen** - Mentioned in design brief, has TODO in code
  - Location: `src/ui/screens/CourseListScreen.{hpp,cpp}`
  - Should display all courses from `CourseRepository`
  - Referenced in `MainMenuScreen` but not implemented
  - Status: `ScreenType::CourseList` exists but screen implementation missing

- [ ] **EnrollmentListScreen** - Mentioned in design brief
  - Location: `src/ui/screens/EnrollmentListScreen.{hpp,cpp}`
  - Should show all enrollments (or filtered enrollments)
  - Status: `ScreenType::EnrollmentList` exists but screen implementation missing

### 2. Missing Widgets
- [ ] **SimpleTable Widget** - Mentioned in design brief section 7.3
  - Location: `src/ui/widgets/SimpleTable.{hpp,cpp}`
  - Should have: `Vector2 position`, `float rowHeight`, `std::vector<std::array<std::string, N>> rows`
  - Would be useful for displaying enrollments, courses in tabular format

### 3. Missing Services
- [ ] **CourseService** - No service layer for courses
  - Should provide: `listCourses()`, `getByCode()`, etc.
  - Currently only `CourseRepository` exists, no business logic layer

- [ ] **EnrollmentService** - No service layer for enrollments
  - Should provide enrollment-related business logic
  - Currently only `EnrollmentRepository` exists

## ⚠️ Security & Code Quality Issues

### 4. SQL Injection Vulnerabilities
**CRITICAL**: All repositories use string concatenation for SQL queries, making them vulnerable to SQL injection.

**Affected Files:**
- `src/core/repositories/StudentRepository.cpp` - Lines 32, 44, 55, 66, 75
- `src/core/repositories/CourseRepository.cpp` - Similar issues
- `src/core/repositories/EnrollmentRepository.cpp` - Similar issues
- `src/core/services/AuthService.cpp` - Lines 6, 25

**Example vulnerable code:**
```cpp
sql << "SELECT ... WHERE student_code = '" << code << "';";
```

**Solution needed:**
- Use SQLite prepared statements (`sqlite3_prepare_v2`, `sqlite3_bind_*`)
- Or at minimum, escape/sanitize user input
- Add parameterized query support to `Database` class

### 5. Password Security
- [ ] Passwords stored in **plain text** (mentioned as "for now" but should be addressed)
- [ ] No password hashing (bcrypt, Argon2, etc.)
- [ ] No password change functionality
- [ ] No password strength validation

## 🟡 Incomplete Functionality

### 6. Quit Button Not Working
- [ ] **MainMenuScreen** - Quit button has comment but doesn't close window
  - Location: `src/ui/screens/MainMenuScreen.cpp:25-27`
  - Current: `// Close window will be handled by main loop`
  - Needed: Actual window close mechanism (set flag or call `CloseWindow()`)

### 7. Missing Error Handling
- [ ] No error messages displayed to user for:
  - Database connection failures (only logged to stderr)
  - Failed queries
  - Invalid user input
- [ ] No validation for:
  - Empty username/password
  - Invalid student codes
  - Out of range values

### 8. Missing Input Validation
- [ ] No validation in `TextInput` widget for:
  - Max length
  - Allowed characters
  - Required fields
- [ ] No validation in repositories before database operations

## 🟢 Nice-to-Have Features (From Design Brief)

### 9. Enhanced UI Features
- [ ] Keyboard shortcuts (e.g., Esc to go back)
- [ ] Better navigation (breadcrumbs, back button on all screens)
- [ ] Loading indicators for database operations
- [ ] Confirmation dialogs for destructive actions

### 10. Additional Functionality
- [ ] Search/filter functionality in list screens
- [ ] Sorting in list screens
- [ ] Pagination for large datasets
- [ ] Export functionality (CSV, PDF)

## 📋 Implementation Priority

### High Priority (Must Have)
1. ✅ Fix SQL injection vulnerabilities (Security critical)
2. ✅ Implement CourseListScreen (Core feature)
3. ✅ Fix Quit button functionality
4. ✅ Add basic input validation

### Medium Priority (Should Have)
5. ✅ Implement EnrollmentListScreen
6. ✅ Add CourseService and EnrollmentService
7. ✅ Implement SimpleTable widget
8. ✅ Add error handling in UI

### Low Priority (Nice to Have)
9. ✅ Password hashing
10. ✅ Enhanced UI features
11. ✅ Search/filter functionality

## 🔧 Quick Fixes Needed

### Immediate Fixes:
1. **Quit Button** - Add window close functionality
2. **CourseListScreen** - Implement basic screen to show courses
3. **SQL Injection** - At minimum, add input sanitization

### Code Locations to Update:
- `src/app/App.cpp:95-100` - Add CourseListScreen and EnrollmentListScreen cases
- `src/ui/screens/MainMenuScreen.cpp:20-23` - Enable Courses button
- `src/core/repositories/*.cpp` - Replace string concatenation with prepared statements
- `src/core/services/AuthService.cpp` - Add password hashing

