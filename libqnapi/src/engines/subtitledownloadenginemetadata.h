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

#ifndef SUBTITLEDOWNLOADENGINEMETADATA_H
#define SUBTITLEDOWNLOADENGINEMETADATA_H

#include <Maybe.h>

#include <QString>

class SubtitleDownloadEngineMetadata {
 private:
  QString name_;
  QString description_;
  Maybe<QUrl> registrationUrl_;
  Maybe<QUrl> uploadUrl_;

 public:
  SubtitleDownloadEngineMetadata(const QString& name,
                                 const QString& description,
                                 const Maybe<QUrl>& registrationUrl,
                                 const Maybe<QUrl>& uploadUrl)
      : name_(name),
        description_(description),
        registrationUrl_(registrationUrl),
        uploadUrl_(uploadUrl) {}

  QString name() const { return name_; }
  QString description() const { return description_; }
  Maybe<QUrl> registrationUrl() const { return registrationUrl_; }
  Maybe<QUrl> uploadUrl() const { return uploadUrl_; }
};

#endif
