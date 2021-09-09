#include "webpAnimation.h"
#include "vwebp.h"

webpAnimation::webpAnimation(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	connect(ui.pushButton, &QPushButton::clicked, this, [=]() {
		/*VWebp* pWebp = new VWebp();
		pWebp->initWebpParse("1.webp");
		pWebp->startWebpParse();*/
		auto url = "https://image-app-test.jiaoyoushow.com/app/logo/voiceroombackground/1.webp";
		ui.widget->startPlay(url, QString());
		//auto path = "1.webp";
		//ui.widget->startPlay(QString(), path);
	});
}

webpAnimation::~webpAnimation()
{
	
}

void webpAnimation::closeEvent(QCloseEvent * event)
{
	ui.widget->stopPlay();
	QWidget::closeEvent(event);
}
