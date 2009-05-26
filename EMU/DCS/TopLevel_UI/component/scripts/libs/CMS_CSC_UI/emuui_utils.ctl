/**@file

This package contains utility functions for UI components.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

const int EMUUI_TABLE_BORDER_WIDTH = 4;
const int EMUUI_TABLE_HEADER_HEIGHT = 20; 
const int EMUUI_TABLE_BORDER_HEIGHT = 4;

/** Sets the table width to fit the columns and adjusts height for numOfRows given (if -1 then height is unchanged). */
void emuui_tableCompact(string tableShapeName, int numOfRows = -1) {
  shape table = getShape(tableShapeName);
  int columnCount = table.columnCount();
  int newTableWidth = 0, newTableHeight;
  for (int i=0; i < columnCount; i++) {
    newTableWidth += table.columnWidth(i);
  }
  newTableWidth += EMUUI_TABLE_BORDER_WIDTH;
  if (table.rowHeaderVisibility()) { // if row header is visible - add the row header width
    newTableWidth += table.rowHeaderWidth();
  }
  
  if (numOfRows != -1) { // compute new height
    newTableHeight = numOfRows * table.rowHeight() + EMUUI_TABLE_BORDER_HEIGHT;
    if (table.columnHeaderVisibility()) { // if header is visible - add EMUUI_TABLE_HEADER_HEIGHT
      newTableHeight += EMUUI_TABLE_HEADER_HEIGHT;
    }
  } else { // don't change the height
    int actualWidth, actualHeight;
    getValue(table, "size", actualWidth, actualHeight);
    newTableHeight = actualHeight;
  }
  
  table.size(newTableWidth, newTableHeight);
}
