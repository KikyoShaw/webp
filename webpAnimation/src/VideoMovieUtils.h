#pragma once

#include "vutility.h"
#include <QImage>

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

	typedef std::function<void(SpriteSheetVo*)> VUTILITY_LIBRARY_EXPORT FnCallback;
	//typedef void(*FnCallback)(SpriteSheetVo* vo);

	QString VUTILITY_LIBRARY_EXPORT getSaveFolderFromFilePath(const QString& strFilePath);

	//SpriteSheetVo* VUTILITY_LIBRARY_EXPORT getSpriteSheet(const QString& strSaveFolder);
	bool VUTILITY_LIBRARY_EXPORT getSpriteSheet(const QString& strSaveFolder, SpriteSheetVo*& ret, bool bDecode);

	//void VUTILITY_LIBRARY_EXPORT convertAlphaMp4ToPngs(QObject* context, const QString& strFilePath, bool bDecode, const FnCallback callBack);
	void VUTILITY_LIBRARY_EXPORT convertAlphaMp4ToPngs(QObject* context, const QString& strFilePath, const QString& strSaveFolder, bool bDecode, const FnCallback callBack);

};
