#pragma once

#include <QAbstractItemModel>
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QDirIterator>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QQueue>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStringView>
#include <QTimer>
#include <QThread>
#include <QValidator>

#include <array>
#include <iostream>

#include <icu.h>

constexpr QColor errorHighlight(0, 0, 0, 128);

constexpr QColor activeGreen(0, 128, 0);
constexpr QColor inactiveGreen(0, 64, 0);

constexpr QColor activeRed(192, 0, 0);
constexpr QColor inactiveRed(96, 0, 0);
