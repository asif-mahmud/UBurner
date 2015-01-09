#include <about_widget.hpp>
#include <QtGui>

AboutWidget :: AboutWidget(QWidget * parent) : QDialog(parent)
{
	license_button = new QPushButton("License");
	help_button = new QPushButton("Help");
	about_button = new QPushButton("About");
	file_viewer = new QTextBrowser;
	file_viewer->setReadOnly(true);
	
	QVBoxLayout * left_lt = new QVBoxLayout;
	left_lt->addWidget(license_button);
	left_lt->addWidget(help_button);
	left_lt->addWidget(about_button);
	left_lt->addStretch();
	
	QHBoxLayout * main_lt = new QHBoxLayout;
	main_lt->addLayout(left_lt);
	main_lt->addWidget(file_viewer);
	
	setLayout(main_lt);
}

AboutWidget ::~AboutWidget() {}
