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

#include "subtitleformat.h"
#include <QMap>

QVector<SubToken> SubtitleFormat::decodeTokenStream(QString tokenStream) const {
  QVector<SubToken> tokens;
  SubToken tok;

  QMap<QString, SubTokenType> tokenTypes = {
      {"{y:b}", STT_BOLD},         {"{b}", STT_BOLD},
      {"<b>", STT_BOLD},           {"{y:i}", STT_ITALIC},
      {"{i}", STT_ITALIC},         {"<i>", STT_ITALIC},
      {"{y:u}", STT_UNDERLINE},    {"{u}", STT_UNDERLINE},
      {"<u>", STT_UNDERLINE},      {"{/y:b}", STT_BOLD_END},
      {"{/b}", STT_BOLD_END},      {"</b>", STT_BOLD_END},
      {"{/y:i}", STT_ITALIC_END},  {"{/i}", STT_ITALIC_END},
      {"</i>", STT_ITALIC_END},    {"{/y:u}", STT_UNDERLINE_END},
      {"{/u}", STT_UNDERLINE_END}, {"</u>", STT_UNDERLINE_END},
      {"{/c}", STT_FONTCOLOR_END}, {"</font>", STT_FONTCOLOR_END},
      {"|", STT_NEWLINE},          {"\r\n", STT_NEWLINE},
      {"\n", STT_NEWLINE}};

  QString wordBuff;

  while (!tokenStream.isEmpty()) {
    tok.payload.clear();

    bool matched = false;

    foreach (QString token, tokenTypes.keys()) {
      if (tokenStream.startsWith(token, Qt::CaseInsensitive)) {
        tok.type = tokenTypes[token];
        tokenStream.remove(0, token.size());
        matched = true;
        break;
      }
    }

    if (!matched) {
      QRegExp colorR1("^\\{c:(.*)\\}");
      colorR1.setPatternSyntax(QRegExp::RegExp2);
      colorR1.setCaseSensitivity(Qt::CaseInsensitive);

      QRegExp colorR2("^<font color=(.*)>");
      colorR1.setPatternSyntax(QRegExp::RegExp2);
      colorR1.setCaseSensitivity(Qt::CaseInsensitive);

      if (colorR1.indexIn(tokenStream) == 0) {
        tok.type = STT_FONTCOLOR;
        tok.payload = parseColor(colorR1.cap(1));
        tokenStream.remove(0, colorR1.cap(0).size());
        matched = true;
      } else if (colorR2.indexIn(tokenStream) == 0) {
        tok.type = STT_FONTCOLOR;
        tok.payload = parseColor(colorR2.cap(1));
        tokenStream.remove(0, colorR2.cap(0).size());
        matched = true;
      } else if (tokenStream[0] == '/' && wordBuff.isEmpty()) {
        tok.type = STT_ITALIC;
        tokenStream.remove(0, 1);
        matched = true;
      } else if (tokenStream[0].isSpace()) {
        while (!tokenStream.isEmpty() && tokenStream[0].isSpace())
          tokenStream.remove(0, 1);
        tok.type = STT_WS;
        matched = true;
      } else {
        wordBuff += tokenStream[0];
        tokenStream.remove(0, 1);
      }
    }

    if (matched) {
      if (!wordBuff.isEmpty()) {
        SubToken wordTok;
        wordTok.type = STT_WORD;
        wordTok.payload = wordBuff;
        tokens.push_back(wordTok);
        wordBuff.clear();
      }
      tokens.push_back(tok);
    }
  }

  if (!wordBuff.isEmpty()) {
    SubToken wordTok;
    wordTok.type = STT_WORD;
    wordTok.payload = wordBuff;
    tokens.push_back(wordTok);
  }

  if (!tokens.isEmpty() && tokens[0].type == STT_WS) {
    while (tokens[0].type == STT_WS) tokens.pop_front();
  }

  return tokens;
}

QString SubtitleFormat::parseColor(QString colorString) const {
  return colorString.replace('"', "").replace('#', "").replace('$', "");
}
