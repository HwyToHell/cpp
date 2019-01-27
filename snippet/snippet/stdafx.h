#pragma once
// std headers
#if defined (_WIN32)
#include <conio.h>
#include <direct.h>		// for _mkdir, _getcwd
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <cmath>
#include <iostream>
#include <list>
#include <string>

// 3rd party headers
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <opencv2/opencv.hpp>
#include <sqlite3.h>