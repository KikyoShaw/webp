#pragma once

#include <QtWidgets/QWidget>
#include "ui_webpAnimation.h"

class webpAnimation : public QWidget
{
    Q_OBJECT

public:
    webpAnimation(QWidget *parent = Q_NULLPTR);
	~webpAnimation();

protected:
	virtual void closeEvent(QCloseEvent *event);

private:
    Ui::webpAnimationClass ui;
};
