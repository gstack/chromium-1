// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This file contains the keyword autocomplete provider. The keyword provider
// is responsible for remembering/suggesting user "search keyword queries"
// (e.g.  "imdb Godzilla") and then fixing them up into valid URLs.  An
// instance of it gets created and managed by the autocomplete controller.
// KeywordProvider uses a TemplateURLService to find the set of keywords.

#ifndef CHROME_BROWSER_AUTOCOMPLETE_KEYWORD_PROVIDER_H_
#define CHROME_BROWSER_AUTOCOMPLETE_KEYWORD_PROVIDER_H_

#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "chrome/browser/autocomplete/autocomplete_input.h"
#include "chrome/browser/autocomplete/autocomplete_provider.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"

class Profile;
class TemplateURL;
class TemplateURLService;

// Autocomplete provider for keyword input.
//
// After construction, the autocomplete controller repeatedly calls Start()
// with some user input, each time expecting to receive a small set of the best
// matches (either synchronously or asynchronously).
//
// To construct these matches, the provider treats user input as a series of
// whitespace-delimited tokens and tries to match the first token as the prefix
// of a known "keyword".  A keyword is some string that maps to a search query
// URL; the rest of the user's input is taken as the input to the query.  For
// example, the keyword "bug" might map to the URL "http://b/issue?id=%s", so
// input like "bug 123" would become "http://b/issue?id=123".
//
// Because we do prefix matching, user input could match more than one keyword
// at once.  (Example: the input "f jazz" matches all keywords starting with
// "f".)  We return the best matches, up to three.
//
// The resulting matches are shown with content specified by the keyword
// (usually "Search [name] for %s"), description "(Keyword: [keyword])", and
// action "[keyword] %s".  If the user has typed a (possibly partial) keyword
// but no search terms, the suggested result is shown greyed out, with
// "<enter term(s)>" as the substituted input, and does nothing when selected.
class KeywordProvider : public AutocompleteProvider,
                        public content::NotificationObserver {
 public:
  KeywordProvider(AutocompleteProviderListener* listener, Profile* profile);
  // For testing.
  KeywordProvider(AutocompleteProviderListener* listener,
                  TemplateURLService* model);

  // Extracts the next whitespace-delimited token from input and returns it.
  // Sets |remaining_input| to everything after the first token (skipping over
  // the first intervening whitespace).
  // If |trim_leading_whitespace| is true then leading whitespace in
  // |*remaining_input| will be trimmed.
  static string16 SplitKeywordFromInput(const string16& input,
                                        bool trim_leading_whitespace,
                                        string16* remaining_input);

  // Returns the replacement string from the user input. The replacement
  // string is the portion of the input that does not contain the keyword.
  // For example, the replacement string for "b blah" is blah.
  // If |trim_leading_whitespace| is true then leading whitespace in
  // replacement string will be trimmed.
  static string16 SplitReplacementStringFromInput(const string16& input,
                                                  bool trim_leading_whitespace);

  // Returns the matching substituting keyword for |input|, or NULL if there
  // is no keyword for the specified input.  If the matching keyword was found,
  // updates |input|'s text and cursor position.
  static const TemplateURL* GetSubstitutingTemplateURLForInput(
      TemplateURLService* model,
      AutocompleteInput* input);

  // If |text| corresponds (in the sense of
  // TemplateURLModel::CleanUserInputKeyword()) to an enabled, substituting
  // keyword, returns that keyword; returns the empty string otherwise.
  string16 GetKeywordForText(const string16& text) const;

  // Creates a fully marked-up AutocompleteMatch for a specific keyword.
  AutocompleteMatch CreateVerbatimMatch(const string16& text,
                                        const string16& keyword,
                                        const AutocompleteInput& input);

  // AutocompleteProvider:
  virtual void Start(const AutocompleteInput& input,
                     bool minimal_changes) OVERRIDE;
  virtual void Stop(bool clear_cached_results) OVERRIDE;

 private:
  class ScopedEndExtensionKeywordMode;
  friend class ScopedEndExtensionKeywordMode;

  virtual ~KeywordProvider();

  // Extracts the keyword from |input| into |keyword|. Any remaining characters
  // after the keyword are placed in |remaining_input|. Returns true if |input|
  // is valid and has a keyword. This makes use of SplitKeywordFromInput to
  // extract the keyword and remaining string, and uses
  // TemplateURLService::CleanUserInputKeyword to remove unnecessary characters.
  // In general use this instead of SplitKeywordFromInput.
  // Leading whitespace in |*remaining_input| will be trimmed.
  static bool ExtractKeywordFromInput(const AutocompleteInput& input,
                                      string16* keyword,
                                      string16* remaining_input);

  // Determines the relevance for some input, given its type, whether the user
  // typed the complete keyword, and whether the user is in "prefer keyword
  // matches" mode, and whether the keyword supports replacement.
  // If |allow_exact_keyword_match| is false, the relevance for complete
  // keywords that support replacements is degraded.
  static int CalculateRelevance(AutocompleteInput::Type type,
                                bool complete,
                                bool support_replacement,
                                bool prefer_keyword,
                                bool allow_exact_keyword_match);

  // Creates a fully marked-up AutocompleteMatch from the user's input.
  // If |relevance| is negative, calculate a relevance based on heuristics.
  AutocompleteMatch CreateAutocompleteMatch(const TemplateURL* template_url,
                                            const AutocompleteInput& input,
                                            size_t prefix_length,
                                            const string16& remaining_input,
                                            bool allowed_to_be_default_match,
                                            int relevance);

  // Fills in the "destination_url" and "contents" fields of |match| with the
  // provided user input and keyword data.
  void FillInURLAndContents(const string16& remaining_input,
                            const TemplateURL* element,
                            AutocompleteMatch* match) const;

  void EnterExtensionKeywordMode(const std::string& extension_id);
  void MaybeEndExtensionKeywordMode();

  // content::NotificationObserver interface.
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

  TemplateURLService* GetTemplateURLService() const;

  // Model for the keywords.  This is only non-null when testing, otherwise the
  // TemplateURLService from the Profile is used.
  TemplateURLService* model_;

  // Identifies the current input state. This is incremented each time the
  // autocomplete edit's input changes in any way. It is used to tell whether
  // suggest results from the extension are current.
  int current_input_id_;

  // The input state at the time we last asked the extension for suggest
  // results.
  AutocompleteInput extension_suggest_last_input_;

  // We remember the last suggestions we've received from the extension in case
  // we need to reset our matches without asking the extension again.
  std::vector<AutocompleteMatch> extension_suggest_matches_;

  // If non-empty, holds the ID of the extension whose keyword is currently in
  // the URL bar while the autocomplete popup is open.
  std::string current_keyword_extension_id_;

  content::NotificationRegistrar registrar_;

  DISALLOW_COPY_AND_ASSIGN(KeywordProvider);
};

#endif  // CHROME_BROWSER_AUTOCOMPLETE_KEYWORD_PROVIDER_H_
