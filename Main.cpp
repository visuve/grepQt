#include "PCH.hpp"
#include "MainWindow.hpp"

void loadIcon(QApplication& application)
{
	QPixmap pixmap;

	if (!pixmap.load(":/grepQt.svg"))
	{
		qWarning() << "Failed to load application icon!";
		return;
	}

	application.setWindowIcon(pixmap);
}

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	loadIcon(application);
	MainWindow window;
	window.show();
	return application.exec();
}
