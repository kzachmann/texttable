/**
 * @file
 * @author      kzachmann
 * @date        2018-12-16 07:23
 *
 * @brief       Building and running the unit tests requires the unit test framework cmocka (https://cmocka.org/)
 */

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

//cmocka
#include <setjmp.h>
#include <cmocka.h>

#include "texttable.h"


static bool sCallbackFunctionCalled = false;

/**
 * @brief    Group setup, called before every test group
 */
static int GroupSetup(void **state)
{
    (void) state;
    return 0;
}

/**
 * @brief    Group tear down, called after every test group
 */
static int GroupTeardown(void **state)
{
    (void) state;
    return 0;
}

/**
 * @brief    Test case setup, called before every test case
 */
static int TestSetup(void **state)
{
    (void) state;
    sCallbackFunctionCalled = false;
    return 0;
}

/**
 * @brief    Test case tear down, called after every test case
 */
static int TestTeardown(void **state)
{
    (void) state;
    return 0;
}

/**
 * @brief    Callback function for one line output.
 */
 static void PrintLine(const char *line)    ///< [in] the line to be print
{
    (void) line;
    sCallbackFunctionCalled = true;
    //printf("%s\n", line);
}

/**
 * @brief   Test init with passing a `NULL` pointer.
 */
 void UTest_TextTableInit_Fail(void ** state)
{
     (void) state;
     assert_false(TextTableInit(NULL));
}

 /**
 * @brief   Test initialization and free.
 */
void UTest_TextTableInit_Success(void ** state)
{
    (void) state;
    TextTable_t tTextTable;
    assert_true(TextTableInit(&tTextTable));
    TextTableFree(&tTextTable);
}

/**
 * @brief   Test free with NULL pointer.
 */
void UTest_TextTableFree_NULL(void ** state)
{
    (void) state;
    TextTableFree(NULL);
}

/**
 * @brief    Test add column to table but table is `NULL`.
 */
void UTest_TextTableAdd_TableNULL(void ** state)
{
    (void) state;
    assert_false(TextTableAdd(NULL, "\033[43;34;64m", "test string"));
    assert_false(sCallbackFunctionCalled);
}

/**
 * @brief   Test add column to table with _ansiSeq_ is `NULL`.
*/
void UTest_TextTableAdd_ansiSeqNULL(void ** state)
{
    (void) state;
    TextTable_t tTextTable;
    assert_true(TextTableInit(&tTextTable));
    assert_true(TextTableAdd(&tTextTable, NULL, "test string"));
    assert_false(sCallbackFunctionCalled);
    TextTableFree(&tTextTable);
}

/**
 * @brief   Test with an too long ANSI sequence (_ansiSeq_).
*/
void UTest_TextTableAdd_ansiSeqTooLong(void ** state)
{
    (void) state;
    TextTable_t tTextTable;
    assert_true(TextTableInit(&tTextTable));
    assert_true(TextTableAdd(&tTextTable, "ansi_color_too_long_12345678901234567890", "test string"));
    assert_false(sCallbackFunctionCalled);
    TextTableFree(&tTextTable);
}

/**
 * @brief   Test add column to table with _format_ is `NULL`.
 * @note    The entry will be added as an empty entry to the table.
*/
void UTest_TextTableAdd_formatNULL(void ** state)
{
    (void) state;
    TextTable_t tTextTable;
    assert_true(TextTableInit(&tTextTable));
    assert_true(TextTableAdd(&tTextTable, "\033[43;34;64m", NULL));
    assert_false(sCallbackFunctionCalled);
    TextTableFree(&tTextTable);
}

/**
 * @brief   Test function @ref TextTablePrint() called with wrong column count.
 *          Table has 3 entries but @ref TextTablePrint() is called with 2 columns.
 */
void UTest_TextTablePrint_ColumnError(void ** state)
{
    (void) state;
    TextTable_t tTextTable;
    assert_true(TextTableInit(&tTextTable));

    assert_true(TextTableAdd(&tTextTable, NULL, "headline1"));
    assert_true(TextTableAdd(&tTextTable, NULL, "headline2"));
    assert_true(TextTableAdd(&tTextTable, NULL, "headline3"));
    assert_false(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_REGULAR_HEAD_ON, 0, 2));
    assert_false(sCallbackFunctionCalled);

    TextTableFree(&tTextTable);
}

/**
 * @brief   Test that `table` the pointer is `NULL`.
 */
void UTest_TextTablePrint_TableNull(void ** state)
{
  (void) state;
  assert_false(TextTablePrint(NULL, PrintLine, TABSTYLE_REGULAR_HEAD_ON, 0, 2));
  assert_false(sCallbackFunctionCalled);
}

/**
 * @brief   Test that callback function pointer is `NULL`.
 */
void UTest_TextTablePrint_CallbackNull(void ** state)
{
  (void) state;
  TextTable_t tTextTable;
  assert_true(TextTableInit(&tTextTable));
  assert_false(TextTablePrint(&tTextTable, NULL, TABSTYLE_REGULAR_HEAD_ON, 0, 2));
  assert_false(sCallbackFunctionCalled);
}

/**
 * @brief   Test several scenarios regarding `posX`.
 */
void UTest_TextTablePrint_posX(void ** state)
{
  (void) state;
  TextTable_t tTextTable;
  assert_true(TextTableInit(&tTextTable));
  
  assert_true(TextTableAdd(&tTextTable, NULL, "headline1"));
  assert_true(TextTableAdd(&tTextTable, NULL, "headline2"));
  assert_true(TextTableAdd(&tTextTable, NULL, "headline3"));

  assert_true(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_REGULAR_HEAD_ON, TEXT_TABLE_MAX_X_POS-1, 3));
  assert_true(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_REGULAR_HEAD_ON, TEXT_TABLE_MAX_X_POS, 3));
  assert_false(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_REGULAR_HEAD_ON, TEXT_TABLE_MAX_X_POS+1, 3));
  assert_true(sCallbackFunctionCalled);

  TextTableFree(&tTextTable);
}

/**
 * @brief   Test if the arguments for the parameter `columns` is `0`.
 */
void UTest_TextTablePrint_columns0(void ** state)
{
  (void) state;
  TextTable_t tTextTable;
  assert_true(TextTableInit(&tTextTable));
  assert_false(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_REGULAR_HEAD_ON, 0, 0));
  assert_false(sCallbackFunctionCalled);
}

/**
 * @brief   Test if the table has zero entries, so @ref TextTableAdd() was never successfully called.
 */
void UTest_TextTablePrint_zeroEntries(void ** state)
{
  (void) state;
  TextTable_t tTextTable;
  assert_true(TextTableInit(&tTextTable));
  assert_false(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_REGULAR_HEAD_ON, 0, 2));
  assert_false(sCallbackFunctionCalled);
}

/**
 * @brief   Test full sequence.
 */
void UTest_TextTablePrint_SequenceOK(void ** state)
{
    (void) state;
    char test[] = "";
    TextTable_t tTextTable;
    assert_true(TextTableInit(&tTextTable));

    assert_true(TextTableAdd(&tTextTable, "\033[43;34;64m", "headline1 color changed"));
    assert_true(TextTableAdd(&tTextTable, NULL, "headline2"));
    assert_true(TextTableAdd(&tTextTable, NULL, "headline3"));
    assert_true(TextTableAdd(&tTextTable, "\033[4m", "headline4 underlined"));
    assert_true(TextTableAdd(&tTextTable, "\033[0;34m", "row1 column1_row1"));
    assert_true(TextTableAdd(&tTextTable, "\033[0;33m", "row1 column2_row1\nrow1 clmn2_r2\nrow1 column2_row3\nrw1 clmn2_r4"));
    assert_true(TextTableAdd(&tTextTable, NULL, "row1 column3_row1"));
    assert_true(TextTableAdd(&tTextTable, NULL, "row1 column4_row1"));
    assert_true(TextTableAdd(&tTextTable, NULL, "row2 column1_row1"));
    assert_true(TextTableAdd(&tTextTable, NULL, NULL));
    assert_true(TextTableAdd(&tTextTable, NULL, "%s", test));
    assert_true(TextTableAdd(&tTextTable, NULL, "row2 column4_row1"));
    assert_true(TextTableAdd(&tTextTable, NULL, "row3 column1_row1"));
    assert_true(TextTableAdd(&tTextTable, NULL, "row3 clmn3_row1\n\nrow3 column3_row3"));
    assert_true(TextTableAdd(&tTextTable, NULL, "row3 column3_row1 very long string 0123456789001234567890012345678900123456789001234567890012345678900123456789001234567890012345678900123456789001234567890"));
    assert_true(TextTableAdd(&tTextTable, NULL, "row3 column4_row1"));

    assert_true(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_REGULAR_HEAD_ON, 0, 4));
    assert_true(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_REGULAR_HEAD_ON, 2, 4));
    assert_true(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_REGULAR_HEAD_OFF, 4, 4));
    tTextTable.charHeadX = '#';
    assert_true(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_SEPARATED_HEAD_ON, 6, 4));
    assert_true(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_SEPARATED_HEAD_OFF, 8, 4));
    assert_true(TextTablePrint(&tTextTable, PrintLine, TABSTYLE_COMACT, 10, 4));

    TextTableFree(&tTextTable);
}

/**
 * @brief    Run unit test with cmocka
 */
 int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    cmocka_set_message_output(CM_OUTPUT_STDOUT);

    const struct CMUnitTest tests[] = {
      cmocka_unit_test_setup_teardown(UTest_TextTableInit_Fail, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTableInit_Success, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTableFree_NULL, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTableAdd_TableNULL, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTableAdd_ansiSeqNULL, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTableAdd_ansiSeqTooLong, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTableAdd_formatNULL, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTablePrint_ColumnError, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTablePrint_TableNull, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTablePrint_CallbackNull, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTablePrint_posX, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTablePrint_columns0, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTablePrint_zeroEntries, TestSetup, TestTeardown),
      cmocka_unit_test_setup_teardown(UTest_TextTablePrint_SequenceOK, TestSetup, TestTeardown),
    };
    return cmocka_run_group_tests(tests, GroupSetup, GroupTeardown);
}

