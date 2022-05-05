#pragma once

#include <QAbstractItemModel>
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QQueue>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStringView>
#include <QTimer>
#include <QThread>
#include <QValidator>

#include <iostream>

namespace grepQt
{
	constexpr QColor errorHighlight(0, 0, 0, 128);

	constexpr QColor activeGreen(0, 128, 0);
	constexpr QColor inactiveGreen(0, 64, 0);

	constexpr QColor activeRed(192, 0, 0);
	constexpr QColor inactiveRed(96, 0, 0);
}
