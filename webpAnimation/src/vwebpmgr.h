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

	// ����mp4����webp��ʽ���������֡���Ҵ����ص�
	void loadAlphaMp4(const QString &url, const QString& saveFolder, bool bDecode, const VideoMovieUtils::FnCallback callBack);

	// webp�������
	QMap<QString, QSharedPointer<VideoMovieUtils::SpriteSheetVo>> m_mapWebp;
	QMutex m_mutexMapWebp;

private:
	QNetworkAccessManager m_networkManager;

private:
	WebpMgr(QObject *parent = Q_NULLPTR);
	Q_DISABLE_COPY(WebpMgr)
};

#define webpMgr WebpMgr::getInstance()
