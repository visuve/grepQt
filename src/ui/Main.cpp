#include "MainWindow.hpp"
#include "FileSearcher.hpp"

#include <QStyleFactory>

wchar_t qtMesssageTypeToChar(QtMsgType type)
{
	switch (type)
	{
		case QtDebugMsg:
			return L'D';
		case QtInfoMsg:
			return L'I';
		case QtWarningMsg:
			return L'W';
		case QtCriticalMsg:
			return L'C';
		case QtFatalMsg:
			return L'F';
	}

	return L'?';
}

std::wostream& qtMessageTypeToStreamType(QtMsgType type)
{
	switch (type)
	{
		case QtDebugMsg:
		case QtInfoMsg:
			return std::wcout;
		case QtWarningMsg:
			return std::wclog;
		case QtCriticalMsg:
		case QtFatalMsg:
			return std::wcerr;
	}

	return std::wcerr;
}

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	const QString time = QTime::currentTime().toString(Qt::DateFormat::ISODateWithMs);

	QString place;

	if (context.function)
	{
		place = context.function;
		place.remove("__cdecl ");
	}
	else
	{
		place = QFileInfo(context.file).fileName();
	}

	qtMessageTypeToStreamType(type)
		<< time.toStdWString() << L" ["
		<< qtMesssageTypeToChar(type) << L"] "
		<< place.toStdWString() << L':'
		<< context.line << L": ";

	if (!message.isEmpty())
	{
		qtMessageTypeToStreamType(type) << message.toStdWString();
	}

	qtMessageTypeToStreamType(type) << std::endl;
}

void setupTheme(QApplication& application)
{
	constexpr QColor darkGray(53, 53, 53);
	constexpr QColor gray(128, 128, 128);
	constexpr QColor black(25, 25, 25);
	constexpr QColor blue(42, 130, 218);
	constexpr QColor highlight(64, 196, 64);

	application.setStyle(QStyleFactory::create("Fusion"));

	QPalette darkPalette;
	darkPalette.setColor(QPalette::Window, darkGray);
	darkPalette.setColor(QPalette::WindowText, Qt::white);
	darkPalette.setColor(QPalette::Base, black);
	darkPalette.setColor(QPalette::AlternateBase, darkGray);
	darkPalette.setColor(QPalette::ToolTipBase, blue);
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
	darkPalette.setColor(QPalette::Text, Qt::white);
	darkPalette.setColor(QPalette::Button, darkGray);
	darkPalette.setColor(QPalette::ButtonText, Qt::white);
	darkPalette.setColor(QPalette::Link, blue);
	darkPalette.setColor(QPalette::Highlight, highlight);
	darkPalette.setColor(QPalette::HighlightedText, Qt::black);

	darkPalette.setColor(QPalette::Active, QPalette::Button, gray.darker());
	darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, gray);
	darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, gray);
	darkPalette.setColor(QPalette::Disabled, QPalette::Text, gray);
	darkPalette.setColor(QPalette::Disabled, QPalette::Light, darkGray);

	application.setPalette(darkPalette);
}

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
	qInstallMessageHandler(messageHandler);

	QApplication application(argc, argv);
	setupTheme(application);
	loadIcon(application);
	MainWindow window;
	window.show();
	return application.exec();
}
