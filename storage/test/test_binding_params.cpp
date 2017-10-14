/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: sw=2 ts=2 et lcs=trail\:.,tab\:>~ :
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is storage test code.
 *
 * The Initial Developer of the Original Code is
 * Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Daniel Witte <dwitte@mozilla.com> (Original Author)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "storage_test_harness.h"

#include "mozStorageHelper.h"
  
/**
 * This file tests binding and reading out string parameters through the
 * mozIStorageStatement API.
 */

void
test_ASCIIString()
{
  nsCOMPtr<mozIStorageConnection> db(getMemoryDatabase());

  // Create table with a single string column.
  (void)db->ExecuteSimpleSQL(NS_LITERAL_CSTRING(
    "CREATE TABLE test (str STRING)"
  ));

  // Create statements to INSERT and SELECT the string.
  nsCOMPtr<mozIStorageStatement> insert, select;
  (void)db->CreateStatement(NS_LITERAL_CSTRING(
    "INSERT INTO test (str) VALUES (?1)"
  ), getter_AddRefs(insert));
  (void)db->CreateStatement(NS_LITERAL_CSTRING(
    "SELECT str FROM test"
  ), getter_AddRefs(select));

  // Roundtrip a string through the table, and ensure it comes out as expected.
  nsCAutoString inserted("I'm an ASCII string");
  {
    mozStorageStatementScoper scoper(insert);
    PRBool hasResult;
    do_check_true(NS_SUCCEEDED(insert->BindUTF8StringParameter(0, inserted)));
    do_check_true(NS_SUCCEEDED(insert->ExecuteStep(&hasResult)));
    do_check_false(hasResult);
  }

  nsCAutoString result;
  {
    mozStorageStatementScoper scoper(select);
    PRBool hasResult;
    do_check_true(NS_SUCCEEDED(select->ExecuteStep(&hasResult)));
    do_check_true(hasResult);
    do_check_true(NS_SUCCEEDED(select->GetUTF8String(0, result)));
  }

  do_check_true(result == inserted);

  (void)db->ExecuteSimpleSQL(NS_LITERAL_CSTRING("DELETE FROM test"));
}

void
test_CString()
{
  nsCOMPtr<mozIStorageConnection> db(getMemoryDatabase());

  // Create table with a single string column.
  (void)db->ExecuteSimpleSQL(NS_LITERAL_CSTRING(
    "CREATE TABLE test (str STRING)"
  ));

  // Create statements to INSERT and SELECT the string.
  nsCOMPtr<mozIStorageStatement> insert, select;
  (void)db->CreateStatement(NS_LITERAL_CSTRING(
    "INSERT INTO test (str) VALUES (?1)"
  ), getter_AddRefs(insert));
  (void)db->CreateStatement(NS_LITERAL_CSTRING(
    "SELECT str FROM test"
  ), getter_AddRefs(select));

  // Roundtrip a string through the table, and ensure it comes out as expected.
  static const char sCharArray[] =
    "I'm not a \xff\x00\xac\xde\xbb ASCII string!";
  nsCAutoString inserted(sCharArray, NS_ARRAY_LENGTH(sCharArray) - 1);
  do_check_true(inserted.Length() == NS_ARRAY_LENGTH(sCharArray) - 1);
  {
    mozStorageStatementScoper scoper(insert);
    PRBool hasResult;
    do_check_true(NS_SUCCEEDED(insert->BindUTF8StringParameter(0, inserted)));
    do_check_true(NS_SUCCEEDED(insert->ExecuteStep(&hasResult)));
    do_check_false(hasResult);
  }

  {
    nsCAutoString result;

    mozStorageStatementScoper scoper(select);
    PRBool hasResult;
    do_check_true(NS_SUCCEEDED(select->ExecuteStep(&hasResult)));
    do_check_true(hasResult);
    do_check_true(NS_SUCCEEDED(select->GetUTF8String(0, result)));

    do_check_true(result == inserted);
  }

  (void)db->ExecuteSimpleSQL(NS_LITERAL_CSTRING("DELETE FROM test"));
}

void
test_UTFStrings()
{
  nsCOMPtr<mozIStorageConnection> db(getMemoryDatabase());

  // Create table with a single string column.
  (void)db->ExecuteSimpleSQL(NS_LITERAL_CSTRING(
    "CREATE TABLE test (str STRING)"
  ));

  // Create statements to INSERT and SELECT the string.
  nsCOMPtr<mozIStorageStatement> insert, select;
  (void)db->CreateStatement(NS_LITERAL_CSTRING(
    "INSERT INTO test (str) VALUES (?1)"
  ), getter_AddRefs(insert));
  (void)db->CreateStatement(NS_LITERAL_CSTRING(
    "SELECT str FROM test"
  ), getter_AddRefs(select));

  // Roundtrip a UTF8 string through the table, using UTF8 input and output.
  static const char sCharArray[] =
    "I'm a \xc3\xbb\xc3\xbc\xc3\xa2\xc3\xa4\xc3\xa7 UTF8 string!";
  nsCAutoString insertedUTF8(sCharArray, NS_ARRAY_LENGTH(sCharArray) - 1);
  do_check_true(insertedUTF8.Length() == NS_ARRAY_LENGTH(sCharArray) - 1);
  NS_ConvertUTF8toUTF16 insertedUTF16(insertedUTF8);
  do_check_true(insertedUTF8 == NS_ConvertUTF16toUTF8(insertedUTF16));
  {
    mozStorageStatementScoper scoper(insert);
    PRBool hasResult;
    do_check_true(NS_SUCCEEDED(insert->BindUTF8StringParameter(0, insertedUTF8)));
    do_check_true(NS_SUCCEEDED(insert->ExecuteStep(&hasResult)));
    do_check_false(hasResult);
  }

  {
    nsCAutoString result;

    mozStorageStatementScoper scoper(select);
    PRBool hasResult;
    do_check_true(NS_SUCCEEDED(select->ExecuteStep(&hasResult)));
    do_check_true(hasResult);
    do_check_true(NS_SUCCEEDED(select->GetUTF8String(0, result)));

    do_check_true(result == insertedUTF8);
  }

  // Use UTF8 input and UTF16 output.
  {
    nsAutoString result;

    mozStorageStatementScoper scoper(select);
    PRBool hasResult;
    do_check_true(NS_SUCCEEDED(select->ExecuteStep(&hasResult)));
    do_check_true(hasResult);
    do_check_true(NS_SUCCEEDED(select->GetString(0, result)));

    do_check_true(result == insertedUTF16);
  }

  (void)db->ExecuteSimpleSQL(NS_LITERAL_CSTRING("DELETE FROM test"));

  // Roundtrip the same string using UTF16 input and UTF8 output.
  {
    mozStorageStatementScoper scoper(insert);
    PRBool hasResult;
    do_check_true(NS_SUCCEEDED(insert->BindStringParameter(0, insertedUTF16)));
    do_check_true(NS_SUCCEEDED(insert->ExecuteStep(&hasResult)));
    do_check_false(hasResult);
  }

  {
    nsCAutoString result;

    mozStorageStatementScoper scoper(select);
    PRBool hasResult;
    do_check_true(NS_SUCCEEDED(select->ExecuteStep(&hasResult)));
    do_check_true(hasResult);
    do_check_true(NS_SUCCEEDED(select->GetUTF8String(0, result)));

    do_check_true(result == insertedUTF8);
  }

  // Use UTF16 input and UTF16 output.
  {
    nsAutoString result;

    mozStorageStatementScoper scoper(select);
    PRBool hasResult;
    do_check_true(NS_SUCCEEDED(select->ExecuteStep(&hasResult)));
    do_check_true(hasResult);
    do_check_true(NS_SUCCEEDED(select->GetString(0, result)));

    do_check_true(result == insertedUTF16);
  }

  (void)db->ExecuteSimpleSQL(NS_LITERAL_CSTRING("DELETE FROM test"));
}

void (*gTests[])(void) = {
  test_ASCIIString,
  test_CString,
  test_UTFStrings,
};

const char *file = __FILE__;
#define TEST_NAME "binding string params"
#define TEST_FILE file
#include "storage_test_harness_tail.h"
