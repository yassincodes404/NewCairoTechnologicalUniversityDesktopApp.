#include "SimpleTable.hpp"

SimpleTable::SimpleTable(float x, float y, float rowHeight)
    : position{x, y}, rowHeight(rowHeight), selectedRow(-1),
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
}

void SimpleTable::setColumnWidths(const std::vector<float>& widths) {
    columnWidths = widths;
}

void SimpleTable::update() {
    Vector2 mousePos = GetMousePosition();
    selectedRow = -1;
    
    if (!headers.empty()) {
        float startY = position.y + rowHeight; // Skip header row
        for (size_t i = 0; i < rows.size(); i++) {
            float y = startY + i * rowHeight;
            float totalWidth = getTotalWidth();
            
            Rectangle rowRect = {position.x, y, totalWidth, rowHeight};
            if (CheckCollisionPointRec(mousePos, rowRect)) {
                selectedRow = (int)i;
                break;
            }
        }
    }
}

void SimpleTable::draw() {
    if (headers.empty() && rows.empty()) {
        return;
    }
    
    // Draw header
    if (!headers.empty()) {
        drawRow(-1, position.y, true);
    }
    
    // Draw rows
    float startY = position.y + (headers.empty() ? 0 : rowHeight);
    for (size_t i = 0; i < rows.size(); i++) {
        bool isSelected = (selectedRow >= 0 && (size_t)selectedRow == i);
        float y = startY + i * rowHeight;
        drawRow((int)i, y, false);
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

