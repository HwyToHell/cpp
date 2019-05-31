//#pragma once
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
#ifdef __GNUC__
#include <chrono>
#include <thread>
#elif #if _MSC_VER <= 1911
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#else
#include <chrono>
#include <thread>
#endif
#include <opencv2/opencv.hpp>
#include <sqlite3.h>
