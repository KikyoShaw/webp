#ifndef VWEBP_H
#define VWEBP_H

#include "VideoMovieUtils.h"
#include <QThread>

struct stWebpFrame
{
	QByteArray baFrame; // 帧数据
	int msFrame; // 帧时长（毫秒）
};

class VWebp : public QThread
{
	Q_OBJECT

public:
	explicit VWebp(QObject* parent = Q_NULLPTR);
	~VWebp();

	// 初始设置webp文件路径
	void initWebpParse(QString strFile);

	// 开始webp文件解析
	bool startWebpParse();

	// 把webp转成png
	static void convertAlphaWebpToPngs(QObject* context, const QString& strFilePath, const QString& strSaveFolder, bool bDecode, const VideoMovieUtils::FnCallback callBack);

protected:
	virtual void run();

signals:
	// webp文件解析完成可以获取结果
	void sigWebpParseResult();

protected:
	// 缓存webp解析结果
	void doWebpParseResult();

private:
	QString m_strFile; // webp文件路径
	QList<stWebpFrame> m_listWebpFrame; // 所有帧数据
	int m_width = 0; // 每帧宽度
	int m_height = 0; // 每帧高度
};

#endif // VWEBP_H
