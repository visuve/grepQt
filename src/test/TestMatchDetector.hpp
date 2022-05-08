#pragma once

#include "Options.hpp"

class TestMatchDetector: public QObject
{
	Q_OBJECT
public:
	TestMatchDetector();

private slots:
	void find_data();
	void find();

private:
	Options _options;
};
