/*
Copyright (c) 2012, Yahoo! Inc.  All rights reserved.
Copyrights licensed under the New BSD License. See the accompanying LICENSE
file for terms.
*/

#include "YKAFOneKey.h"

#include <sstream>

using namespace OpenVanilla;

class OneKeyStringTable : public OVLocalizationStringTable {
 public:
  OneKeyStringTable() {
    "Query:";
    "Type ENTER to send; ESC to cancel";
    "OneKey Service Canceled";
    "OneKey Services";
    "No query input, canceled";

#ifndef _MSC_VER
    add("zh_TW", "One Key Services", "一點通");
    add("zh_CN", "One Key Services", "一点通");
    add("zh_TW", "Query:", "查詢字串");
    add("zh_CN", "Query:", "查询字串");
    add("zh_TW", "Type ENTER to send; ESC to cancel",
        "按下 ENTER 送出：按下ESC 取消。");
    add("zh_CN", "Type ENTER to send; ESC to cancel",
        "按下 ENTER 送出：按下ESC 取消。");
    add("zh_TW", "OneKey Service Canceled", "取消使用一點通服務");
    add("zh_CN", "OneKey Service Canceled", "取消使用一点通服务");
    add("zh_TW", "OneKey Services", "一點通服務");
    add("zh_CN", "OneKey Services", "一点通服务");
    add("zh_TW", "No query input, canceled", "沒有輸入內容，取消一點通查詢。");
    add("zh_CN", "No query input, canceled", "没有输入内容，取消一点通查询。");
#else
    add("zh_TW", "One Key Services", "\xe4\xb8\x80\xe9\xbb\x9e\xe9\x80\x9a");
    add("zh_CN", "One Key Services", "\xe4\xb8\x80\xe7\x82\xb9\xe9\x80\x9a");
    add("zh_TW", "Query:", "\xe6\x9f\xa5\xe8\xa9\xa2\xe5\xad\x97\xe4\xb8\xb2");
    add("zh_CN", "Query:", "\xe6\x9f\xa5\xe8\xaf\xa2\xe5\xad\x97\xe4\xb8\xb2");
    add("zh_TW", "Type ENTER to send; ESC to cancel",
        "\xe6\x8c\x89\xe4\xb8\x8b ENTER "
        "\xe9\x80\x81\xe5\x87\xba\xef\xbc\x9a\xe6\x8c\x89\xe4\xb8\x8b ESC "
        "\xe5\x8f\x96\xe6\xb6\x88\xe3\x80\x82");
    add("zh_CN", "Type ENTER to send; ESC to cancel",
        "\xe6\x8c\x89\xe4\xb8\x8b ENTER "
        "\xe9\x80\x81\xe5\x87\xba\xef\xbc\x9a\xe6\x8c\x89\xe4\xb8\x8b ESC "
        "\xe5\x8f\x96\xe6\xb6\x88\xe3\x80\x82");
    add("zh_TW", "OneKey Service Canceled",
        "\xe5\x8f\x96\xe6\xb6\x88\xe4\xbd\xbf\xe7\x94\xa8\xe4\xb8\x80\xe9\xbb"
        "\x9e\xe9\x80\x9a\xe6\x9c\x8d\xe5\x8b\x99\x22");
    add("zh_CN", "OneKey Service Canceled",
        "\xe5\x8f\x96\xe6\xb6\x88\xe4\xbd\xbf\xe7\x94\xa8\xe4\xb8\x80\xe7\x82"
        "\xb9\xe9\x80\x9a\xe6\x9c\x8d\xe5\x8a\xa1");
    add("zh_TW", "OneKey Services",
        "\xe4\xb8\x80\xe9\xbb\x9e\xe9\x80\x9a\xe6\x9c\x8d\xe5\x8b\x99");
    add("zh_CN", "OneKey Services",
        "\xe4\xb8\x80\xe7\x82\xb9\xe9\x80\x9a\xe6\x9c\x8d\xe5\x8a\xa1");
    add("zh_TW", "No query input, canceled",
        "\xe6\xb2\x92\xe6\x9c\x89\xe8\xbc\xb8\xe5\x85\xa5\xe5\x85\xa7\xe5\xae"
        "\xb9\xef\xbc\x8c\xe5\x8f\x96\xe6\xb6\x88\xe4\xb8\x80\xe9\xbb\x9e\xe9"
        "\x80\x9a\xe6\x9f\xa5\xe8\xa9\xa2\xe3\x80\x82");
    add("zh_CN", "No query input, canceled",
        "\xe6\xb2\xa1\xe6\x9c\x89\xe8\xbe\x93\xe5\x85\xa5\xe5\x86\x85\xe5\xae"
        "\xb9\xef\xbc\x8c\xe5\x8f\x96\xe6\xb6\x88\xe4\xb8\x80\xe7\x82\xb9\xe9"
        "\x80\x9a\xe6\x9f\xa5\xe8\xaf\xa2\xe3\x80\x82");
#endif
  }
};

typedef OVLocalization<OneKeyStringTable> OKL;

YKAFOneKeyContext::YKAFOneKeyContext(YKAFOneKey* module)
    : m_module(module), m_oneKeyData(0), m_loggingUserInput(false) {}

YKAFOneKeyContext::~YKAFOneKeyContext() {
  if (m_oneKeyData) {
    delete m_oneKeyData;
  }
}

void YKAFOneKeyContext::clear(OVLoaderService* loaderService) {
  m_loggingUserInput = false;
  m_invokingFeature = 0;
  m_loggingText.clear();
  m_loggingTextCursor = 0;

  loaderService->setPrompt("");
  loaderService->setPromptDescription("");
  loaderService->setLog("");
}

void YKAFOneKeyContext::startSession(OVLoaderService* loaderService) {
  if (m_oneKeyData) {
    delete m_oneKeyData;
    m_oneKeyData = 0;
  }

  void* data = loaderService->loaderSpecificDataObjectForName("OneKeyDataCopy");

  if (data) {
    m_oneKeyData = reinterpret_cast<PVPlistValue*>(data);
    // loaderService->logger("YKAFOneKeyContext") << "We have data!" << endl <<
    // *m_oneKeyData << endl;
  }

  clear(loaderService);
}

void YKAFOneKeyContext::stopSession(OVLoaderService* loaderService) {
  if (m_oneKeyData) {
    delete m_oneKeyData;
    m_oneKeyData = 0;
  }

  clear(loaderService);
}

bool YKAFOneKeyContext::handleDirectText(const vector<string>& segments,
                                         OVTextBuffer* readingText,
                                         OVTextBuffer* composingText,
                                         OVCandidateService* candidateService,
                                         OVLoaderService* loaderService) {
  if (!m_oneKeyData) {
    return false;
  }

  if (m_loggingUserInput && m_invokingFeature) {
    if (!m_invokingFeature->isKeyTrue("RecordsKeyStrokeOnly")) {
      m_loggingText += OVStringHelper::Join(segments);
      loaderService->setLog(m_loggingText);
    }

    return true;
  }

  return false;
}

bool YKAFOneKeyContext::handleKey(OVKey* key, OVTextBuffer* readingText,
                                  OVTextBuffer* composingText,
                                  OVCandidateService* candidateService,
                                  OVLoaderService* loaderService) {
  //	cerr << "handleKey " << key->keyCode() << ", logging? " <<
  // m_loggingUserInput << ", invoking feature? " << hex << (unsigned
  // int)m_invokingFeature  << dec << endl;

  if (!m_oneKeyData) {
    return false;
  }

  if (composingText->composedText().length()) {
    return false;
  }
  if (readingText->composedText().length()) {
    return false;
  }

  if ((key->keyCode() == OVKeyCode::Left ||
       key->keyCode() == OVKeyCode::Right) &&
      m_loggingUserInput) {
    if (composingText->isEmpty() && readingText->isEmpty()) {
      loaderService->beep();
      return true;
    }
  }

  if (key->keyCode() == m_module->m_cfgShortcutKey &&
      !key->isCombinedFunctionKey() && !m_loggingUserInput) {
    bool suppressed = (bool)loaderService->loaderSpecificDataObjectForName(
        "ServerSideUIDisabled");
    if (suppressed) {
      loaderService->beep();
      return true;
    }

    PVPlistValue* farray = m_oneKeyData->valueForKey("Features");
    if (!farray) {
      return true;
    }

    size_t fc = farray->arraySize();
    if (!fc) {
      return true;
    }

    // scan the keys first
    set<char> usedShortcutKeys;
    size_t vendorFeatures = 0;
    vector<pair<char, size_t> > collectedFeatures;

    for (size_t i = 0; i < fc; i++) {
      PVPlistValue* feature = farray->arrayElementAtIndex(i);
      if (feature->isKeyTrue("IsVendorFeature") && vendorFeatures < 10) {
        char key = (vendorFeatures != 9 ? ('1' + (char)vendorFeatures) : '0');
        usedShortcutKeys.insert(key);
        collectedFeatures.push_back(pair<char, size_t>(key, i));
        vendorFeatures++;
      }

      else {
        char key = 0;
        string sk = feature->stringValueForKey("ShortcutKey");
        if (sk.size()) {
          key = tolower(sk[0] & 0x7f);
          if (key < 'a' || key > 'z') {
            key = 0;
          }
        }

        if (key) {
          usedShortcutKeys.insert(key);
        }

        collectedFeatures.push_back(pair<char, size_t>(key, i));
      }
    }

    string unassignedKeys = "abcdefghijklmnopqrstuvwxyz";
    string::iterator uki = unassignedKeys.begin();
    for (vector<pair<char, size_t> >::iterator cfi = collectedFeatures.begin();
         cfi != collectedFeatures.end(); ++cfi) {
      if ((*cfi).first) {
        continue;
      }

      while (uki != unassignedKeys.end()) {
        if (usedShortcutKeys.find(*uki) == usedShortcutKeys.end()) {
          break;
        }

        uki++;
      }

      if (uki != unassignedKeys.end()) {
        (*cfi).first = *uki;
        uki++;
      }
    }

    OVOneDimensionalCandidatePanel* panel =
        candidateService->useVerticalCandidatePanel();
    OVCandidateList* list = panel->candidateList();
    list->clear();

    // now we collect the features
    string shortcutKey;
    for (vector<pair<char, size_t> >::iterator cfi = collectedFeatures.begin();
         cfi != collectedFeatures.end(); ++cfi) {
      if ((*cfi).first) {
        //				loaderService->logger("OneKey") << "key "
        //<<
        //(*cfi).first << ", feature: " << (*cfi).second << endl;
        shortcutKey += string(1, (*cfi).first);
        list->addCandidate(localizedFeatureTitleAtIndex((*cfi).second));
      }
    }

    composingText->clear();
    composingText->updateDisplay();
    readingText->clear();
    readingText->updateDisplay();
    loaderService->setLog("");

    panel->setCandidatesPerPage(shortcutKey.size());
    panel->setCandidateKeys(shortcutKey, loaderService);
    panel->updateDisplay();
    panel->show();
    panel->yieldToCandidateEventHandler();
    return true;
  }

  if (!m_loggingUserInput || !m_invokingFeature) {
    return false;
  }

  if ((key->keyCode() == m_module->m_cfgShortcutKey ||
       key->keyCode() == OVKeyCode::Esc) &&
      !key->isCombinedFunctionKey()) {
    loaderService->notify(OKL::S("OneKey Service Canceled"));
    composingText->clear();
    composingText->updateDisplay();
    readingText->clear();
    readingText->updateDisplay();
    clear(loaderService);
    return true;
  }

  if (key->keyCode() == OVKeyCode::Space) {
    if (m_loggingText.size()) {
      m_loggingText += " ";
      loaderService->setLog(m_loggingText);
      return true;
    } else {
      loaderService->beep();
      return true;
    }
  }

  if (key->keyCode() == OVKeyCode::Backspace) {
    vector<string> codepoints =
        OVUTF8Helper::SplitStringByCodePoint(m_loggingText);
    if (codepoints.size()) {
      codepoints.pop_back();
    } else {
      loaderService->beep();
      return true;
    }

    m_loggingText = OVStringHelper::Join(codepoints);
    loaderService->setLog(m_loggingText);
    return true;
  }

  if (key->keyCode() == OVKeyCode::Return ||
      key->keyCode() == OVKeyCode::Enter) {
    if (!m_loggingText.length()) {
      loaderService->notify(OKL::S("No query input, canceled"));
    } else {
      PVPlistValue* action = m_invokingFeature->valueForKey("Action");
      string actionString = action ? action->stringValue() : string();

      PVPlistValue* tracker = m_invokingFeature->valueForKey("Tracker");
      string trackerString = tracker ? tracker->stringValue() : string();

      if (trackerString.size()) {
        loaderService->callLoaderFeature("SendTrackerRequest", trackerString);
        //				loaderService->logger("OneKey") <<
        // trackerString << endl;
      }

      if (actionString == "OpenDictionary") {
        loaderService->lookupInDefaultDictionary(m_loggingText);
      } else if (actionString == "OpenURL") {
        // re-encode the string
        string reencodedString = m_loggingText;

        PVPlistValue* encoding =
            m_invokingFeature->valueForKey("PlaceHolderEncoding");
        if (encoding) {
          string encodingString = encoding->stringValue();
          if (!encodingString.length()) {
            encodingString = "UTF-8";
          }

          OVEncodingService* encodingService = loaderService->encodingService();
          if (encodingService) {
            pair<bool, string> result = encodingService->convertEncoding(
                "UTF-8", encodingString, m_loggingText);
            if (result.first) {
              reencodedString = result.second;
            }
          }
        }

        string URLString = localizedStringForKeyInPlist(
            m_invokingFeature->valueForKey("ActionURL"));

        PVPlistValue* placeholder =
            m_invokingFeature->valueForKey("InputPlaceHolder");
        if (placeholder) {
          string placeholderString = placeholder->stringValue();
          if (placeholderString.length()) {
            if (URLString.size()) {
              reencodedString = OVStringHelper::PercentEncode(reencodedString);
              URLString = OVStringHelper::
                  StringByReplacingOccurrencesOfStringWithString(
                      URLString, placeholderString, reencodedString);
            }
          }
        }

        if (URLString.length()) {
          loaderService->openURL(URLString);
        }
      } else if (actionString == "LaunchApp") {
        string placeHolder =
            m_invokingFeature->stringValueForKey("InputPlaceHolder");
        string appName = m_invokingFeature->stringValueForKey("LaunchAppName");
        string appArgs =
            m_invokingFeature->stringValueForKey("LaunchAppArguments");
        vector<string> aa;
        if (appName.length()) {
          aa.push_back(appName);
          if (appArgs.length()) {
            if (placeHolder.length()) {
              appArgs = OVStringHelper::
                  StringByReplacingOccurrencesOfStringWithString(
                      appArgs, placeHolder, m_loggingText);
            }
            aa.push_back(appArgs);
          }

          loaderService->callLoaderFeature("LaunchApp",
                                           OVStringHelper::Join(aa, "\t"));
        }
      }
    }

    readingText->clear();
    readingText->updateDisplay();
    composingText->clear();
    composingText->updateDisplay();
    clear(loaderService);
    return true;
  }

  if (m_invokingFeature->isKeyTrue("RecordsKeyStrokeOnly")) {
    if (key->isDirectTextKey() || key->isCapsLockOn()) {
      m_loggingText += string(1, toupper((char)key->keyCode()));
    } else {
      m_loggingText += string(1, (char)key->keyCode());
    }

    loaderService->setLog(m_loggingText);
    return true;
  }

  return false;
}

bool YKAFOneKeyContext::candidateSelected(OVCandidateService* candidateService,
                                          const string& text, size_t index,
                                          OVTextBuffer* readingText,
                                          OVTextBuffer* composingText,
                                          OVLoaderService* loaderService) {
  readingText->clear();
  readingText->updateDisplay();
  composingText->clear();
  composingText->updateDisplay();

  // see if we need to log
  PVPlistValue* farray = m_oneKeyData->valueForKey("Features");
  if (!farray) {
    return true;
  }

  m_invokingFeature = farray->arrayElementAtIndex(index);

  if (!m_invokingFeature) {
    return true;
  }

  if (!m_invokingFeature->isKeyTrue("RequiresInput")) {
    string action = m_invokingFeature->stringValueForKey("Action");
    if (action == "LaunchApp") {
      string appName = m_invokingFeature->stringValueForKey("LaunchAppName");
      string appArgs =
          m_invokingFeature->stringValueForKey("LaunchAppArguments");
      vector<string> aa;
      if (appName.length()) {
        aa.push_back(appName);
        if (appArgs.length()) {
          aa.push_back(appArgs);
        }

        loaderService->callLoaderFeature("LaunchApp",
                                         OVStringHelper::Join(aa, "\t"));
      }
    } else if (action == "OpenURL") {
      string url = m_invokingFeature->stringValueForKey("ActionURL");
      if (url.length()) {
        loaderService->openURL(url);
      }
    }

    // invoke the feature, and returns
    m_invokingFeature = 0;
    return true;
  }

  m_loggingUserInput = true;
  string lstr;

  lstr = localizedStringForKeyInPlist(
      m_invokingFeature->valueForKey("InputPrompt"), OKL::S("Query:"));
  lstr = OVStringHelper::StringByReplacingOccurrencesOfStringWithString(
      lstr, "\\n", "\n");
  loaderService->setPrompt(lstr);

  lstr = localizedStringForKeyInPlist(
      m_invokingFeature->valueForKey("InputPromptDescription"),
      OKL::S("Type ENTER to send; ESC to cancel"));
  lstr = OVStringHelper::StringByReplacingOccurrencesOfStringWithString(
      lstr, "\\n", "\n");
  loaderService->setPromptDescription(lstr);

  return true;
}

void YKAFOneKeyContext::candidateCanceled(OVCandidateService* candidateService,
                                          OVTextBuffer* readingText,
                                          OVTextBuffer* composingText,
                                          OVLoaderService* loaderService) {
  loaderService->notify(OKL::S("OneKey Service Canceled"));
  composingText->clear();
  composingText->updateDisplay();
  clear(loaderService);
}

bool YKAFOneKeyContext::candidateNonPanelKeyReceived(
    OVCandidateService* candidateService, const OVKey* key,
    OVTextBuffer* readingText, OVTextBuffer* composingText,
    OVLoaderService* loaderService) {
  if (key->keyCode() == m_module->m_cfgShortcutKey &&
      !key->isCombinedFunctionKey()) {
    // OVOneDimensionalCandidatePanel* panel =
    // candidateService->useOneDimensionalCandidatePanel();
    OVOneDimensionalCandidatePanel* panel =
        candidateService->useVerticalCandidatePanel();
    panel->hide();
    panel->cancelEventHandler();
    clear(loaderService);
    composingText->clear();
    composingText->updateDisplay();
    loaderService->notify(OKL::S("OneKey Service Canceled"));
    return true;
  }

  OVOneDimensionalCandidatePanel* panel =
      candidateService->useVerticalCandidatePanel();
  panel->updateDisplay();

  loaderService->beep();
  return true;
}

size_t YKAFOneKeyContext::featureCount() {
  PVPlistValue* farray = m_oneKeyData->valueForKey("Features");
  if (!farray) {
    return 0;
  }

  return farray->arraySize();
}

const string YKAFOneKeyContext::localizedFeatureTitleAtIndex(size_t index) {
  PVPlistValue* farray = m_oneKeyData->valueForKey("Features");
  if (!farray) {
    return string();
  }

  if (index >= farray->arraySize()) {
    return string();
  }

  PVPlistValue* feature = farray->arrayElementAtIndex(index);
  if (feature->type() != PVPlistValue::Dictionary) {
    return string();
  }

  PVPlistValue* name = feature->valueForKey("ServiceName");
  string defaultTitle = name ? name->stringValue() : string();

  return localizedStringForKeyInPlist(feature->valueForKey("Title"),
                                      defaultTitle);
}

const string YKAFOneKeyContext::localizedStringForKeyInPlist(
    PVPlistValue* dict, const string& defaultValue) {
  if (!dict) {
    return defaultValue;
  }

  if (dict->type() == PVPlistValue::String) {
    return dict->stringValue();
  }

  PVPlistValue* value = dict->valueForKey(m_module->m_locale);
  if (!value) {
    value = dict->valueForKey("en");
  }

  if (!value) {
    vector<string> keys = dict->dictionaryKeys();
    if (keys.size()) {
      value = dict->valueForKey(keys[0]);
    }
  }

  if (!value) {
    return defaultValue;
  }

  if (value->type() != PVPlistValue::String) {
    return defaultValue;
  }

  return value->stringValue();
}

const string YKAFOneKey::localizedName(const string& locale) {
  return OKL::S(locale, "OneKey Services");
}

OVEventHandlingContext* YKAFOneKey::createContext() {
  return new YKAFOneKeyContext(this);
}

bool YKAFOneKey::initialize(OVPathInfo* pathInfo,
                            OVLoaderService* loaderService) {
  m_cfgShortcutKey = '`';
  m_locale = loaderService->locale();
  OKL::SetDefaultLocale(m_locale);

  return true;
}

const string YKAFOneKey::identifier() const { return "OneKey"; }

int YKAFOneKey::suggestedOrder() const { return -10000; }

void YKAFOneKey::loadConfig(OVKeyValueMap* moduleConfig,
                            OVLoaderService* loaderService) {
  if (moduleConfig->hasKey("ShortcutKey")) {
    string v = moduleConfig->stringValueForKey("ShortcutKey");
    if (v.size()) m_cfgShortcutKey = v[0];
  }

  m_locale = loaderService->locale();
}

void YKAFOneKey::saveConfig(OVKeyValueMap* moduleConfig,
                            OVLoaderService* loaderService) {
  moduleConfig->setKeyStringValue("ShortcutKey", string(1, m_cfgShortcutKey));
}
