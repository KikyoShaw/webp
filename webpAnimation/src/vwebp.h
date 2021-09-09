#ifndef VWEBP_H
#define VWEBP_H

#include "VideoMovieUtils.h"
#include <QThread>

struct stWebpFrame
{
	QByteArray baFrame; // ֡����
	int msFrame; // ֡ʱ�������룩
};

class VWebp : public QThread
{
	Q_OBJECT

public:
	explicit VWebp(QObject* parent = Q_NULLPTR);
	~VWebp();

	// ��ʼ����webp�ļ�·��
	void initWebpParse(QString strFile);

	// ��ʼwebp�ļ�����
	bool startWebpParse();

	// ��webpת��png
	static void convertAlphaWebpToPngs(QObject* context, const QString& strFilePath, const QString& strSaveFolder, bool bDecode, const VideoMovieUtils::FnCallback callBack);

protected:
	virtual void run();

signals:
	// webp�ļ�������ɿ��Ի�ȡ���
	void sigWebpParseResult();

protected:
	// ����webp�������
	void doWebpParseResult();

private:
	QString m_strFile; // webp�ļ�·��
	QList<stWebpFrame> m_listWebpFrame; // ����֡����
	int m_width = 0; // ÿ֡���
	int m_height = 0; // ÿ֡�߶�
};

#endif // VWEBP_H
