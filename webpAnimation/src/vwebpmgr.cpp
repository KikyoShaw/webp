#include "vwebpmgr.h"
#include <QNetworkRequest>
#include <QNetworkReply>
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
static QMutex s_mp4_mutex;
static void doMp4CallbackQueue(const QString strFilePath, VideoMovieUtils::SpriteSheetVo* pvo) {
	QMutexLocker locker(&s_mp4_mutex);
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

void WebpMgr::loadAlphaMp4(const QString & url, const QString & saveFolder, bool bDecode, const VideoMovieUtils::FnCallback callBack)
{
	const QDir mp4Dir(saveFolder);
	if (!mp4Dir.exists()) {
		mp4Dir.mkpath("");
	}

	const QString hashId = QCryptographicHash::hash(url.toStdString().c_str(), QCryptographicHash::Md5).toHex().toUpper();

	// webp格式的后缀名
	QString strSuffix;
	if (url.contains(".webp", Qt::CaseInsensitive))
		strSuffix = ".webp";
	else
		strSuffix = ".mp4";

	const QString strFilePath = mp4Dir.absoluteFilePath(hashId) + strSuffix;
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

		// 复用解析结果
		if (m_mapWebp.contains(strFilePath))
		{
			auto data = m_mapWebp.value(strFilePath);
			// 回调序列帧数据结构需要重新分配内存
			callBack(data->clone());
			return;
		}
	}

	//等待队列
	{
		QMutexLocker locker(&s_mp4_mutex);
		auto* list = s_mp4_map[strFilePath];
		if (nullptr == list) {
			list = new std::vector<VideoMovieUtils::FnCallback>();
			s_mp4_map[strFilePath] = list;
		}

		list->push_back(callBack);
	}


	//开始执行下载流程
	QNetworkRequest request(url);
	auto reply = m_networkManager.get(request);

	qInfo() << "start download mp4" << url << strFilePath;
	//保存图片到本地
	connect(reply, &QNetworkReply::finished, this, [=] {
		auto data = reply->readAll();
		reply->deleteLater();
		if (data.isEmpty()) {
			doMp4CallbackQueue(strFilePath, nullptr);
			return;
		}
		//auto fileName = QString("%1%2%3").arg(path).arg(QDir::separator()).arg(name);
		QFile file(strFilePath);
		file.open(QIODevice::WriteOnly);
		file.write(data);
		file.close();

		if (url.contains(".webp", Qt::CaseInsensitive))
		{
			qInfo() << "end download webp" << url << strFilePath;
			VWebp::convertAlphaWebpToPngs(this, strFilePath, strSaveFolder, bDecode, [=](VideoMovieUtils::SpriteSheetVo* pvo) {
				doMp4CallbackQueue(strFilePath, pvo);
			});
		}
		else
		{
			qInfo() << "end download mp4" << url << strFilePath;
			VideoMovieUtils::convertAlphaMp4ToPngs(this, strFilePath, strSaveFolder, bDecode, [=](VideoMovieUtils::SpriteSheetVo* pvo) {
				doMp4CallbackQueue(strFilePath, pvo);
			});
		}
	});
}
