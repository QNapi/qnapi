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

#ifndef SCANCONFIG_H
#define SCANCONFIG_H

#include <QString>
#include <QStringList>

class ScanConfig {
 private:
  QString lastDir_;
  bool skipIfSubtitlesExist_;
  QStringList filters_;
  QString skipFilters_;

 public:
  ScanConfig() {}
  ScanConfig(const QString& lastDir, const bool& skipIfSubtitlesExist,
             const QStringList& filters, const QString& skipFilters)
      : lastDir_(lastDir),
        skipIfSubtitlesExist_(skipIfSubtitlesExist),
        filters_(filters),
        skipFilters_(skipFilters) {}

  QString lastDir() const { return lastDir_; }
  bool skipIfSubtitlesExist() const { return skipIfSubtitlesExist_; }
  QStringList filters() const { return filters_; }
  QString skipFilters() const { return skipFilters_; }

  const ScanConfig setLastDir(const QString& lastDir) const {
    return ScanConfig(lastDir, skipIfSubtitlesExist_, filters_, skipFilters_);
  }
  const ScanConfig setSkipIfSubtitlesExist(
      const bool& skipIfSubtitlesExist) const {
    return ScanConfig(lastDir_, skipIfSubtitlesExist, filters_, skipFilters_);
  }
  const ScanConfig setFilters(const QStringList& filters) const {
    return ScanConfig(lastDir_, skipIfSubtitlesExist_, filters, skipFilters_);
  }
  const ScanConfig setSkipFilters(const QString& skipFilters) const {
    return ScanConfig(lastDir_, skipIfSubtitlesExist_, filters_, skipFilters);
  }

  QString toString() const;
};

#endif
