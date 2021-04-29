/**
 * @file
 * @author  kzachmann
 * @date    2018-12-16 07:16
 *
 * @brief   Demo/Usage of TextTable
 */

#ifdef _WIN32
#include <windows.h>
#endif
#include <inttypes.h>
#include <stdio.h>
#include "texttable.h"

 /**
  * @brief   Callback function for one line output
  */
static void PrintLineCallbackFunction(const char* line)
{
  printf("%s\n", line);
}

/**
 * @brief   main
 */
int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

#ifdef _WIN32
  // Enable support for ANSI sequences on the Windows command line
  DWORD fdwSaveOldMode;
  HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if ((hStdOut != INVALID_HANDLE_VALUE) && GetConsoleMode(hStdOut, &fdwSaveOldMode))
  {
    SetConsoleMode(hStdOut, fdwSaveOldMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  }
#endif

  TextTable_t tTextTable;
  TextTableInit(&tTextTable);
  TextTableAdd(&tTextTable, NULL, "Headline1");
  TextTableAdd(&tTextTable, NULL, "Headline2");
  TextTableAdd(&tTextTable, NULL, "Headline3");
  TextTableAdd(&tTextTable, NULL, "Headline4");
  TextTableAdd(&tTextTable, NULL, "Headline n");

  TextTableAdd(&tTextTable, NULL, "Row2 Column1");
  TextTableAdd(&tTextTable, NULL, "Row2 Column2");
  TextTableAdd(&tTextTable, NULL, "Row2 Column3");
  TextTableAdd(&tTextTable, NULL, "Row2 Column4");
  TextTableAdd(&tTextTable, NULL, "Row2 Column n");

  TextTableAdd(&tTextTable, NULL, "Row3.1 Column1");
  TextTableAdd(&tTextTable, NULL, "Row3.1 Column2\nRow3.2 Column2 '%s'\nRow3.n Column2 ...", "some text");
  TextTableAdd(&tTextTable, NULL, "Row3.1 Column3 '%s'", "some text");
  TextTableAdd(&tTextTable, NULL, "Row3.1 Column4 int: '%d'", 4711);
  TextTableAdd(&tTextTable, NULL, "Row3.1 Column n");

  TextTableAdd(&tTextTable, NULL, "Row n Column1");
  TextTableAdd(&tTextTable, NULL, "Row n Column2");
  TextTableAdd(&tTextTable, NULL, "Row n Column3");
  TextTableAdd(&tTextTable, NULL, "Row n Column4");
  TextTableAdd(&tTextTable, NULL, "Row n Column n");

  size_t numberOfColumns = 5;
  printf("\n TABSTYLE_REGULAR_HEAD_ON\n");
  TextTablePrint(&tTextTable, PrintLineCallbackFunction, TABSTYLE_REGULAR_HEAD_ON, 0, numberOfColumns);
  printf("\n TABSTYLE_REGULAR_HEAD_OFF\n");
  TextTablePrint(&tTextTable, PrintLineCallbackFunction, TABSTYLE_REGULAR_HEAD_OFF, 1, numberOfColumns);
  printf("\n TABSTYLE_SEPARATED_HEAD_ON\n");
  TextTablePrint(&tTextTable, PrintLineCallbackFunction, TABSTYLE_SEPARATED_HEAD_ON, 2, numberOfColumns);
  printf("\n TABSTYLE_SEPARATED_HEAD_OFF\n");
  TextTablePrint(&tTextTable, PrintLineCallbackFunction, TABSTYLE_SEPARATED_HEAD_OFF, 3, numberOfColumns);
  printf("\n TABSTYLE_COMACT\n");
  TextTablePrint(&tTextTable, PrintLineCallbackFunction, TABSTYLE_COMACT, 4, numberOfColumns);
  TextTableFree(&tTextTable);

  printf("\n TABSTYLE_REGULAR_HEAD_ON - using ANSI sequences\n");
  TextTableInit(&tTextTable);
  TextTableAdd(&tTextTable, "\033[4m", "Head1 underlined");
  TextTableAdd(&tTextTable, "\033[4m", "Head2 underlined");
  TextTableAdd(&tTextTable, "\033[4m", "Head3 underlined");
  TextTableAdd(&tTextTable, "\033[0;34m", "Row2 Column1 blue");
  TextTableAdd(&tTextTable, NULL, "Row2.1 Column2\nRow2.2 Columns2 %s", "test");
  TextTableAdd(&tTextTable, "\033[46;37m", "Row2 Column3 %d\ncyan", 4711);
  TextTableAdd(&tTextTable, NULL, "Row3 Column1");
  TextTableAdd(&tTextTable, "\033[0;33m", "Row3 Column2 yellow");
  TextTableAdd(&tTextTable, NULL, "Row3 Column3");
  TextTablePrint(&tTextTable, PrintLineCallbackFunction, TABSTYLE_REGULAR_HEAD_ON, 0, 3);
  TextTableFree(&tTextTable);

  // Note: ANSI sequences do not work on the standard Windows console (cmd).
  char ansiColor[TEXT_TABLE_MAX_ANSI_SEQ_LEN];

  printf("\n16 standard and high intensity colors\n");
  TextTableInit(&tTextTable);
  tTextTable.spacesBetweenBorder = 0; // no spaces between borders
  for (size_t i = 0; i < 16; i++)
  {
    snprintf(ansiColor, sizeof(ansiColor), "\x1b[48;5;%zum", i);
    TextTableAdd(&tTextTable, ansiColor, "    \n");
  }
  TextTablePrint(&tTextTable, PrintLineCallbackFunction, TABSTYLE_COMACT, 0, 16);
  TextTableFree(&tTextTable);

  printf("216 colors\n");
  TextTableInit(&tTextTable);
  tTextTable.spacesBetweenBorder = 0; // no spaces between borders
  for (size_t i = 16; i < 232; i++)
  {
    snprintf(ansiColor, sizeof(ansiColor), "\x1b[48;5;%zum", i);
    TextTableAdd(&tTextTable, ansiColor, "  ");
  }
  TextTablePrint(&tTextTable, PrintLineCallbackFunction, TABSTYLE_COMACT, 0, 36);
  TextTableFree(&tTextTable);

  printf("Grayscale colors\n");
  TextTableInit(&tTextTable);
  tTextTable.spacesBetweenBorder = 0; // no spaces between borders
  for (size_t i = 232; i < 256; i++)
  {
    snprintf(ansiColor, sizeof(ansiColor), "\x1b[48;5;%zum", i);
    TextTableAdd(&tTextTable, ansiColor, "   ");
  }
  TextTablePrint(&tTextTable, PrintLineCallbackFunction, TABSTYLE_COMACT, 0, 24);
  TextTableFree(&tTextTable);

  return 0;
}