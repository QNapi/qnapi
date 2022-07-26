/*****************************************************************************
** QNapi
** Copyright (C) 2008-2017 Piotr Krzemiński <pio.krzeminski@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*****************************************************************************/

#ifndef PATHUTILS_H
#define PATHUTILS_H

#include "subtitlelanguage.h"

#include <QString>

class ChangeFilePath {
 public:
  /**
   * Add a given string before file extension and before language code.
   *
   * Add \a str to the complete base name of the file, excluding language code
   * if the file name has one.
   *
   * Examples (add 'X' string):
   *    a.b.c   -> a.bX.c
   *    a.eng.c -> aX.eng.c
   *    a       -> aX
   *
   * If this function is called multiple times on the same \c ChangeFilePath
   * object, only the last call will be taken into account.
   *
   * @param str The string to add.
   * @return *this
   */
  ChangeFilePath &addToBaseName(const QString &str) {
    addToBaseNameStr = str;
    return *this;
  }

  /**
   * Remove file extension.
   *
   * Examples:
   *    a.b.c -> a.b
   *    a.    -> a
   *    .a    ->
   *    a     -> a
   *
   * @return *this
   */
  ChangeFilePath &removeExtension() {
    doRemoveExtension = true;
    return *this;
  }

  /**
   * Add file extension.
   *
   * Add an extension to a file name. If the file name already has an extension,
   * it won't be removed (unless \c removeExtension is set).
   *
   * If this function is called multiple times on the same \c ChangeFilePath
   * object, only the last call will be taken into account.
   *
   * @param ext File extension to add.
   * @return *this
   */
  ChangeFilePath &addExtension(const QString &ext) {
    addExtensionStr = ext;
    return *this;
  }

  /**
   * Change file extension.
   *
   * Change file extension to a given string. If file has no extension, add one.
   * Same as \c removeExtension + \c addExtension.
   *
   * @param ext New file extension.
   * @return *this
   */
  ChangeFilePath &setExtension(const QString &ext) {
    return removeExtension().addExtension(ext);
  }

  /**
   * Remove language code from a file name.
   *
   * Remove language code from a file name if it has one.
   * Examples:
   *    a.b.c   -> a.b.c
   *    a.eng.c -> a.c
   *    eng.b.c -> eng.b.c
   *    a.eng   -> a.eng
   *
   * \c removeLanguageCode can be combined with \c addLanguageCode. The order
   * in which these two functions were called makes no diffrence,
   * \c removeLanguageCode will cause removal of the language code that was
   * present in original file name only.
   *
   * @return *this
   */
  ChangeFilePath &removeLanguageCode() {
    doRemoveLanguageCode = true;
    return *this;
  }

  /**
   * Add language code to a file name.
   *
   * Add \c lang language code to a file name. If the file name already
   * has a language code, it won't be removed (unless \c removeLanguageCode is
   * set). If \a type is \c LCT_NONE, nothing will be added.
   *
   * Examples (add 'fra' code):
   *    a.b.c   -> a.b.fra.c
   *    a.eng.c -> a.eng.fra.c
   *
   * Examples (add 'fra' code with \c removeLanguageCode also set):
   *    a.b.c   -> a.b.fra.c
   *    a.eng.c -> a.fra.c
   *
   * If this function is called multiple times on the same \c ChangeFilePath
   * object, only the last call will be taken into account.
   *
   * @param lang The language.
   * @param type Type of the code to add.
   * @return *this
   */
  ChangeFilePath &addLanguageCode(const SubtitleLanguage &lang,
                                  LangCodeType type) {
    addLanguageCodeLang = lang;
    addLanguageCodeType = type;
    return *this;
  }

  /**
   * Apply changes on a given string.
   * @param path File path to modify.
   * @return Modified file path.
   */
  QString apply(const QString &path) const;

 private:
  QString addToBaseNameStr;
  bool doRemoveExtension = false;
  QString addExtensionStr;
  bool doRemoveLanguageCode = false;
  SubtitleLanguage addLanguageCodeLang;
  LangCodeType addLanguageCodeType = LCT_NONE;
};

#endif // PATHUTILS_H

