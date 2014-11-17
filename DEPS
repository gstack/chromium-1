# This file is automatically processed to create .DEPS.git which is the file
# that gclient uses under git.
#
# See http://code.google.com/p/chromium/wiki/UsingGit
#
# To test manually, run:
#   python tools/deps2git/deps2git.py -o .DEPS.git
#   gclient runhooks
# DO NOT CHECK IN CHANGES TO .DEPS.git. It will be automatically updated by
# a bot when you modify this one.
#
# When adding a new dependency, please update the top-level .gitignore file
# to list the dependency's destination directory.

vars = {
  # Use this googlecode_url variable only if there is an internal mirror for it.
  # If you do not know, use the full path while defining your new deps entry.
  "googlecode_url": "http://%s.googlecode.com/svn",
  "sourceforge_url": "http://svn.code.sf.net/p/%(repo)s/code",
  "webkit_trunk": "http://src.chromium.org/blink/trunk",
  "nacl_trunk": "http://src.chromium.org/native_client/trunk",
  "webkit_revision": "161734",
  "chromium_git": "https://chromium.googlesource.com",
  "chromiumos_git": "https://chromium.googlesource.com/chromiumos",
  "chromium_rhino_git": "https://github.com/cvsuser-chromium", 
}

deps = {
  "src/src/rhino":
    Var("chromium_rhino_git") + "/librhino.git",
    
  "src/src/native_client":
    Var("chromium_rhino_git") + "/native_client.git",
    
  "src/src/tools":
    Var("chromium_rhino_git") + "/chromium-tools.git",
    
  "src/src/v8":
    Var("chromium_rhino_git") + "/chromium-v8.git",
  
  "src/src/third_party":
    Var("chromium_rhino_git") + "/chromium-third-party.git",
    
  "src/src/third_party/angle_dx11":
    Var("chromium_rhino_git") + "/chromium-third-party_angle_dx11.git",

  "src/src/third_party/ffmpeg":
    Var("chromium_rhino_git") + "/chromium-third-party_ffmpeg.git",

  "src/src/third_party/WebKit":
    Var("chromium_rhino_git") + "/third_party_WebKit.git",

  "src/src/third_party/icu":
    Var("chromium_rhino_git") + "/third_party_icu.git",

  "src/src/third_party/perl":
    Var("chromium_rhino_git") + "/third_party_perl.git",

  "src/src/third_party/hunspell_dictionaries":
    Var("chromium_rhino_git") + "/third_party_hunspell_dictionaries.git",

  "src/src/third_party/libjingle":
    Var("chromium_rhino_git") + "/libjingle.git",
    
} 


deps_os = {
  "win": {
  }
}


include_rules = [
  # Everybody can use some things.
  "+src/base",
  "+src/build",
  "+src/ipc",

  # Everybody can use headers generated by tools/generate_library_loader.
  #"+src/library_loaders",

  "+src/testing",
  "+src/third_party/icu/source/common/unicode",
  "+src/third_party/icu/source/i18n/unicode",
  "+src/url",
]



hooks = [
  {
    # Update LASTCHANGE. This is also run by export_tarball.py in
    # src/tools/export_tarball - please keep them in sync.
    "name": "lastchange",
    "pattern": ".",
    "action": ["python", "src/src/build/util/lastchange.py",
               "-o", "src/src/build/util/LASTCHANGE.chromium"],
  },
  {
    # Update LASTCHANGE.blink. This is also run by export_tarball.py in
    # src/tools/export_tarball - please keep them in sync.
    "name": "lastchange",
    "pattern": ".",
    "action": ["python", "src/src/build/util/lastchange.py",
               "-s", "src/src/rhino",
               "-o", "src/src/build/util/LASTCHANGE.rhino"],
  },  
  #{
    # A change to a .gyp, .gypi, or to GYP itself should run the generator.
    #"name": "gyp",
    #"pattern": ".",
    #"action": ["python", "src/src/build/gyp_chromium src/src/rhino/rhino.gyp --depth=."],
  #},
  {
    # A change to a .gyp, .gypi, or to GYP itself should run the generator.
    "name": "echo",
    "pattern": ".",
    "action": ["echo", "python build/gyp_chromium rhino/rhino.gyp --depth=."],
  },
]
