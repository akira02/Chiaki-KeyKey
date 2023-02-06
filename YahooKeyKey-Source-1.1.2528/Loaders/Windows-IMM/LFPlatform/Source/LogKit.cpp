//
// LogKit.cpp
//
// Copyright (c) 2007-2010 Lithoglyph Inc. All rights reserved.
//

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "LogKit.h"

#include <OpenVanilla.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include <sstream>

namespace LFPlatform {

using namespace OpenVanilla;

class WindowsLogger {
 public:
  static const size_t StringBufferLength = 1024;

  WindowsLogger()
      : m_indentLevel(0),
        m_logFile(0),
        m_consoleMode(false),
        m_consoleInitialized(false)
  // , m_displaysMillisecond(false)
  {
    m_stringBuffer = new char[StringBufferLength];
  }

  ~WindowsLogger() {
    flush();

    if (m_stringBuffer) delete[] m_stringBuffer;
  }

  void flush() {
    if (m_logFile) {
      fclose(m_logFile);
      m_logFile = 0;
    }
  }

  int indentLevel() { return m_indentLevel; }

  void useLogFile(const string& filename) {
    flush();
    m_logFilename = filename;
  }

  void setIndentLevel(int indentLevel) {
    m_indentLevel = indentLevel < 0 ? 0 : indentLevel;
  }

  bool consoleEnabled() { return m_consoleMode; }

  void useConsole(bool mode) { m_consoleMode = mode; }

  // bool displaysMillisecond()
  // {
  //     return m_displaysMillisecond;
  // }
  //
  // void setDisplaysMillisecond(bool display)
  // {
  //     m_displaysMillisecond = display;
  // }

  void write(const string& section, const char* level, const char* format,
             va_list ap) {
    memset(m_stringBuffer, 0, StringBufferLength);
    vsnprintf_s(m_stringBuffer, StringBufferLength, _TRUNCATE, format, ap);

    stringstream sst;
    sst << OVDateTimeHelper::LocalDateTimeString();

    sst << " - ";

    if (m_indentLevel > 1) sst << string(m_indentLevel - 1, ' ');

    if (section.size()) sst << "(" << section << ") ";

    if (level) sst << level << ": ";

    sst << m_stringBuffer << endl;

    openLogFile();
    if (m_logFile) {
      fprintf_s(m_logFile, "%s", sst.str().c_str());
    }

    wstring ws = OVUTF16::FromUTF8(sst.str());

    if (m_consoleMode) {
      if (!m_consoleInitialized) {
        m_consoleInitialized = true;
        AllocConsole();
      }

      WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), ws.c_str(),
                   (DWORD)ws.length(), 0, 0);
    }

#ifndef NDEBUG
    OutputDebugString(ws.c_str());
#endif
  }

  static WindowsLogger* SharedLogger;

 protected:
  void openLogFile() {
    if (!m_logFile) {
      if (m_logFilename.size()) {
        m_logFile = OVFileHelper::OpenStream(m_logFilename, "a");
      }
    }
  }

  int m_indentLevel;
  FILE* m_logFile;
  string m_logFilename;
  bool m_consoleMode;
  char* m_stringBuffer;
  bool m_consoleInitialized;
  // bool m_displaysMillisecond;
};

WindowsLogger* WindowsLogger::SharedLogger = 0;

void Logger::PrepareLogger() {
  if (!WindowsLogger::SharedLogger)
    WindowsLogger::SharedLogger = new WindowsLogger;
}

void Logger::CloseLogger() {
  if (WindowsLogger::SharedLogger) {
    delete WindowsLogger::SharedLogger;
    WindowsLogger::SharedLogger = 0;
  }
}

void Logger::FlushLogger() { WindowsLogger::SharedLogger->flush(); }

int Logger::IndentLevel() { return WindowsLogger::SharedLogger->indentLevel(); }

void Logger::UseLogFile(const string& filename) {
  PrepareLogger();
  WindowsLogger::SharedLogger->useLogFile(filename);
}

void Logger::SetIndentLevel(int indentLevel) {
  WindowsLogger::SharedLogger->setIndentLevel(indentLevel);
}

bool Logger::ConsoleEnabled() {
  return WindowsLogger::SharedLogger->consoleEnabled();
}

void Logger::UseConsole(bool mode) {
  PrepareLogger();
  WindowsLogger::SharedLogger->useConsole(mode);
}

// bool Logger::DisplaysMillisecond()
// {
//     return WindowsLogger::SharedLogger->displaysMillisecond();
// }
//
// void Logger::SetDisplaysMillisecond(bool display)
// {
//     WindowsLogger::SharedLogger->setDisplaysMillisecond(display);
// }

void Logger::Write(const string& section, const char* level, const char* format,
                   va_list ap) {
  WindowsLogger::SharedLogger->write(section, level, format, ap);
}

};  // namespace LFPlatform
