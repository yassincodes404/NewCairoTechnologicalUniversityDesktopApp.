#include "SimpleTable.hpp"
#include <cmath>

SimpleTable::SimpleTable(float x, float y, float rowHeight)
    : position{x, y}, rowHeight(rowHeight), selectedRow(-1), firstVisibleRow(0),
      headerColor((Color){60, 80, 120, 255}),
      rowColor((Color){40, 45, 60, 255}),
      selectedRowColor((Color){100, 150, 255, 255}),
      textColor(WHITE),
      headerTextColor(WHITE) {
}

void SimpleTable::addRow(const std::vector<std::string>& columns) {
    rows.push_back(columns);
}

void SimpleTable::setHeaders(const std::vector<std::string>& headers) {
    this->headers = headers;
    if (columnWidths.empty() && !headers.empty()) {
        // Auto-calculate column widths
        columnWidths.resize(headers.size(), 150.0f);
    }
}

void SimpleTable::clear() {
    rows.clear();
    selectedRow = -1;
    firstVisibleRow = 0;
}

void SimpleTable::setColumnWidths(const std::vector<float>& widths) {
    columnWidths = widths;
}

void SimpleTable::update() {
    Vector2 mousePos = GetMousePosition();
    
    // Handle scroll with mouse wheel when hovering over the table area
    float totalWidth = getTotalWidth();
    float visibleHeight = (float)GetScreenHeight() - position.y - 80.0f; // leave some margin at bottom
    if (visibleHeight <= 0) {
        visibleHeight = (float)GetScreenHeight();
    }
    Rectangle tableArea = {position.x, position.y, totalWidth, visibleHeight};
    if (CheckCollisionPointRec(mousePos, tableArea) && !rows.empty()) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            int maxFirst;
            if (headers.empty()) {
                maxFirst = (int)rows.size() - (int)(visibleHeight / rowHeight);
            } else {
                maxFirst = (int)rows.size() - (int)((visibleHeight - rowHeight) / rowHeight);
            }
            if (maxFirst < 0) maxFirst = 0;

            firstVisibleRow -= (int)wheel; // wheel positive when scrolling up
            if (firstVisibleRow < 0) firstVisibleRow = 0;
            if (firstVisibleRow > maxFirst) firstVisibleRow = maxFirst;
        }
    }

    // Keyboard navigation: Up/Down arrows move selection and auto-scroll, with key repeat
    if (!rows.empty()) {
        static float upTimer = 0.0f;
        static float downTimer = 0.0f;
        float dt = GetFrameTime();
        upTimer += dt;
        downTimer += dt;

        const float initialDelay = 0.3f;
        const float repeatInterval = 0.07f;

        auto moveUp = [&]() {
            if (selectedRow <= 0) {
                selectedRow = 0;
            } else {
                selectedRow--;
            }
        };

        auto moveDown = [&]() {
            if (selectedRow < 0) {
                selectedRow = 0;
            } else if (selectedRow < (int)rows.size() - 1) {
                selectedRow++;
            }
        };

        // Single press
        if (IsKeyPressed(KEY_UP)) {
            moveUp();
            upTimer = 0.0f;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            moveDown();
            downTimer = 0.0f;
        }

        // Held keys: after delay, repeat at interval
        if (IsKeyDown(KEY_UP) && upTimer > initialDelay) {
            if (fmodf(upTimer - initialDelay, repeatInterval) < dt) {
                moveUp();
            }
        } else if (!IsKeyDown(KEY_UP)) {
            upTimer = 0.0f;
        }

        if (IsKeyDown(KEY_DOWN) && downTimer > initialDelay) {
            if (fmodf(downTimer - initialDelay, repeatInterval) < dt) {
                moveDown();
            }
        } else if (!IsKeyDown(KEY_DOWN)) {
            downTimer = 0.0f;
        }

        // Ensure selection is within visible window
        if (selectedRow >= 0) {
            float contentHeight = visibleHeight - (headers.empty() ? 0.0f : rowHeight);
            int visibleRows = (int)(contentHeight / rowHeight);
            if (visibleRows < 1) visibleRows = 1;

            if (selectedRow < firstVisibleRow) {
                firstVisibleRow = selectedRow;
            } else if (selectedRow >= firstVisibleRow + visibleRows) {
                firstVisibleRow = selectedRow - visibleRows + 1;
            }

            if (firstVisibleRow < 0) firstVisibleRow = 0;
            int maxFirst = (int)rows.size() - visibleRows;
            if (maxFirst < 0) maxFirst = 0;
            if (firstVisibleRow > maxFirst) firstVisibleRow = maxFirst;
        }
    }

    // Hover/selection detection only on visible rows (mouse)
    if (!headers.empty()) {
        float startY = position.y + rowHeight; // Skip header row
        float contentHeight = visibleHeight - rowHeight;
        int visibleRows = (int)(contentHeight / rowHeight);
        if (visibleRows < 0) visibleRows = 0;

        for (int i = 0; i < visibleRows; ++i) {
            int rowIndex = firstVisibleRow + i;
            if (rowIndex < 0 || rowIndex >= (int)rows.size()) break;

            float y = startY + i * rowHeight;
            Rectangle rowRect = {position.x, y, totalWidth, rowHeight};
            if (CheckCollisionPointRec(mousePos, rowRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                selectedRow = rowIndex;
                break;
            }
        }
    }
}

void SimpleTable::draw() {
    if (headers.empty() && rows.empty()) {
        return;
    }
    
    float visibleHeight = (float)GetScreenHeight() - position.y - 80.0f;
    if (visibleHeight <= 0) {
        visibleHeight = (float)GetScreenHeight();
    }

    float currentY = position.y;

    // Draw header
    if (!headers.empty()) {
        drawRow(-1, currentY, true);
        currentY += rowHeight;
        visibleHeight -= rowHeight;
    }

    if (visibleHeight <= 0 || rows.empty()) {
        return;
    }

    int maxVisibleRows = (int)(visibleHeight / rowHeight);
    if (maxVisibleRows < 1) maxVisibleRows = 1;

    int lastRow = firstVisibleRow + maxVisibleRows;
    if (lastRow > (int)rows.size()) {
        lastRow = (int)rows.size();
    }

    // Draw visible rows
    for (int i = firstVisibleRow; i < lastRow; ++i) {
        float y = currentY + (i - firstVisibleRow) * rowHeight;
        drawRow(i, y, false);
    }

    // Draw scrollbar if not all rows are visible
    int totalRows = (int)rows.size();
    if (totalRows > maxVisibleRows) {
        float totalWidth = getTotalWidth();
        float scrollX = position.x + totalWidth + 4.0f;
        float trackHeight = (float)maxVisibleRows * rowHeight;

        // Track
        DrawRectangle((int)scrollX, (int)currentY, 6, (int)trackHeight, (Color){40, 40, 40, 255});

        // Thumb size proportional to visible / total
        float ratio = (float)maxVisibleRows / (float)totalRows;
        float thumbHeight = trackHeight * ratio;
        if (thumbHeight < 10.0f) thumbHeight = 10.0f;

        // Thumb position proportional to firstVisibleRow
        float maxScroll = (float)(totalRows - maxVisibleRows);
        float offset = (maxScroll > 0.0f) ? (trackHeight - thumbHeight) * ((float)firstVisibleRow / maxScroll) : 0.0f;

        DrawRectangle((int)scrollX, (int)(currentY + offset), 6, (int)thumbHeight, (Color){120, 120, 200, 255});
    }
}

void SimpleTable::drawRow(int rowIndex, float y, bool isHeader) {
    const std::vector<std::string>* columns = nullptr;
    
    if (isHeader) {
        columns = &headers;
    } else if (rowIndex >= 0 && rowIndex < (int)rows.size()) {
        columns = &rows[rowIndex];
    } else {
        return;
    }
    
    // Draw background
    Color bgColor = isHeader ? headerColor : 
                   (selectedRow == rowIndex ? selectedRowColor : rowColor);
    float totalWidth = getTotalWidth();
    DrawRectangle((int)position.x, (int)y, (int)totalWidth, (int)rowHeight, bgColor);
    
    // Draw border
    DrawRectangleLines((int)position.x, (int)y, (int)totalWidth, (int)rowHeight, 
                      (Color){100, 100, 100, 255});
    
    // Draw columns
    float x = position.x + 10;
    for (size_t i = 0; i < columns->size(); i++) {
        float colWidth = (i < columnWidths.size()) ? columnWidths[i] : 150.0f;
        
        // Draw column border
        if (i > 0) {
            DrawLine((int)x, (int)y, (int)x, (int)(y + rowHeight), 
                    (Color){100, 100, 100, 255});
        }
        
        // Draw text
        if (i < columns->size()) {
            Color textCol = isHeader ? headerTextColor : textColor;
            DrawTextEx(GetFontDefault(), (*columns)[i].c_str(), 
                      {x + 5, y + rowHeight / 2 - 10}, 18, 1, textCol);
        }
        
        x += colWidth;
    }
}

float SimpleTable::getTotalWidth() const {
    float total = 0;
    for (float width : columnWidths) {
        total += width;
    }
    return total > 0 ? total : 600.0f; // Default width if no columns specified
}

