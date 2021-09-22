#include "vwebpmgr.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkConfiguration>
#include "vwebp.h"

WebpMgr & WebpMgr::getInstance()
{
	static WebpMgr m_instance;
	return m_instance;
}

WebpMgr::WebpMgr(QObject *parent)
	: QObject(parent)
{	
}

WebpMgr::~WebpMgr()
{
}

static std::map<QString, std::vector<VideoMovieUtils::FnCallback>* > s_mp4_map;
static QMutex s_webp_mutex;
static void doWebpCallbackQueue(const QString strFilePath, VideoMovieUtils::SpriteSheetVo* pvo) {
	QMutexLocker locker(&s_webp_mutex);
	auto* list = s_mp4_map[strFilePath];
	s_mp4_map.erase(strFilePath);
	if (list == Q_NULLPTR) {
		return;
	}
	bool bFirst = true;
	for (auto it = list->begin(); it != list->end(); ++it) {
		if (nullptr != pvo) {
			if (bFirst) {
				bFirst = false;
			}
			else {
				pvo = pvo->clone();
			}
		}

		(*it)(pvo);
	}
	delete list;
}

void WebpMgr::loadAlphaWebp(const QString & url, const QString & saveFolder, bool bDecode, const VideoMovieUtils::FnCallback callBack)
{
	const QDir webpDir(saveFolder);
	if (!webpDir.exists()) {
		webpDir.mkpath("");
	}
	//��ȡ���ӹ�ϣֵ
	const QString hashId = QCryptographicHash::hash(url.toStdString().c_str(), QCryptographicHash::Md5).toHex().toUpper();
	// webp��ʽ�ĺ�׺��
	const QString strFilePath = webpDir.absoluteFilePath(hashId) + ".webp";
	const QString strSaveFolder = VideoMovieUtils::getSaveFolderFromFilePath(strFilePath);
	{
		VideoMovieUtils::SpriteSheetVo* ret = nullptr;
		if (VideoMovieUtils::getSpriteSheet(strSaveFolder, ret, bDecode)) {
			callBack(ret);
			return;
		}
	}
	{
		QMutexLocker lock(&m_mutexMapWebp);

		// ���ý������
		if (m_mapWebp.contains(strFilePath))
		{
			auto data = m_mapWebp.value(strFilePath);
			// �ص�����֡���ݽṹ��Ҫ���·����ڴ�
			callBack(data->clone());
			return;
		}
	}

	//�ȴ�����
	{
		QMutexLocker locker(&s_webp_mutex);
		auto* list = s_mp4_map[strFilePath];
		if (nullptr == list) {
			list = new std::vector<VideoMovieUtils::FnCallback>();
			s_mp4_map[strFilePath] = list;
		}

		list->push_back(callBack);
	}

	//����ļ�����ֱ�Ӳ���
	if (QFile::exists(strFilePath)) {
		qInfo() << "end download webp" << url << strFilePath;
		VWebp::convertAlphaWebpToPngs(this, strFilePath, strSaveFolder, bDecode, [=](VideoMovieUtils::SpriteSheetVo* pvo) {
			doWebpCallbackQueue(strFilePath, pvo);
		});
		return;
	}

	//��ʼִ����������
	QNetworkRequest request;
	request.setUrl(QUrl(url));
	request.setHeader(QNetworkRequest::UserAgentHeader, "RT-Thread ART");
	m_networkManager.activeConfiguration();
	auto reply = m_networkManager.get(request);
	//����ͼƬ������
	connect(reply, &QNetworkReply::finished, this, [=] {
		auto data = reply->readAll();
		reply->deleteLater();
		if (data.isEmpty()) {
			doWebpCallbackQueue(strFilePath, nullptr);
			return;
		}
		//auto fileName = QString("%1%2%3").arg(path).arg(QDir::separator()).arg(name);
		QFile file(strFilePath);
		file.open(QIODevice::WriteOnly);
		file.write(data);
		file.close();

		qInfo() << "end download webp" << url << strFilePath;
		VWebp::convertAlphaWebpToPngs(this, strFilePath, strSaveFolder, bDecode, [=](VideoMovieUtils::SpriteSheetVo* pvo) {
			doWebpCallbackQueue(strFilePath, pvo);
		});
	});
}
