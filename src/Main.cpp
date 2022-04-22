#include "MainWindow.hpp"
#include "FileSearcher.hpp"

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

	if (context.function && !message.isEmpty())
	{
		qtMessageTypeToStreamType(type)
			<< time.toStdWString() << L" ["
			<< qtMesssageTypeToChar(type) << L"] "
			<< context.function << L':'
			<< context.line << L": "
			<< message.toStdWString() << std::endl;
	}

	if (context.function && message.isEmpty())
	{
		qtMessageTypeToStreamType(type)
			<< time.toStdWString() << L" ["
			<< qtMesssageTypeToChar(type) << L"] "
			<< context.function << L':'
			<< context.line << std::endl;
	}
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

	qDebug() << QTextCodec::availableCodecs();

	QApplication application(argc, argv);
	loadIcon(application);
	MainWindow window;
	window.show();
	return application.exec();
}
