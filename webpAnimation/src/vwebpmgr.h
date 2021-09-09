#pragma once

#include <QObject>
#include "VideoMovieUtils.h"
#include <QNetworkAccessManager>
#include <QMutex>

class QNetworkAccessManager;

class WebpMgr : public QObject
{
	Q_OBJECT
public:
	static WebpMgr& getInstance();
	~WebpMgr();

	// 加载mp4或者webp格式，获得序列帧并且触发回调
	void loadAlphaMp4(const QString &url, const QString& saveFolder, bool bDecode, const VideoMovieUtils::FnCallback callBack);

	// webp解析结果
	QMap<QString, QSharedPointer<VideoMovieUtils::SpriteSheetVo>> m_mapWebp;
	QMutex m_mutexMapWebp;

private:
	QNetworkAccessManager m_networkManager;

private:
	WebpMgr(QObject *parent = Q_NULLPTR);
	Q_DISABLE_COPY(WebpMgr)
};

#define webpMgr WebpMgr::getInstance()
