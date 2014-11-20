# Copyright 2009 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
{
  'variables': {
    'chromium_code': 1,
  },
  'conditions': [
    ['android_webview_build==0', {
      'includes': [
        'android_webview_tests.gypi',
      ],
    }],
  ],
  'targets': [
    {
      'target_name': 'libwebviewchromium',
      'type': 'shared_library',
      'android_unmangled_name': 1,
      'dependencies': [
        'android_webview_common',
      ],
      'conditions': [
        # Avoid clashes between the versions of this library built with
        # android_webview_build==1 by using a different name prefix.
        [ 'android_webview_build==0', {
          'product_prefix': 'libstandalone',
        }],
        # The general approach is to allow the executable target to choose
        # the allocator, but as in the WebView case we are building a library
        # only, put the dependency on the allocator here
        [ 'android_webview_build==1 and android_use_tcmalloc==1', {
          'dependencies': [
            '../base/allocator/allocator.gyp:allocator', ],
        }],
        [ 'android_webview_build==1 and use_system_skia==0', {
          # When not using the system skia there are linker warnings about
          # overriden hidden symbols which there's no easy way to eliminate;
          # disable them. http://crbug.com/157326
          'ldflags': [
            '-Wl,--no-fatal-warnings',
          ],
          'ldflags!': [
            '-Wl,--fatal-warnings',
          ],
        }],
      ],
      'sources': [
        'lib/main/webview_entry_point.cc',
      ],
    },
    {
      'target_name': 'android_webview_pak',
      'type': 'none',
      'dependencies': [
        '<(DEPTH)/content/content_resources.gyp:content_resources',
        '<(DEPTH)/net/net.gyp:net_resources',
        '<(DEPTH)/ui/ui.gyp:ui_resources',
        '<(DEPTH)/webkit/webkit_resources.gyp:webkit_resources',
      ],
      'variables': {
        'repack_path': '<(DEPTH)/tools/grit/grit/format/repack.py',
      },
      'actions': [
        {
          'action_name': 'repack_android_webview_pack',
          'variables': {
            'pak_inputs': [
              '<(SHARED_INTERMEDIATE_DIR)/content/content_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/net/net_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources/ui_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/webkit/blink_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/webkit/webkit_resources_100_percent.pak',
            ],
          },
          'inputs': [
            '<(repack_path)',
            '<@(pak_inputs)',
          ],
          'outputs': [
            '<(PRODUCT_DIR)/android_webview_apk/assets/webviewchromium.pak',
          ],
          'action': ['python', '<(repack_path)', '<@(_outputs)',
                     '<@(pak_inputs)'],
        }
      ],
    },
    {
      'target_name': 'android_webview_common',
      'type': 'static_library',
      'dependencies': [
        '../android_webview/native/webview_native.gyp:webview_native',
        '../components/components.gyp:auto_login_parser',
        '../components/components.gyp:autofill_content_renderer',
        '../components/components.gyp:navigation_interception',
        '../components/components.gyp:visitedlink_browser',
        '../components/components.gyp:visitedlink_renderer',
        '../components/components.gyp:web_contents_delegate_android',
        '../content/content.gyp:content_app_both',
        '../gpu/gpu.gyp:command_buffer_service',
        '../gpu/gpu.gyp:gles2_implementation',
        '../skia/skia.gyp:skia',
        '../ui/gl/gl.gyp:gl',
        '../ui/shell_dialogs/shell_dialogs.gyp:shell_dialogs',
        '../webkit/common/gpu/webkit_gpu.gyp:webkit_gpu',
        'android_webview_pak',
      ],
      'include_dirs': [
        '..',
        '../skia/config',
        '<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources/',
      ],
      'sources': [
        'browser/aw_browser_context.cc',
        'browser/aw_browser_context.h',
        'browser/aw_browser_main_parts.cc',
        'browser/aw_browser_main_parts.h',
        'browser/aw_contents_client_bridge_base.cc',
        'browser/aw_contents_client_bridge_base.h',
        'browser/aw_content_browser_client.cc',
        'browser/aw_content_browser_client.h',
        'browser/aw_contents_io_thread_client.h',
        'browser/aw_cookie_access_policy.cc',
        'browser/aw_cookie_access_policy.h',
        'browser/aw_download_manager_delegate.cc',
        'browser/aw_download_manager_delegate.h',
        'browser/aw_form_database_service.cc',
        'browser/aw_form_database_service.h',
        'browser/aw_gl_surface.cc',
        'browser/aw_gl_surface.h',
        'browser/aw_http_auth_handler_base.cc',
        'browser/aw_http_auth_handler_base.h',
        'browser/aw_javascript_dialog_manager.cc',
        'browser/aw_javascript_dialog_manager.h',
        'browser/aw_login_delegate.cc',
        'browser/aw_login_delegate.h',
        'browser/aw_pref_store.cc',
        'browser/aw_pref_store.h',
        'browser/aw_quota_manager_bridge.cc',
        'browser/aw_quota_manager_bridge.h',
        'browser/aw_quota_permission_context.cc',
        'browser/aw_quota_permission_context.h',
        'browser/aw_request_interceptor.cc',
        'browser/aw_request_interceptor.h',
        'browser/aw_result_codes.h',
        'browser/aw_web_preferences_populater.cc',
        'browser/aw_web_preferences_populater.h',
        'browser/browser_view_renderer.h',
        'browser/find_helper.cc',
        'browser/find_helper.h',
        'browser/gpu_memory_buffer_factory_impl.cc',
        'browser/gpu_memory_buffer_factory_impl.h',
        'browser/icon_helper.cc',
        'browser/icon_helper.h',
        'browser/in_process_view_renderer.cc',
        'browser/in_process_view_renderer.h',
        'browser/input_stream.h',
        'browser/intercepted_request_data.h',
        'browser/jni_dependency_factory.h',
        'browser/gl_view_renderer_manager.cc',
        'browser/gl_view_renderer_manager.h',
        'browser/net/android_stream_reader_url_request_job.cc',
        'browser/net/android_stream_reader_url_request_job.h',
        'browser/net/aw_network_delegate.cc',
        'browser/net/aw_network_delegate.h',
        'browser/net/aw_url_request_context_getter.cc',
        'browser/net/aw_url_request_context_getter.h',
        'browser/net/aw_url_request_job_factory.cc',
        'browser/net/aw_url_request_job_factory.h',
        'browser/net_disk_cache_remover.cc',
        'browser/net_disk_cache_remover.h',
        'browser/net/init_native_callback.h',
        'browser/net/input_stream_reader.cc',
        'browser/net/input_stream_reader.h',
        'browser/renderer_host/aw_render_view_host_ext.cc',
        'browser/renderer_host/aw_render_view_host_ext.h',
        'browser/renderer_host/aw_resource_dispatcher_host_delegate.cc',
        'browser/renderer_host/aw_resource_dispatcher_host_delegate.h',
        'browser/scoped_allow_wait_for_legacy_web_view_api.h',
        'browser/scoped_app_gl_state_restore.cc',
        'browser/scoped_app_gl_state_restore.h',
        'common/android_webview_message_generator.cc',
        'common/android_webview_message_generator.h',
        'common/aw_content_client.cc',
        'common/aw_content_client.h',
        'common/aw_hit_test_data.cc',
        'common/aw_hit_test_data.h',
        'common/aw_resource.h',
        'common/aw_switches.cc',
        'common/aw_switches.h',
        'common/render_view_messages.cc',
        'common/render_view_messages.h',
        'common/url_constants.cc',
        'common/url_constants.h',
        'lib/aw_browser_dependency_factory_impl.cc',
        'lib/aw_browser_dependency_factory_impl.h',
        'lib/main/aw_main_delegate.cc',
        'lib/main/aw_main_delegate.h',
        'public/browser/draw_gl.h',
        'renderer/aw_content_renderer_client.cc',
        'renderer/aw_content_renderer_client.h',
        'renderer/aw_render_process_observer.cc',
        'renderer/aw_render_process_observer.h',
        'renderer/aw_render_view_ext.cc',
        'renderer/aw_render_view_ext.h',
      ],
    },
    {
      'target_name': 'android_webview_java',
      'type': 'none',
      'dependencies': [
        '../components/components.gyp:navigation_interception_java',
        '../components/components.gyp:web_contents_delegate_android_java',
        '../content/content.gyp:content_java',
        '../ui/ui.gyp:ui_java',
      ],
      'variables': {
        'java_in_dir': '../android_webview/java',
      },
      'includes': [ '../build/java.gypi' ],
    },
  ],
}