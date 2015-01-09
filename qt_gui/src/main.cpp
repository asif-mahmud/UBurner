#include <QApplication>
#include <global_qt_gui.hpp>
#include <opprog_int.hpp>
#include <iostream>

using namespace std;

int main(int argc, char ** argv)
{
	QApplication app(argc,argv);

	init_gui();	

	return app.exec();
}

