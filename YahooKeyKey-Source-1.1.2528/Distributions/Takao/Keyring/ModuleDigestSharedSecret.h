// [AUTO_HEADER]

#ifndef ModuleDigestSharedSecret_h
#define ModuleDigestSharedSecret_h

#define TAKAO_MODULE_DIGEST_SHARED_SECRET ".ykmod"

#if defined(__APPLE__)
#define kMinotaurModulePackagePlistName "Info.plist"
#define kMinotaurModulePackageIdentifier "CFBundleIdentifier"
#define kMinotaurModulePackageSignature "MPSignature"
#define kMinotaurModulePackagePrimaryBinary "CFBundleExecutable"
#define kMinotaurModulePackageVersion "CFBundleVersion"
#else
#define kMinotaurModulePackagePlistName "ModulePackage.plist"
#define kMinotaurModulePackageIdentifier "MPIdentifier"
#define kMinotaurModulePackageSignature "MPSignature"
#define kMinotaurModulePackagePrimaryBinary "MPPrimaryBinary"
#define kMinotaurModulePackageVersion "MPVersion"
#endif

#ifdef WIN32
#define TAKAO_VENDOR_MODULE_COMPONENT_NAME "Takao-ModulePackage-Windows"
#else
#if defined(__APPLE__)
#define TAKAO_VENDOR_MODULE_COMPONENT_NAME "Takao-ModulePackage-OSX"
#endif
#endif

#endif
