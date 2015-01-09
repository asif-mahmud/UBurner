/**
* splash.hpp
* Author : Asif Mahmud Shimon
*/
#ifndef QT_SPLASH_HPP
#define QT_SPLASH_HPP

#include <QSplashScreen>

class Splash : public QSplashScreen
{
	Q_OBJECT

	public :
		Splash(QWidget * parent = NULL );
		virtual ~Splash();

		void show_msg(QString);
		void view_splash();
		void mousePressEvent(QMouseEvent *);
};

#endif//QT_SPLASH_HPP
