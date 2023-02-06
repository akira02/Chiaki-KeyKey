/*
Copyright (c) 2012, Yahoo! Inc.  All rights reserved.
Copyrights licensed under the New BSD License. See the accompanying LICENSE
file for terms.
*/

#ifndef YKAFOneKeySearch_h
#define YKAFOneKeySearch_h

#if defined(__APPLE__)
#include <OpenVanilla/OpenVanilla.h>
#include <PlainVanilla/PlainVanilla.h>
#else
#include "OpenVanilla.h"
#include "PlainVanilla.h"
#endif

namespace OpenVanilla {
using namespace std;

class YKAFOneKey;

class YKAFOneKeyContext : public OVEventHandlingContext {
 public:
  YKAFOneKeyContext(YKAFOneKey* module);
  ~YKAFOneKeyContext();

  virtual bool handleDirectText(const vector<string>& segments,
                                OVTextBuffer* readingText,
                                OVTextBuffer* composingText,
                                OVCandidateService* candidateService,
                                OVLoaderService* loaderService);
  virtual bool handleKey(OVKey* key, OVTextBuffer* readingText,
                         OVTextBuffer* composingText,
                         OVCandidateService* candidateService,
                         OVLoaderService* loaderService);

  virtual void clear(OVLoaderService* loaderService);
  virtual void startSession(OVLoaderService* loaderService);
  virtual void stopSession(OVLoaderService* loaderService);

  virtual bool candidateSelected(OVCandidateService* candidateService,
                                 const string& text, size_t index,
                                 OVTextBuffer* readingText,
                                 OVTextBuffer* composingText,
                                 OVLoaderService* loaderService);
  virtual void candidateCanceled(OVCandidateService* candidateService,
                                 OVTextBuffer* readingText,
                                 OVTextBuffer* composingText,
                                 OVLoaderService* loaderService);
  virtual bool candidateNonPanelKeyReceived(
      OVCandidateService* candidateService, const OVKey* key,
      OVTextBuffer* readingText, OVTextBuffer* composingText,
      OVLoaderService* loaderService);

 protected:
  YKAFOneKey* m_module;
  PVPlistValue* m_oneKeyData;

  bool m_loggingUserInput;
  PVPlistValue* m_invokingFeature;
  string m_loggingText;
  size_t m_loggingTextCursor;

  size_t featureCount();
  const string localizedFeatureTitleAtIndex(size_t index);

  const string localizedStringForKeyInPlist(PVPlistValue* dict,
                                            const string& defaultValue = "");
};

class YKAFOneKey : public OVAroundFilter {
 public:
  virtual const string localizedName(const string& locale);
  virtual OVEventHandlingContext* createContext();
  virtual bool initialize(OVPathInfo* pathInfo, OVLoaderService* loaderService);
  virtual const string identifier() const;
  virtual int suggestedOrder() const;

  virtual void loadConfig(OVKeyValueMap* moduleConfig,
                          OVLoaderService* loaderService);
  virtual void saveConfig(OVKeyValueMap* moduleConfig,
                          OVLoaderService* loaderService);

 protected:
  friend class YKAFOneKeyContext;
  char m_cfgShortcutKey;
  string m_locale;
};

};  // namespace OpenVanilla

#endif
