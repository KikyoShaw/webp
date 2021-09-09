#include "vwebp.h"
#include "webp/demux.h"
#include "vwebpmgr.h"
#include <QFile>
#include <QImage>
#include <QDebug>

// ����libwebp-1.2.1
// x86 Native Tools Command Prompt for VS 2017
// nmake /f Makefile.vc CFG=debug-dynamic RTLIBCFG=dynamic ARCH=x86
// nmake /f Makefile.vc CFG=release-dynamic RTLIBCFG=dynamic ARCH=x86
// libwebpdemux_debug_dll.lib��libwebpdemux_debug.dll��libwebp_debug.dll
// libwebpdemux_dll.lib��libwebpdemux.dll��libwebp.dll

VWebp::VWebp(QObject* parent/* = Q_NULLPTR*/)
	: QThread(parent)
{
}

VWebp::~VWebp()
{
}

// ��ʼ����webp�ļ�·��
void VWebp::initWebpParse(QString strFile)
{
	m_strFile = strFile;
}

// ��ʼwebp�ļ�����
bool VWebp::startWebpParse()
{
	if (m_strFile.isEmpty())
	{
		qInfo() << "webp file empty";
		return false;
	}

	// ���������߳�
	if (isRunning())
	{
		qInfo() << "webp running" << m_strFile;
		return true;
	}

	qInfo() << "webp start thread" << m_strFile;

	// �����߳�
	start();
	return true;
}

void VWebp::run()
{
	qInfo() << "webp run start" << m_strFile;

	// ��ȡwebp�ļ�
	QFile file(m_strFile);
	bool bOpen = file.open(QIODevice::ReadOnly);
	if (!bOpen)
	{
		qInfo() << "webp file open fail" << m_strFile;
		return;
	}
	QByteArray baFile = file.readAll();
	file.close();
	if (baFile.isEmpty())
	{
		qInfo() << "webp file read empty" << m_strFile;
		return;
	}

	// ��ʼ��ʹ��webp�ļ�����
	WebPData webp_data = {0};
	WebPDataInit(&webp_data);
	webp_data.bytes = (const uint8_t*)baFile.constData();
	webp_data.size = baFile.size();

	// ����webp��ͼ������
	WebPAnimDecoderOptions dec_options = {0};
	WebPAnimDecoderOptionsInit(&dec_options);
	dec_options.color_mode = MODE_RGBA;
	WebPAnimDecoder* dec = WebPAnimDecoderNew(&webp_data, &dec_options);
	if (!dec)
	{
		qInfo() << "WebPAnimDecoderNew fail" << m_strFile;
		return;
	}

	// ����webp��ͼ��Ϣ
	WebPAnimInfo anim_info = {0};
	int ret = WebPAnimDecoderGetInfo(dec, &anim_info);
	if (!ret)
	{
		qInfo() << "WebPAnimDecoderGetInfo fail" << ret << m_strFile;

		// �ͷ�
		WebPAnimDecoderDelete(dec);
		return;
	}

	m_width = anim_info.canvas_width;
	m_height = anim_info.canvas_height;
	int sizeFrame = anim_info.canvas_width * anim_info.canvas_height * 4; // ÿ֡���ݴ�С

	// ��������webp��ͼ
	while (WebPAnimDecoderHasMoreFrames(dec)) // ������һ֡
	{
		// ������һ֡
		uint8_t* buf = NULL;
		int timestamp = 0;
		ret = WebPAnimDecoderGetNext(dec, &buf, &timestamp);
		if (!ret)
		{
			break;
		}

		// ����֡����
		stWebpFrame webpFrame;
		webpFrame.baFrame = QByteArray((const char*)buf, sizeFrame); // ���
		webpFrame.msFrame = timestamp;
		m_listWebpFrame.push_back(webpFrame);

		// ���Ա���ÿ֡�ļ�
		//QImage image((const uchar*)webpFrame.baFrame.constData(), anim_info.canvas_width, anim_info.canvas_height, QImage::Format_RGBA8888);
		//QString fileName = QString("d:/testout/%1_%2.png").arg(m_listWebpFrame.size()).arg(webpFrame.msFrame);
		//image.save(fileName, "PNG");
	} // while

	// �ͷ�
	WebPAnimDecoderDelete(dec);
	dec = NULL;

	// ����webp�������
	doWebpParseResult();

	// ֪ͨwebp�������
	emit sigWebpParseResult();
}

// ��webpת��png
void VWebp::convertAlphaWebpToPngs(QObject* context, const QString& strFilePath, const QString& strSaveFolder, bool bDecode, const VideoMovieUtils::FnCallback callBack)
{
	{
		QMutexLocker lock(&webpMgr.m_mutexMapWebp);

		// ���ý������
		if (webpMgr.m_mapWebp.contains(strFilePath))
		{
			auto data = webpMgr.m_mapWebp.value(strFilePath);

			if (callBack)
			{
				// �ص�����֡���ݽṹ��Ҫ���·����ڴ�
				callBack(data->clone());
			}

			return;
		}
	}

	VWebp* pWebp = new VWebp();
	pWebp->initWebpParse(strFilePath);

	connect(pWebp, &QThread::finished, pWebp, &QObject::deleteLater);

	connect(pWebp, &VWebp::sigWebpParseResult, context, [=]() {
		QSharedPointer<VideoMovieUtils::SpriteSheetVo> spData;

		{
			QMutexLocker lock(&webpMgr.m_mutexMapWebp);

			if (webpMgr.m_mapWebp.contains(strFilePath))
			{
				spData = webpMgr.m_mapWebp.value(strFilePath);
			}
		}

		if (callBack)
		{
			// �ص�����֡���ݽṹ��Ҫ���·����ڴ�
			callBack(spData->clone());
		}
	});

	bool bStart = pWebp->startWebpParse();
	if (!bStart)
	{
		delete pWebp;
		pWebp = NULL;
	}
}

// ����webp�������
void VWebp::doWebpParseResult()
{
	QMutexLocker lock(&webpMgr.m_mutexMapWebp);

	if (webpMgr.m_mapWebp.contains(m_strFile))
		return;

	QSharedPointer<VideoMovieUtils::SpriteSheetVo> spData = QSharedPointer<VideoMovieUtils::SpriteSheetVo>(new VideoMovieUtils::SpriteSheetVo());

	if (!m_listWebpFrame.isEmpty())
		spData->inverval = m_listWebpFrame.first().msFrame;
	spData->width = m_width;
	spData->height = m_height;
	spData->format = QImage::Format_RGBA8888;

	for (auto & each : m_listWebpFrame)
	{
		VideoMovieUtils::SpriteFrameVo frame;
		frame.argb = (uchar*)each.baFrame.data();
		frame.frameIndex = spData->frames.size();
		frame.x = 0;
		frame.y = 0;
		frame.width = m_width;
		frame.height = m_height;

		// ��������֡���ݽṹ��Ҫ���·����ڴ�
		spData->frames.push_back(frame.clone());

		frame.argb = NULL; // ���ⱻ�����ͷ�
	} // for

	webpMgr.m_mapWebp.insert(m_strFile, spData);
}
