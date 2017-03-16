#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define ENABLE_ASSERTION    (1)

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstdarg>
#include <thread>
#include <string>
#include <atomic>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <ctype.h>

#ifdef _WIN32
#include <corecrt_io.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include "defs.h"
#include "debug.h"

#include "pattern.h"
