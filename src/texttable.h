/**
 * @file
 * @author  kzachmann
 * @date    2017-06-06 22:52
 *
 * @brief   Dynamic, aligned and size optimized ASCII Text Tables
 *          =====================================================
 */

#ifndef _TEXT_TABLE_H_
#define _TEXT_TABLE_H_

#include <stddef.h>
#include <stdbool.h>

#define TEXT_TABLE_MAX_COLUMN_LEN       96  ///< Maximum text length of one column
#define TEXT_TABLE_MAX_X_POS            30  ///< Maximum left shift of the whole table
#define TEXT_TABLE_MAX_ANSI_SEQ_LEN     24  ///< Maximum length for ANSI sequence use

/**
 * @brief   One table entry
 */
typedef struct TextTableEntry_t
{
    struct TextTableEntry_t *nextEntry; ///< Pointer to next table entry
    struct TextTableEntry_t *prevEntry; ///< Pointer to previous table entry
    char *text;                         ///< The text of the table cell
    size_t textLen;                     ///< Length of the text
    size_t rowMaxTextLen;               ///< Maximum text length of all rows in the table
    char *ansiSeq;                      ///< Holds the ANSI sequence
    size_t ansiSeqLen;                  ///< Length of the ANSI sequence
    char *writeTxt;                     ///< Write pointer
}TextTableEntry_t;

/**
 * @brief   The table
 */
typedef struct
{
    TextTableEntry_t *head;     ///< Pointer to first entry or NULL
    TextTableEntry_t *tail;     ///< Pointer to last entry or NULL
    size_t entries;             ///< Number of table entries

    char charGridX;             ///< default '-'
    char charGridBoundary;      ///< default '|'
    char charGridSeparator;     ///< default '|'
    char charHeadX;             ///< default '='
    char charHeadBoundary;      ///< default '|'
    char charHeadSeparator;     ///< default '|'
    char charConnectorXY;       ///< default '+'
    size_t spacesBetweenBorder; ///< Spaces (0x20) between column text and column border
}TextTable_t;

/**
 * @brief   Available table styles
 */
typedef enum TabStyle_e
{
    /// Example:
    /** @code
    +=============+=============+=============+
    | Head1       | Head2       | Head3       |
    +=============+=============+=============+
    | Row2Column1 | Row2Column2 | Row2Column2 |
    |             | ColumnRow2  |             |
    | Row3Column1 | Row3Column2 | Row3Column2 |
    +-------------+-------------+-------------+
    @endcode */
    TABSTYLE_REGULAR_HEAD_ON,

    /// Example:
    /** @code
    +-------------+-------------+-------------+
    | Head1       | Head2       | Head3       |
    | Row2Column1 | Row2Column2 | Row2Column2 |
    |             | ColumnRow2  |             |
    | Row3Column1 | Row3Column2 | Row3Column2 |
    +-------------+-------------+-------------+
    @endcode */
    TABSTYLE_REGULAR_HEAD_OFF,

    /// Example:
    /** @code
    +=============+=============+=============+
    | Head1       | Head2       | Head3       |
    +=============+=============+=============+
    | Row2Column1 | Row2Column2 | Row2Column2 |
    |             | ColumnRow2  |             |
    +-------------+-------------+-------------+
    | Row3Column1 | Row3Column2 | Row3Column2 |
    +-------------+-------------+-------------+
    @endcode */
    TABSTYLE_SEPARATED_HEAD_ON,

    /// Example:
    /** @code
    +-------------+-------------+-------------+
    | Head1       | Head2       | Head3       |
    +-------------+-------------+-------------+
    | Row2Column1 | Row2Column2 | Row2Column2 |
    |             | ColumnRow2  |             |
    +-------------+-------------+-------------+
    | Row3Column1 | Row3Column2 | Row3Column2 |
    +-------------+-------------+-------------+
    @endcode */
    TABSTYLE_SEPARATED_HEAD_OFF,

    /// Example:
    /** @code
    Head1       Head2       Head3
    Row2Column1 Row2Column2 Row2Column2
    ColumnRow2
    Row3Column1 Row3Column2 Row3Column2
    @endcode */
    TABSTYLE_COMACT

}TabStyle_e;


bool TextTableInit(TextTable_t *table);
// @cond make doxygen happy
#ifdef _WIN32
bool TextTableAdd(TextTable_t *table, const char *ansiSeq, const char* format, ...);
#else
bool TextTableAdd(TextTable_t *table, const char *ansiSeq, const char* format, ...)__attribute__((format(printf, 3, 4)));
#endif
bool TextTableAdd(TextTable_t *table, const char *ansiSeq, const char* format, ...);
// @endcond
bool TextTablePrint(TextTable_t *table, void(*PrintLineCallbackFunction)(const char *line), TabStyle_e tabStyle, size_t posX, size_t columns);
void TextTableFree(TextTable_t *table);

#endif