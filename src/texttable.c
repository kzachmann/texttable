/**
 * @file
 * @version     1.0.0
 * @author      kzachmann
 * @date        2017-06-06 22:51
 * @mainpage    Main Page
 * @brief       Easy to use ASCII Text Tables
 *              ===============================
 *
 * The implementation is used as follows (design by contract):
 *  1. Before a table can be filled, the @ref TextTableInit() function must be called for initialization.
 *  2. The table can be filled with entries using the @ref TextTableAdd() function.
 *     For each table entry this function is called. __Please note that memory is allocated for each table entry__.
 *  3. For the output the function @ref TextTablePrint() is used. For each table row the registered callback function
 *     is called. @ref TextTablePrint() can be called as often as you like, the table designs (@ref TabStyle_e) can be changed,
 *     different callback functions can be used. Please note that only an __even__ number of entries added with
 *     @ref TextTableAdd() will work. Otherwise the rows cannot calculated.
 *  4. To cleanup all the allocated memory the function @ref TextTableFree() must be called.
 *
 * Usage example:
 * --------------
 * @msc
 *  Caller,TextTable,Stdout;
 *  Caller=>TextTable     [label="TextTableInit(...)"];
 *  Caller<<TextTable     [label="return bool"];
 *  Caller=>TextTable     [label="TextTableAdd(...)"];
 *  TextTable=>TextTable  [label="add table entry to list"];
 *  Caller<<TextTable     [label="return bool"];
 *  Caller=>TextTable     [label="TextTableAdd(...)"];
 *  TextTable=>TextTable  [label="add table entry to list"];
 *  Caller<<TextTable     [label="return bool"];
 *  Caller=>TextTable     [label="TextTableAdd(...)"];
 *  TextTable=>TextTable  [label="add table entry to list"];
 *  Caller<<TextTable     [label="return bool"];
 *  Caller=>TextTable     [label="TextTableAdd(...)"];
 *  TextTable=>TextTable  [label="add table entry to list"];
 *  Caller<<TextTable     [label="return bool"];
 *  Caller=>TextTable     [label="TextTablePrint(...)"];
 *  TextTable=>TextTable  [label="process entire table"];
 *  Caller<<=TextTable    [label="PrintLineCallbackFunction(...)"];
 *  Caller=>Caller        [label="pass line to output function"];
 *  Caller=>Stdout        [label="printf(...)"];
 *  Caller<<=TextTable    [label="PrintLineCallbackFunction(...)"];
 *  Caller=>Caller        [label="pass line to output function"];
 *  Caller=>Stdout        [label="printf(...)"];
 *  Caller<<TextTable     [label="return bool"];
 *  Caller=>TextTable     [label="TextTablePrint(...)"];
 *  TextTable=>TextTable  [label="process entire table"];
 *  Caller<<=TextTable    [label="PrintLineCallbackFunction(...)"];
 *  Caller=>Caller        [label="pass line to output function"];
 *  Caller=>Stdout        [label="printf(...)"];
 *  Caller<<=TextTable    [label="PrintLineCallbackFunction(...)"];
 *  Caller=>Caller        [label="pass line to output function"];
 *  Caller=>Stdout        [label="printf(...)"];
 *  Caller<<TextTable     [label="return bool"];
 *  Caller=>TextTable     [label="TextTableFree(...)"];
 * @endmsc
 *
 * Output example:
 * ---------------
 * @code
 * +================+============================+============================+=================+
 * | Headline1      | Headline2                  | Headline3                  | Headline n      |
 * +================+============================+============================+=================+
 * | Row2 Column1   | Row2 Column2               | Row2 Column3               | Row2 Column n   |
 * +----------------+----------------------------+----------------------------+-----------------+
 * | Row3.1 Column1 | Row3.1 Column2             | Row3.1 Column3 'some text' | Row3.1 Column n |
 * |                | Row3.2 Column2 'some text' |                            |                 |
 * |                | Row3.n Column2 ...         |                            |                 |
 * +----------------+----------------------------+----------------------------+-----------------+
 * | Row n Column1  | Row n Column2              | Row n Column3              | Row n Column n  |
 * +----------------+----------------------------+----------------------------+-----------------+
 * @endcode
 *
 * @note This implementation uses extensive dynamic memory allocation. It is up to the user to check
 * how often @ref TextTableAdd() is called and it's also up to the user to check the length
 * and the and arguments of the specified formatted strings.
 *
 * @defgroup group_InterfaceFunctions Interface functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "texttable.h"

#ifdef UNIT_TESTING
#include "texttable_test.h"
#endif

 /**
  * @brief   Closing ANSI-sequence tag
  */
static const char sAnsiSequenceEnd[] = {0x1B, '[', '0', 'm'};

/**
 * @brief   Internal use
 */
typedef struct
{
    size_t rowMaxTextLen;
    size_t ansiSeqMaxLen;
}T_Column;

/**
 * @brief         Initialize the table.
 * @retval true   success
 * @retval false  failed
 * @ingroup       group_InterfaceFunctions
 */
bool TextTableInit(TextTable_t *table) ///< [in] The table
{
    if (NULL != table)
    {
        table->head = NULL;
        table->tail = NULL;
        table->entries = 0;
        table->charGridX = '-';
        table->charGridBoundary = '|';
        table->charGridSeparator = '|';
        table->charHeadX = '=';
        table->charHeadBoundary = '|';
        table->charHeadSeparator = '|';
        table->charConnectorXY = '+';
        table->spacesBetweenBorder = 1;
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief   Add table entry.
 *          - For each entry memory is allocated (free with @ref TextTableFree())
 *          - `\n` adds new row in entry
 * @retval true   success - the entry will be added to the table
 * @retval false  failed - the entry will __not__ be added to the table
 * @ingroup group_InterfaceFunctions
 */
bool TextTableAdd(
    TextTable_t *table,     ///< [in] The table
    const char *ansiSeq,    ///< [in] ANSI-sequence string, closing tag will be added automatically
    const char* format,     ///< [in] printf(...) like
    ...)                    ///< [in] printf(...) like arguments
{
    if (NULL == table)
    {
        return false;
    }

    TextTableEntry_t *pEntry = (TextTableEntry_t*) malloc(sizeof(TextTableEntry_t));
    if (pEntry == NULL)
    {
        return false;
    }

    memset(pEntry, 0, sizeof(TextTableEntry_t));

    if (NULL == table->head)
    {
        // no entry in table
        table->head = pEntry;
        pEntry->prevEntry = NULL;
    }
    else
    {
        table->tail->nextEntry = pEntry;
        pEntry->prevEntry = table->tail;
        table->tail->nextEntry = pEntry;
    }
    table->tail = pEntry;
    table->entries++;

    // calculate length for column buffer
    if (NULL != format)
    {
        va_list arg;
        va_start(arg, format);
        int textLen = vsnprintf(NULL, 0, format, arg);
        va_end(arg);

        if (textLen > 0)
        {
            pEntry->textLen = textLen;
            if (ansiSeq != NULL)
            {
                pEntry->ansiSeqLen = strlen(ansiSeq);
                if (TEXT_TABLE_MAX_ANSI_SEQ_LEN > pEntry->ansiSeqLen)
                {
                    pEntry->ansiSeq = (char*) malloc(pEntry->ansiSeqLen + 1);
                    snprintf(pEntry->ansiSeq, pEntry->ansiSeqLen + 1, "%s", ansiSeq);
                }
                else
                {
                    pEntry->ansiSeqLen = 0;
                }
            }

            if (TEXT_TABLE_MAX_COLUMN_LEN < pEntry->textLen)
            {
                pEntry->textLen = TEXT_TABLE_MAX_COLUMN_LEN;
            }
            pEntry->text = (char*) malloc(pEntry->textLen + 1);
            if (pEntry->text == NULL)
            {
                pEntry->textLen = 0;
                return false;
            }
            memset(pEntry->text, 0, pEntry->textLen + 1);

            va_start(arg, format);
            if (vsnprintf(pEntry->text, pEntry->textLen + 1, format, arg) < 1)
            {
                memset(pEntry->text, 0, pEntry->textLen + 1);
            }
            va_end(arg);

            // calculate the maximum length column row
            pEntry->rowMaxTextLen = 1;
            size_t rowTextLen = 0;
            for (size_t i = 0; i < pEntry->textLen; i++)
            {
                if ('\n' == pEntry->text[i])
                {
                    if (pEntry->rowMaxTextLen < rowTextLen)
                    {
                        pEntry->rowMaxTextLen = rowTextLen;
                    }
                    rowTextLen = 0;
                }
                else
                {
                    rowTextLen++;
                }
            }
            if (pEntry->rowMaxTextLen < rowTextLen)
            {
                pEntry->rowMaxTextLen = rowTextLen;
            }
        }
    }
    return true;
}

/**
 * @brief         Print the table line by line to the registered output-callback-function.
 * @retval true   success
 * @retval false  failed
 * @ingroup       group_InterfaceFunctions
 */
bool TextTablePrint(
    TextTable_t *table,                                 ///< [in] The table.
    void(*PrintLineCallbackFunction)(const char *line), ///< [in] Function pointer to output function.
    TabStyle_e tabStyle,                                ///< [in] Choose one of the styles.
    size_t posX,                                        ///< [in] Number of chars to right shift the table.
    size_t columns)                                     ///< [in] Number of table columns.
                                                        /// Please note that only an __even__ number of entries added with
                                                        /// @ref TextTableAdd() will work. Otherwise the rows cannot calculated.
{
    if (NULL == table)
    {
      return false;
    }
    if (NULL == PrintLineCallbackFunction)
    {
        return false;
    }
    if (posX > TEXT_TABLE_MAX_X_POS)
    {
        return false;
    }
    if (columns == 0)
    {
        return false;
    }
    if (table->entries == 0)
    {
        return false;
    }
    
    // calculate number of rows in the table
    size_t rows = table->entries / columns;
    if (table->entries != (rows * columns))
    {
        return false;   // cannot calculate rows
    }

    T_Column *pColumn = (T_Column*) malloc(sizeof(T_Column) * columns);
    if (pColumn == NULL)
    {
        return false;
    }
    // calculate max text length (width) of each column row in the table
    memset(pColumn, 0, sizeof(T_Column) * columns);
    TextTableEntry_t* pEntry = table->head;
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < columns; j++)
        {
            if (pEntry->rowMaxTextLen > pColumn[j].rowMaxTextLen)
            {
                pColumn[j].rowMaxTextLen = pEntry->rowMaxTextLen;
            }
            if (pEntry->ansiSeqLen > pColumn[j].ansiSeqMaxLen)
            {
                pColumn[j].ansiSeqMaxLen = pEntry->ansiSeqLen;
            }
            pEntry = pEntry->nextEntry;
        }
    }
    // calculate row length
    size_t rowLen = 2; // + 1 '\0' zero terminated string, + 1 opening column character
    for (size_t i = 0; i < columns; i++)
    {
        rowLen += pColumn[i].rowMaxTextLen + (table->spacesBetweenBorder*2) + 1; // +1 closing column character
        if (pColumn[i].ansiSeqMaxLen)
        {
            rowLen += pColumn[i].ansiSeqMaxLen + sizeof(sAnsiSequenceEnd);
        }
    }
    rowLen = rowLen + posX;

    char *rowBuf = (char*) malloc(rowLen);
    if (rowBuf == NULL)
    {
        return false;
    }
    else
    {
        memset(rowBuf, ' ', rowLen);
    }

    char *gridBuf = (char*) malloc(rowLen);
    if (gridBuf == NULL)
    {
        return false;
    }
    else
    {
        memset(gridBuf, ' ', rowLen);
    }

    char *headBuf = (char*) malloc(rowLen);
    if (headBuf == NULL)
    {
        return false;
    }
    else
    {
        memset(headBuf, ' ', rowLen);
    }

    // create border / header
    size_t idx = posX;
    gridBuf[idx] = table->charConnectorXY;
    headBuf[idx] = table->charConnectorXY;
    idx++;
    for (size_t i = 0; i < columns; i++)
    {
        for (size_t j = 0; j < (pColumn[i].rowMaxTextLen + (table->spacesBetweenBorder*2)); j++)
        {
            gridBuf[idx] = table->charGridX;
            headBuf[idx] = table->charHeadX;
            idx++;
        }
        gridBuf[idx] = table->charConnectorXY;
        headBuf[idx] = table->charConnectorXY;
        idx++;
    }
    gridBuf[idx] = '\0';
    headBuf[idx] = '\0';

    // print table
    pEntry = table->head;
    // rows
    bool firstTableLine = true;
    for (size_t i = 0; i < rows; i++)
    {
        bool newLine = false;
        bool firstRowPerColumn = true;
        TextTableEntry_t *pEntrySave = pEntry;
        // columns
        do
        {
            size_t idxRow = posX;
            // if newline detected restore column pointer
            if (newLine)
            {
                pEntry = pEntrySave;
                newLine = false;
            }

            for (size_t j = 0; j < columns; j++)
            {
                if (0 == j) // first column in table
                {
                    if (0 == i) // first row in table
                    {
                        switch (tabStyle)
                        {
                            case TABSTYLE_COMACT:
                                break;
                            case TABSTYLE_REGULAR_HEAD_OFF:
                            case TABSTYLE_SEPARATED_HEAD_OFF:
                                rowBuf[idxRow++] = table->charGridBoundary;
                                for (size_t spaces=0; spaces < table->spacesBetweenBorder; spaces++)
                                {
                                  rowBuf[idxRow++] = ' ';
                                }
                                break;
                            case TABSTYLE_REGULAR_HEAD_ON:
                            case TABSTYLE_SEPARATED_HEAD_ON:
                                rowBuf[idxRow++] = table->charHeadBoundary;
                                for (size_t spaces = 0; spaces < table->spacesBetweenBorder; spaces++)
                                {
                                  rowBuf[idxRow++] = ' ';
                                }
                                break;
                        }
                    }
                    else        // next row, first column in table
                    {
                        if (TABSTYLE_COMACT != tabStyle)
                        {
                            rowBuf[idxRow++] = table->charGridBoundary;
                            for (size_t spaces = 0; spaces < table->spacesBetweenBorder; spaces++)
                            {
                              rowBuf[idxRow++] = ' ';
                            }
                        }
                    }
                }
                else
                {
                    for (size_t spaces = 0; spaces < table->spacesBetweenBorder; spaces++)
                    {
                        rowBuf[idxRow++] = ' ';
                    }
                }

                if (firstRowPerColumn)
                {
                    pEntry->writeTxt = pEntry->text; // set write pointer
                }
                // write column row
                for (size_t k = 0; k < pColumn[j].rowMaxTextLen; k++)
                {
                    // ansi sequence start
                    if ((0 == k) && (NULL != pEntry->ansiSeq))
                    {
                        memcpy(&rowBuf[idxRow], pEntry->ansiSeq, pEntry->ansiSeqLen);
                        idxRow = idxRow + pEntry->ansiSeqLen;
                    }
                    // space or text
                    if ((NULL == pEntry->writeTxt) || (0x00 == *pEntry->writeTxt) || ('\n' == *pEntry->writeTxt))
                    {
                        rowBuf[idxRow++] = ' '; // space
                    }
                    else
                    {
                        rowBuf[idxRow++] = *pEntry->writeTxt++;
                    }
                }

                // ansi sequence end
                if (NULL != pEntry->ansiSeq)
                {
                    memcpy(&rowBuf[idxRow], sAnsiSequenceEnd, sizeof(sAnsiSequenceEnd));
                    idxRow = idxRow + sizeof(sAnsiSequenceEnd);
                }

                // check if newline in column row - increase write pointer
                if ((NULL != pEntry->writeTxt) && ('\n' == *pEntry->writeTxt))
                {
                    newLine = true;
                    pEntry->writeTxt++;
                }

                for (size_t spaces = 0; spaces < table->spacesBetweenBorder; spaces++)
                {
                    rowBuf[idxRow++] = ' ';
                }
                if (TABSTYLE_COMACT != tabStyle)
                {
                    if (j < (columns - 1))
                    {
                      rowBuf[idxRow++] = table->charGridSeparator; // separate column
                    }
                }

                pEntry = pEntry->nextEntry;
            }
            if (0 == i) // first row, last column in table
            {
                switch (tabStyle)
                {
                    case TABSTYLE_COMACT:
                        break;
                    case TABSTYLE_REGULAR_HEAD_OFF:
                    case TABSTYLE_SEPARATED_HEAD_OFF:
                        rowBuf[idxRow++] = table->charGridBoundary;
                        break;
                    case TABSTYLE_REGULAR_HEAD_ON:
                    case TABSTYLE_SEPARATED_HEAD_ON:
                        rowBuf[idxRow++] = table->charHeadBoundary;
                        break;
                }
            }
            else // next row, last column in table
            {
                if (TABSTYLE_COMACT != tabStyle)
                {
                    rowBuf[idxRow++] = table->charGridBoundary;
                }
            }
            rowBuf[idxRow] = 0x00; // zero terminated

            // first line
            if (firstTableLine)
            {
                switch (tabStyle)
                {
                    case TABSTYLE_COMACT:
                        break;
                    case TABSTYLE_REGULAR_HEAD_OFF:
                    case TABSTYLE_SEPARATED_HEAD_OFF:
                        PrintLineCallbackFunction(gridBuf);
                        break;
                    case TABSTYLE_REGULAR_HEAD_ON:
                    case TABSTYLE_SEPARATED_HEAD_ON:
                        PrintLineCallbackFunction(headBuf);
                        break;
                }
            }
            PrintLineCallbackFunction(rowBuf);
            firstTableLine = false;
            firstRowPerColumn = false;

        } while (newLine);

        if (i == 0) // first row, closing line of header
        {
            switch (tabStyle)
            {
                case TABSTYLE_COMACT:
                case TABSTYLE_REGULAR_HEAD_OFF:
                    // no closing line
                    break;
                case TABSTYLE_SEPARATED_HEAD_OFF:
                    PrintLineCallbackFunction(gridBuf);
                    break;
                case TABSTYLE_REGULAR_HEAD_ON:
                case TABSTYLE_SEPARATED_HEAD_ON:
                    PrintLineCallbackFunction(headBuf);
                    break;
            }
        }
        else if (i < (rows - 1)) // between rows
        {
            switch (tabStyle)
            {
                case TABSTYLE_COMACT:
                case TABSTYLE_REGULAR_HEAD_ON:
                case TABSTYLE_REGULAR_HEAD_OFF:
                    // no line
                    break;
                case TABSTYLE_SEPARATED_HEAD_OFF:
                case TABSTYLE_SEPARATED_HEAD_ON:
                    PrintLineCallbackFunction(gridBuf);
                    break;
            }
        }
    }
    if (TABSTYLE_COMACT != tabStyle)
    {
      PrintLineCallbackFunction(gridBuf);
    }

    free(gridBuf);
    free(headBuf);
    free(rowBuf);
    free(pColumn);

    return true;
}

/**
 * @brief   Release all allocated memory.
 * @ingroup group_InterfaceFunctions
 */
void TextTableFree(TextTable_t *table) ///< [in] the table
{
    if (NULL != table)
    {
        TextTableEntry_t *pEntry = table->head;
        while (NULL != pEntry)
        {
            TextTableEntry_t *pEntryNext = pEntry->nextEntry;
            if (NULL != pEntry->text)
            {
                free(pEntry->text);
            }
            if (NULL != pEntry->ansiSeq)
            {
                free(pEntry->ansiSeq);
            }
            free(pEntry);
            pEntry = pEntryNext;
        }

        table->head = NULL;
        table->tail = NULL;
        table->entries = 0;
    }
}
