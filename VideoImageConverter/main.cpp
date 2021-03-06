#include <QApplication>
#include <videoloader.h>
#include <QDebug>
#include <QUrl>
#include "mainwindow.h"
#include "conversionprogressdialog.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setApplicationName(QStringLiteral(TARGET));
	QApplication::setApplicationVersion(QStringLiteral(VERSION));
	QApplication::setOrganizationName(QStringLiteral(COMPANY));
	QApplication::setOrganizationDomain(QStringLiteral("com.sykcoder42"));
	QApplication::setApplicationDisplayName(QCoreApplication::translate("main", "Video to APNG-Converter"));
	QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.ico")));

	qRegisterMetaType<QList<ConverterStream::SetupInfo*>>();
	qRegisterMetaType<ConvertFileInfo::Status>();
	qRegisterMetaType<QMessageBox::Icon>();

	MainWindow w;
	ConversionProgressDialog c;
	QObject::connect(&w, &MainWindow::startConversion,
					 &c, &ConversionProgressDialog::startConversion);
	w.show();

	return a.exec();
}
