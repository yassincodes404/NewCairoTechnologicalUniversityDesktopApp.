#ifndef SIMPLE_TABLE_HPP
#define SIMPLE_TABLE_HPP

#include "raylib.h"
#include <vector>
#include <string>
#include <array>

class SimpleTable {
public:
    SimpleTable(float x, float y, float rowHeight);
    
    void addRow(const std::vector<std::string>& columns);
    void setHeaders(const std::vector<std::string>& headers);
    void clear();
    
    void draw();
    
    // Get selected row index (if mouse or keyboard selection)
    int getSelectedRow() const { return selectedRow; }
    void update(); // Call this to handle mouse selection, scrolling, and keyboard navigation
    
    void setColumnWidths(const std::vector<float>& widths);
    void setHeaderColor(Color color) { headerColor = color; }
    void setRowColor(Color color) { rowColor = color; }
    void setSelectedRowColor(Color color) { selectedRowColor = color; }
    void setTextColor(Color color) { textColor = color; }
    void setHeaderTextColor(Color color) { headerTextColor = color; }

private:
    Vector2 position;
    float rowHeight;
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    std::vector<float> columnWidths;
    
    int selectedRow;
    int firstVisibleRow;
    Color headerColor;
    Color rowColor;
    Color selectedRowColor;
    Color textColor;
    Color headerTextColor;
    
    void drawRow(int rowIndex, float y, bool isHeader = false);
    float getTotalWidth() const;
};

#endif // SIMPLE_TABLE_HPP

