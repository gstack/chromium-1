// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/utility/importer/external_process_importer_bridge.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task_runner.h"
#include "base/values.h"
#include "chrome/common/importer/imported_bookmark_entry.h"
#include "chrome/common/importer/imported_favicon_usage.h"
#include "chrome/common/importer/importer_data_types.h"
#include "chrome/common/importer/profile_import_process_messages.h"
#include "components/autofill/core/common/password_form.h"
#include "ipc/ipc_sender.h"

namespace {

// Rather than sending all import items over IPC at once we chunk them into
// separate requests.  This avoids the case of a large import causing
// oversized IPC messages.
const int kNumBookmarksToSend = 100;
const int kNumHistoryRowsToSend = 100;
const int kNumFaviconsToSend = 100;

}

ExternalProcessImporterBridge::ExternalProcessImporterBridge(
    const DictionaryValue& localized_strings,
    IPC::Sender* sender,
    base::TaskRunner* task_runner)
    : sender_(sender),
      task_runner_(task_runner) {
  // Bridge needs to make its own copy because OS 10.6 autoreleases the
  // localized_strings value that is passed in (see http://crbug.com/46003 ).
  localized_strings_.reset(localized_strings.DeepCopy());
}

void ExternalProcessImporterBridge::AddBookmarks(
    const std::vector<ImportedBookmarkEntry>& bookmarks,
    const string16& first_folder_name) {
  Send(new ProfileImportProcessHostMsg_NotifyBookmarksImportStart(
      first_folder_name, bookmarks.size()));

  // |bookmarks_left| is required for the checks below as Windows has a
  // Debug bounds-check which prevents pushing an iterator beyond its end()
  // (i.e., |it + 2 < s.end()| crashes in debug mode if |i + 1 == s.end()|).
  int bookmarks_left = bookmarks.end() - bookmarks.begin();
  for (std::vector<ImportedBookmarkEntry>::const_iterator it =
           bookmarks.begin(); it < bookmarks.end();) {
    std::vector<ImportedBookmarkEntry> bookmark_group;
    std::vector<ImportedBookmarkEntry>::const_iterator end_group =
        it + std::min(bookmarks_left, kNumBookmarksToSend);
    bookmark_group.assign(it, end_group);

    Send(new ProfileImportProcessHostMsg_NotifyBookmarksImportGroup(
        bookmark_group));
    bookmarks_left -= end_group - it;
    it = end_group;
  }
  DCHECK_EQ(0, bookmarks_left);
}

void ExternalProcessImporterBridge::AddHomePage(const GURL& home_page) {
  Send(new ProfileImportProcessHostMsg_NotifyHomePageImportReady(home_page));
}

#if defined(OS_WIN)
void ExternalProcessImporterBridge::AddIE7PasswordInfo(
    const importer::ImporterIE7PasswordInfo& password_info) {
  Send(new ProfileImportProcessHostMsg_NotifyIE7PasswordInfo(password_info));
}
#endif

void ExternalProcessImporterBridge::SetFavicons(
    const std::vector<ImportedFaviconUsage>& favicons) {
  Send(new ProfileImportProcessHostMsg_NotifyFaviconsImportStart(
    favicons.size()));

  // |favicons_left| is required for the checks below as Windows has a
  // Debug bounds-check which prevents pushing an iterator beyond its end()
  // (i.e., |it + 2 < s.end()| crashes in debug mode if |i + 1 == s.end()|).
  int favicons_left = favicons.end() - favicons.begin();
  for (std::vector<ImportedFaviconUsage>::const_iterator it =
           favicons.begin(); it < favicons.end();) {
    std::vector<ImportedFaviconUsage> favicons_group;
    std::vector<ImportedFaviconUsage>::const_iterator end_group =
        it + std::min(favicons_left, kNumFaviconsToSend);
    favicons_group.assign(it, end_group);

    Send(new ProfileImportProcessHostMsg_NotifyFaviconsImportGroup(
        favicons_group));
    favicons_left -= end_group - it;
    it = end_group;
  }
  DCHECK_EQ(0, favicons_left);
}

void ExternalProcessImporterBridge::SetHistoryItems(
    const std::vector<ImporterURLRow>& rows,
    importer::VisitSource visit_source) {
  Send(new ProfileImportProcessHostMsg_NotifyHistoryImportStart(rows.size()));

  // |rows_left| is required for the checks below as Windows has a
  // Debug bounds-check which prevents pushing an iterator beyond its end()
  // (i.e., |it + 2 < s.end()| crashes in debug mode if |i + 1 == s.end()|).
  int rows_left = rows.end() - rows.begin();
  for (std::vector<ImporterURLRow>::const_iterator it = rows.begin();
       it < rows.end();) {
    std::vector<ImporterURLRow> row_group;
    std::vector<ImporterURLRow>::const_iterator end_group =
        it + std::min(rows_left, kNumHistoryRowsToSend);
    row_group.assign(it, end_group);

    Send(new ProfileImportProcessHostMsg_NotifyHistoryImportGroup(
        row_group, visit_source));
    rows_left -= end_group - it;
    it = end_group;
  }
  DCHECK_EQ(0, rows_left);
}

void ExternalProcessImporterBridge::SetKeywords(
    const std::vector<importer::URLKeywordInfo>& url_keywords,
    bool unique_on_host_and_path) {
  Send(new ProfileImportProcessHostMsg_NotifyKeywordsReady(
      url_keywords, unique_on_host_and_path));
}

void ExternalProcessImporterBridge::SetFirefoxSearchEnginesXMLData(
    const std::vector<std::string>& search_engine_data) {
  Send(new ProfileImportProcessHostMsg_NotifyFirefoxSearchEngData(
      search_engine_data));
}

void ExternalProcessImporterBridge::SetPasswordForm(
    const autofill::PasswordForm& form) {
  Send(new ProfileImportProcessHostMsg_NotifyPasswordFormReady(form));
}

void ExternalProcessImporterBridge::NotifyStarted() {
  Send(new ProfileImportProcessHostMsg_Import_Started());
}

void ExternalProcessImporterBridge::NotifyItemStarted(
    importer::ImportItem item) {
  Send(new ProfileImportProcessHostMsg_ImportItem_Started(item));
}

void ExternalProcessImporterBridge::NotifyItemEnded(importer::ImportItem item) {
  Send(new ProfileImportProcessHostMsg_ImportItem_Finished(item));
}

void ExternalProcessImporterBridge::NotifyEnded() {
  // The internal process detects import end when all items have been received.
}

string16 ExternalProcessImporterBridge::GetLocalizedString(int message_id) {
  string16 message;
  localized_strings_->GetString(base::IntToString(message_id), &message);
  return message;
}

ExternalProcessImporterBridge::~ExternalProcessImporterBridge() {}

void ExternalProcessImporterBridge::Send(IPC::Message* message) {
  task_runner_->PostTask(
      FROM_HERE,
      base::Bind(&ExternalProcessImporterBridge::SendInternal,
                 this, message));
}

void ExternalProcessImporterBridge::SendInternal(IPC::Message* message) {
  DCHECK(task_runner_->RunsTasksOnCurrentThread());
  sender_->Send(message);
}
