//
// LogKit.h
//
// Copyright (c) 2007-2010 Lithoglyph Inc. All rights reserved.
//

#ifndef LogKit_h
#define LogKit_h

#include <stdarg.h>

#include <string>

namespace LFPlatform {
using namespace std;

#ifdef WIN32
#ifndef NDEBUG
#define ENABLE_LOGGER_LEVEL_ERROR
#define ENABLE_LOGGER_LEVEL_DEBUG
#else
// #define ENABLE_LOGGER_LEVEL_DEBUG
#define ENABLE_LOGGER_LEVEL_ERROR
#endif
#endif

class Logger {
 protected:
  static void Write(const string& section, const char* level,
                    const char* format, va_list ap);

 public:
  static void PrepareLogger();
  static void CloseLogger();
  static void FlushLogger();
  static int IndentLevel();
  static void UseLogFile(const string& filename);
  static void SetIndentLevel(int indentLevel);
  static bool ConsoleEnabled();
  static void UseConsole(bool mode);
  // static bool DisplaysMillisecond();
  // static void SetDisplaysMillisecond(bool display);

 public:
  Logger(const string& section = "", int indentIncrement = 1);
  ~Logger();

  virtual void error(const char* format, ...) const;
  virtual void debug(const char* format, ...) const;
  virtual void nop(const char* format, ...) const;

 protected:
  string m_section;
  int m_indentLevel;
  int m_indentIncrement;
};

inline Logger::Logger(const string& section, int indentIncrement)
    : m_section(section), m_indentIncrement(indentIncrement) {
  Logger::PrepareLogger();

  m_indentLevel = IndentLevel() + indentIncrement;
  Logger::SetIndentLevel(m_indentLevel);
}

inline Logger::~Logger() {
  Logger::SetIndentLevel(IndentLevel() - m_indentIncrement);
  Logger::FlushLogger();
}

inline void Logger::error(const char* format, ...) const {
#ifdef ENABLE_LOGGER_LEVEL_ERROR
  va_list ap;
  va_start(ap, format);
  Logger::SetIndentLevel(m_indentLevel);
  Logger::Write(m_section, "ERROR", format, ap);
  va_end(ap);
#endif
}

inline void Logger::debug(const char* format, ...) const {
#ifdef ENABLE_LOGGER_LEVEL_DEBUG
  va_list ap;
  va_start(ap, format);
  Logger::SetIndentLevel(m_indentLevel);
  Logger::Write(m_section, 0, format, ap);
  va_end(ap);
#endif
}

inline void Logger::nop(const char*, ...) const {}
};  // namespace LFPlatform

#endif