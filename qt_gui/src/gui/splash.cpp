#include <splash.hpp>
#include <gui_config.hpp>

Splash :: Splash (QWidget * parent) : QSplashScreen(parent)
{
	QPixmap pix(DEFAULT_SPLASH_SCREEN);
	this->setPixmap(pix);
	this->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::SplashScreen);
}

Splash :: ~Splash()
{}

void Splash :: view_splash()
{
	this->show();
}

void Splash :: show_msg(QString msg)
{
	this->showMessage(msg,Qt::AlignLeft|Qt::AlignBottom);
}

void Splash ::mousePressEvent(QMouseEvent * e)
{
	return;
}
