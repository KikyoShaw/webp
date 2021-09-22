#pragma once

#include <QImage>
#include <QtCore>

namespace VideoMovieUtils
{
	struct SpriteFrameVo {
		uchar* argb = nullptr;	//png֡���е� ARGB���ݣ�ע����Ҫʹ�÷��Լ��ڴ��ͷ�

		int frameIndex;

		int x;
		int y;
		int width;
		int height;

		~SpriteFrameVo();

		SpriteFrameVo* clone();
	};

	struct SpriteSheetVo
	{
		QVector<SpriteFrameVo*> frames;	//png֡���е� ARGB���ݣ�ע����Ҫʹ�÷��Լ��ڴ��ͷ�
		int inverval;			//֮֡���ʱ��������λ:ms
		
		int width;
		int height;

		QImage::Format format;	//Ŀǰһ���� Format_RGBA8888

		~SpriteSheetVo();

		SpriteSheetVo* clone();
	};

	typedef std::function<void(SpriteSheetVo*)> FnCallback;
	//typedef void(*FnCallback)(SpriteSheetVo* vo);

	QString getSaveFolderFromFilePath(const QString& strFilePath);

	//SpriteSheetVo* getSpriteSheet(const QString& strSaveFolder);
	bool getSpriteSheet(const QString& strSaveFolder, SpriteSheetVo*& ret, bool bDecode);
};
