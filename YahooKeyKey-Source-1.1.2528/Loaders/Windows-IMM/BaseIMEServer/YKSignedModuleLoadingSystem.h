//
// Developed for Yahoo! Taiwan by Lithoglyph Inc.
// Copyright (c) 2007-2010 Yahoo! Taiwan.
//

#ifndef YKSignedModuleLoadingSystem_h
#define YKSignedModuleLoadingSystem_h

#include "Minotaur.h"
#include "ModuleDigestSharedSecret.h"
#include "PlainVanilla.h"

#if !defined(__APPLE__)
#include "LFPlatform.h"
#include "PVDLLLoadingSystem.h"
#else
#include <CoreFoundation/CoreFoundation.h>
#endif

extern "C" {
extern char ModulePublicKey[];
extern size_t ModulePublicKeySize;
};

namespace OpenVanilla {
using namespace std;

#if !defined(__APPLE__)
using namespace LFPlatform;
#endif

using namespace Minotaur;

class YKSignedModuleLoadingSystem : public PVCommonPackageLoadingSystem {
 public:
  YKSignedModuleLoadingSystem(PVLoaderPolicy* policy)
      : PVCommonPackageLoadingSystem(policy) {}

  virtual ~YKSignedModuleLoadingSystem() {
#if defined(__APPLE__)
    unloadAllUnloadables();
#endif
  }

  virtual void reset() {
    m_localizedNameMap.clear();
    m_versionMap.clear();
    PVCommonPackageLoadingSystem::reset();
  }

  virtual const string localizedNameForPackage(const string& name,
                                               OVLoaderService* loaderService) {
    map<string, PackageMetadata>::iterator iter = m_modulePackages.find(name);
    if (iter == m_modulePackages.end()) return name;

    map<string, string>::iterator mi = m_localizedNameMap.find(name);
    if (mi != m_localizedNameMap.end()) {
      return (*mi).second;
    }

#if defined(__APPLE__)
    PackageMetadata& pkgdata = (*iter).second;
    if (!pkgdata.library) {
      return name;
    }

    NSDictionary* dict = (NSDictionary*)CFBundleGetLocalInfoDictionary(
        (CFBundleRef)pkgdata.library);
    NSString* displayName = [dict objectForKey:@"CFBundleDisplayName"];

    // NSLog(@"Obtained display name for package: %@, dict: %@", displayName,
    // dict);

    if (!displayName) {
      return name;
    }

    string ln = [displayName UTF8String];
    m_localizedNameMap[name] = ln;

    return ln;

#else

    string plistPath =
        OVPathHelper::PathCat(contentPathFromPackageRoot((*iter).second.path),
                              kMinotaurModulePackagePlistName);
    if (!OVPathHelper::PathExists(plistPath)) {
      return name;
    }

    PVPropertyList plist(plistPath);
    PVPlistValue* dict = plist.rootDictionary();
    PVPlistValue* lnd = dict->valueForKey("MPLocalizedName");
    string ln;

    if (!lnd) {
      m_localizedNameMap[name] = name;
      return name;
    }

    if (lnd->type() != PVPlistValue::Dictionary) {
      m_localizedNameMap[name] = name;
      return name;
    }

    ln = lnd->stringValueForKey(loaderService->locale());
    if (!ln.length()) {
      ln = lnd->stringValueForKey("en");
    }

    if (!ln.length()) {
      m_localizedNameMap[name] = name;
    } else {
      m_localizedNameMap[name] = ln;
    }

    return ln;
#endif
  }

  const map<string, string> identifiableVersionMap() const {
    return m_versionMap;
  }

 protected:
  map<string, string> m_localizedNameMap;
  map<string, string> m_versionMap;

  virtual const string contentPathFromPackageRoot(const string& path) const {
#if defined(__APPLE__)
    return OVPathHelper::PathCat(path, "Contents");
#else
    return path;
#endif
  }

  virtual const string binaryPathFromPackageRoot(const string& path) const {
#if defined(__APPLE__)
    return OVPathHelper::PathCat(contentPathFromPackageRoot(path), "MacOS");
#else
    return path;
#endif
  }

  virtual void* loadLibrary(const string& path) {
#if !defined(__APPLE__)
    Logger logger("YKSignedModuleLoadingSystem");
    logger.debug("Attempting to load: %s", path.c_str());
#else
// NSLog(@"Attempting to load: %s", path.c_str());
#endif

    string plistPath = OVPathHelper::PathCat(contentPathFromPackageRoot(path),
                                             kMinotaurModulePackagePlistName);

#if !defined(__APPLE__)
    logger.debug("Loading plist: %s", plistPath.c_str());
#else
// NSLog(@"Loading plist: %s", plistPath.c_str());
#endif

    if (!OVPathHelper::PathExists(plistPath)) {
#if !defined(__APPLE__)
      logger.debug("Plist doesn't exist");
#else
      // NSLog(@"Plist doesn't exist");
#endif
      return 0;
    }

    PVPropertyList plist(plistPath);
    PVPlistValue* dict = plist.rootDictionary();
    string primaryBinaryPath =
        dict->stringValueForKey(kMinotaurModulePackagePrimaryBinary);

    if (!primaryBinaryPath.length()) {
#if !defined(__APPLE__)
      logger.debug("Invalid module: %s", path.c_str());
#else
      NSLog(@"Invalid module: %s", path.c_str());
#endif
      return 0;
    }

    string mpId = dict->stringValueForKey(kMinotaurModulePackageIdentifier);
    if (!mpId.length()) {
      return 0;
    }

    primaryBinaryPath = OVPathHelper::PathCat(binaryPathFromPackageRoot(path),
                                              primaryBinaryPath);

    pair<char*, size_t> binData = OVFileHelper::SlurpFile(primaryBinaryPath);
    if (!binData.first || !binData.second) {
#if !defined(__APPLE__)
      logger.debug("Cannot load binary: %s", primaryBinaryPath.c_str());
#else
      NSLog(@"Cannot load binary: %s", primaryBinaryPath.c_str());
#endif
      return 0;
    }

    pair<char*, size_t> keyData(ModulePublicKey, ModulePublicKeySize);

    string secret = mpId + TAKAO_MODULE_DIGEST_SHARED_SECRET;
    pair<char*, size_t> block;
    block.second = secret.size() + binData.second;
    block.first = (char*)calloc(1, block.second);
    memcpy(block.first, secret.data(), secret.size());
    free(binData.first);

    char* digest = Minos::Digest(block.first, block.second);
    free(block.first);

    bool valid = false;

    string sig = dict->stringValueForKey(kMinotaurModulePackageSignature);

#if !defined(__APPLE__)
    logger.debug("Module signature: %s", sig.c_str());
#else
// NSLog(@"Module signature: %s", sig.c_str());
#endif

    pair<char*, size_t> binSig = Minos::BinaryFromHexString(sig);
    if (binSig.first && binSig.second) {
      pair<char*, size_t> decryptedDigest = Minos::GetBack(binSig, keyData);

      if (decryptedDigest.first && decryptedDigest.second) {
        if (decryptedDigest.second == Minos::DigestSize()) {
          valid = Minos::LazyMatch(digest, decryptedDigest.first,
                                   Minos::DigestSize());
        }
        free(decryptedDigest.first);
      }

      free(binSig.first);
    }

    free(digest);

    if (!valid) {
      return 0;
    }

    // -- register the version info
    string mpver = dict->stringValueForKey(kMinotaurModulePackageVersion);

    if (mpId.length() && mpver.length()) {
      m_versionMap[mpId] = mpver;
    }

#if !defined(__APPLE__)
    logger.debug("Registering package id %s, version %s", mpId.c_str(),
                 mpver.c_str());
#else
// NSLog(@"Registering package id %s, version %s", mpId.c_str(), mpver.c_str());
#endif

    // ---
#if !defined(__APPLE__)
    logger.debug("Primary binary: %s", primaryBinaryPath.c_str());
    return internalLoadLibrary(primaryBinaryPath);
#else
    // NSLog(@"Primary binary: %s", primaryBinaryPath.c_str());
    return internalLoadLibrary(path);
#endif
  }

#if !defined(__APPLE__)
  virtual void* internalLoadLibrary(const string& primaryBinaryPath) {
    return (void*)LoadLibraryW(OVUTF16::FromUTF8(primaryBinaryPath).c_str());
  }

  virtual bool unloadLibrary(void* library) {
    return FreeLibrary((HMODULE)library) == TRUE;
  }

  virtual void* getFunctionNamed(void* library, const string& name) {
    return (void*)GetProcAddress((HMODULE)library, name.c_str());
  }
#else

#if defined(__APPLE__)
#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
#define NSMakeCollectable(x) ((id)(x))
#endif
#endif

  virtual void* internalLoadLibrary(const string& path) {
    CFStringRef pathString =
        CFStringCreateWithCString(NULL, path.c_str(), kCFStringEncodingUTF8);
    CFURLRef url = CFURLCreateWithFileSystemPath(NULL, pathString,
                                                 kCFURLPOSIXPathStyle, false);

    [NSMakeCollectable(pathString) autorelease];
    [NSMakeCollectable(url) autorelease];

    CFBundleRef bundle = CFBundleCreate(NULL, url);

    if (bundle) {
      if (CFBundleLoadExecutable(bundle)) {
        return (void*)bundle;
      } else {
        CFRelease(bundle);
        return 0;
      }
    }
  }

  virtual bool unloadLibrary(void* library) {
    if (!library) return false;

    CFBundleRef bundle = (CFBundleRef)library;
    CFBundleUnloadExecutable(bundle);
    CFRelease(bundle);
    return true;
  }

  virtual void* getFunctionNamed(void* library, const string& name) {
    if (!library) return 0;

    CFStringRef funcString =
        CFStringCreateWithCString(NULL, name.c_str(), kCFStringEncodingUTF8);
    [NSMakeCollectable(funcString) autorelease];
    CFBundleRef bundle = (CFBundleRef)library;

    return CFBundleGetFunctionPointerForName(bundle, funcString);
  }

#endif
};
};
#endif
