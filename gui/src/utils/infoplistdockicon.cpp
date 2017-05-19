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

#include "infoplistdockicon.h"

#include <QCoreApplication>
#include <QDomDocument>
#include <QDomNode>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

bool InfoPlistDockIcon::readShowDockIcon() {
  const bool show_default = true;

  QString infoPlistPath =
      QFileInfo(QCoreApplication::applicationDirPath() + "/../Info.plist")
          .canonicalFilePath();

  QFile plistFile(infoPlistPath);

  QDomDocument doc;
  if (!doc.setContent(&plistFile)) return show_default;

  if (!doc.hasChildNodes()) return show_default;

  QDomNodeList nodes = doc.childNodes();

  QDomNode node;
  int i;
  for (i = 0; i < nodes.size(); ++i) {
    node = nodes.at(i);
    if (node.nodeName() == "plist") break;
  }

  if (i == nodes.size()) return show_default;

  if (!node.hasChildNodes()) return show_default;

  nodes = node.childNodes();

  for (i = 0; i < nodes.size(); ++i) {
    node = nodes.at(i);
    if (node.nodeName() == "dict") break;
  }

  if (i == nodes.size()) return show_default;

  if (!node.hasChildNodes()) return show_default;

  nodes = node.childNodes();

  for (i = 0; i < nodes.size(); ++i) {
    node = nodes.at(i);

    QString subText;

    if (node.hasChildNodes()) {
      subText = node.childNodes().at(0).toText().data();
    }

    if (subText == "LSUIElement") break;
  }

  if (i < nodes.size()) {
    node = node.nextSibling();
    return (node.nodeName() != "true");
  }

  return show_default;
}

void InfoPlistDockIcon::setShowDockIcon(bool show) {
  QString infoPlistPath =
      QFileInfo(QCoreApplication::applicationDirPath() + "/../Info.plist")
          .canonicalFilePath();

  QFile plistFile(infoPlistPath);

  QDomDocument doc;
  if (!doc.setContent(&plistFile) || !doc.hasChildNodes()) return;

  QDomNodeList nodes = doc.childNodes();

  QDomNode node;
  int i;
  for (i = 0; i < nodes.size(); ++i) {
    node = nodes.at(i);
    if (node.nodeName() == "plist") break;
  }

  if ((i == nodes.size()) || !node.hasChildNodes()) return;

  nodes = node.childNodes();

  for (i = 0; i < nodes.size(); ++i) {
    node = nodes.at(i);
    if (node.nodeName() == "dict") break;
  }

  if ((i == nodes.size()) || !node.hasChildNodes()) return;

  nodes = node.childNodes();

  for (i = 0; i < nodes.size(); ++i) {
    node = nodes.at(i);

    QString subText;

    if (node.hasChildNodes()) {
      subText = node.childNodes().at(0).toText().data();
    }

    if (subText == "LSUIElement") break;
  }

  if (i >= nodes.size()) return;

  node = node.nextSibling();

  node.toElement().setTagName(show ? "false" : "true");

  QString modifiedContent = doc.toString(4);

  plistFile.close();

  if (!plistFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) return;

  QTextStream plistStream(&plistFile);

  plistStream << modifiedContent;

  plistFile.close();
}
