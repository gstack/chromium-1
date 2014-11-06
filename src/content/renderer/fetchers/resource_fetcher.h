// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// A wrapper around ResourceHandle and ResourceHandleClient that simplifies
// the download of an HTTP object.  The interface is modeled after URLFetcher
// in the /chrome/browser.
//
// ResourceFetcher::Delegate::OnURLFetchComplete will be called async after
// the ResourceFetcher object is created.

#ifndef CONTENT_RENDERER_FETCHERS_RESOURCE_FETCHER_H_
#define CONTENT_RENDERER_FETCHERS_RESOURCE_FETCHER_H_

#include <string>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/timer/timer.h"
#include "content/common/content_export.h"
#include "third_party/WebKit/public/platform/WebURLLoaderClient.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "url/gurl.h"

class GURL;

namespace blink {
class WebFrame;
class WebURLLoader;
struct WebURLError;
}

namespace content {

class CONTENT_EXPORT ResourceFetcher
    : NON_EXPORTED_BASE(public blink::WebURLLoaderClient) {
 public:
  // This will be called when the URL has been fetched, successfully or not.
  // If there is a failure, response and data will both be empty.  |response|
  // and |data| are both valid until the URLFetcher instance is destroyed.
  typedef base::Callback<void(const blink::WebURLResponse&,
                              const std::string&)> Callback;

  // We need a frame to make requests.
  ResourceFetcher(
      const GURL& url, blink::WebFrame* frame,
      blink::WebURLRequest::TargetType target_type,
      const Callback& callback);
  virtual ~ResourceFetcher();

  // Stop the request and don't call the callback.
  void Cancel();

  bool completed() const { return completed_; }

 protected:
  // WebURLLoaderClient methods:
  virtual void willSendRequest(
      blink::WebURLLoader* loader, blink::WebURLRequest& new_request,
      const blink::WebURLResponse& redirect_response);
  virtual void didSendData(
      blink::WebURLLoader* loader, unsigned long long bytes_sent,
      unsigned long long total_bytes_to_be_sent);
  virtual void didReceiveResponse(
      blink::WebURLLoader* loader, const blink::WebURLResponse& response);
  virtual void didReceiveCachedMetadata(
      blink::WebURLLoader* loader, const char* data, int data_length);

  virtual void didReceiveData(
      blink::WebURLLoader* loader, const char* data, int data_length,
      int encoded_data_length);
  virtual void didFinishLoading(
      blink::WebURLLoader* loader, double finishTime);
  virtual void didFail(
      blink::WebURLLoader* loader, const blink::WebURLError& error);

  scoped_ptr<blink::WebURLLoader> loader_;

  // URL we're fetching
  GURL url_;

  // Target type
  blink::WebURLRequest::TargetType target_type_;

  // A copy of the original resource response
  blink::WebURLResponse response_;

  // Set to true once the request is compelte.
  bool completed_;

 private:
  // Start the actual download.
  void Start(blink::WebFrame* frame);

  void RunCallback(const blink::WebURLResponse& response,
                   const std::string& data);

  // Callback when we're done
  Callback callback_;

  // Buffer to hold the content from the server.
  std::string data_;

  // Buffer to hold metadata from the cache.
  std::string metadata_;
};

/////////////////////////////////////////////////////////////////////////////
// A resource fetcher with a timeout
class CONTENT_EXPORT ResourceFetcherWithTimeout
    : NON_EXPORTED_BASE(public ResourceFetcher) {
 public:
  ResourceFetcherWithTimeout(const GURL& url,
                             blink::WebFrame* frame,
                             blink::WebURLRequest::TargetType target_type,
                             int timeout_secs,
                             const Callback& callback);
  virtual ~ResourceFetcherWithTimeout();

 private:
  // Callback for timer that limits how long we wait for the alternate error
  // page server.  If this timer fires and the request hasn't completed, we
  // kill the request.
  void TimeoutFired();

  // Limit how long we wait for the alternate error page server.
  base::OneShotTimer<ResourceFetcherWithTimeout> timeout_timer_;
};

}  // namespace content

#endif  // CONTENT_RENDERER_FETCHERS_RESOURCE_FETCHER_H_
