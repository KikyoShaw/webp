#pragma once

#include <QtWidgets/QWidget>
#include "ui_webpAnimation.h"

class webpAnimation : public QWidget
{
    Q_OBJECT

public:
    webpAnimation(QWidget *parent = Q_NULLPTR);

private:
    Ui::webpAnimationClass ui;
};
