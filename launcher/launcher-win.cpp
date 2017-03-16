#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <deque>
#include <cstdlib>

#define BUFFER_SIZE		(1048576 * 256)

using namespace std;

HANDLE stdin_r, stdin_w, stdout_r, stdout_w;


void ReadAllLines(const string& filePath, __out vector<string>& lines)
{
	ifstream fs;
	fs.open(filePath, ios::in | ios::binary);
	assert(fs);
	string line;
	while (getline(fs, line, '\n')) {
		size_t len = line.size();
		if (len > 0) {
			if (line[len - 1] == '\r') {
				line.resize(len - 1);
			}
			lines.push_back(line);
		}
	}
}

int GetFileSize(const string& filePath) {
	FILE* file = fopen(filePath.c_str(), "rb");
	assert(file);
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fclose(file);
	return size;
}

void MyCreatePipe(__out HANDLE& hRead, __out HANDLE& hWrite)
{
	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0x00, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	BOOL succ = CreatePipe(&hRead, &hWrite, &sa, BUFFER_SIZE);
	assert(succ);
}

void MyCreateProcess(const string filePath)
{
	STARTUPINFOA sInfo;
	ZeroMemory(&sInfo, sizeof(sInfo));

	PROCESS_INFORMATION pInfo;
	ZeroMemory(&pInfo, sizeof(pInfo));
	sInfo.cb = sizeof(sInfo);
	sInfo.dwFlags = STARTF_USESTDHANDLES;
	sInfo.hStdInput = stdin_r;
	sInfo.hStdOutput = stdout_w;
	sInfo.hStdError = GetStdHandle(STD_OUTPUT_HANDLE);

	BOOL succ = CreateProcessA(
		nullptr,
		(LPSTR)filePath.c_str(),
		nullptr,
		nullptr,
		TRUE,
		NORMAL_PRIORITY_CLASS,
		nullptr,
		nullptr,
		&sInfo,
		&pInfo);
	assert(succ);

	HANDLE dummyTarget;
	succ = DuplicateHandle(pInfo.hProcess, stdin_w, GetCurrentProcess(), &dummyTarget, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
	assert(succ);
	CloseHandle(dummyTarget);
	succ = DuplicateHandle(pInfo.hProcess, stdout_r, GetCurrentProcess(), &dummyTarget, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
	assert(succ);
	CloseHandle(dummyTarget);

	CloseHandle(pInfo.hProcess);
	CloseHandle(pInfo.hThread);
}

void WriteLine(const string& line, bool flush = false) {
	static string buf;
	buf.append(line);
	buf.append("\n");
	if (flush) {
		DWORD writtenBytes;
		BOOL success = WriteFile(stdin_w, buf.c_str(), (DWORD)buf.size(), &writtenBytes, NULL);
		assert(success);
		assert(writtenBytes == buf.size());
		buf.resize(0);
	}
}

string ReadLine() {
	static deque<string> dq;
	if (!dq.size()) {
		while (true) {
			static char buf[BUFFER_SIZE];  // We assume one line doesn't excceed BUFFER_SIZE=256MB
			static size_t curr = 0;

			DWORD totBytes, readBytes;
			BOOL success = PeekNamedPipe(stdout_r, nullptr, 0, NULL, &totBytes, NULL);
			assert(success);
			if (totBytes == 0) {
				Sleep(1);
				continue;
			}

			totBytes = (DWORD)min(totBytes, sizeof(buf) - curr);
			success = ReadFile(stdout_r, buf + curr, totBytes, &readBytes, NULL);
			assert(success);
			assert(readBytes == totBytes);

			char* st = buf;
			for (char* p = buf + curr; p < (buf + curr) + readBytes; ++p) {
				if (*p == '\r' && p + 1 < (buf + curr) + readBytes) {
					assert(*(p + 1) == '\n');
					dq.push_back(string(st, p - st));
					st = p + 2;
					++p;
				}
				else if (*p == '\n') {
					dq.push_back(string(st, p - st));
					st = p + 1;
				}
			}

			if (st == buf) {  // This is not a '\n' in this read
				assert(dq.size() == 0);
				continue;
			}

			assert(dq.size() > 0);
			assert(st > buf && st <= (buf + curr) + readBytes);
			size_t newCurr = (buf + curr + readBytes) - st;
			memmove(buf, st, newCurr);
			curr = newCurr;
			break;
		}
	}

	assert(dq.size());
	const string line = dq.front();
	dq.pop_front();
	return line;
}

int main(int argc, char* argv[])
{
	ULONGLONG mainStartTime = GetTickCount64();
	static_assert(sizeof(void*) == 8, "Must run as 64bit process");

	if (argc != 3) {
		fprintf(stderr,
			"\n"
			"Launcher expects exactly 2 parameters.\n"
			"Usage:\n"
			"  launcher <Directory> <TestCase>\n"
			"\n");
		exit(1);
	}

	//
	// Prepare the test-case files
	//
	string directory = argv[1];
	for (size_t i = 0; i < directory.size(); ++i) {
		if (directory[i] == '\\') {
			directory[i] = '/';
		}
	}
	if (*directory.rbegin() != '/') {
		directory.append("/");
	}
	string fpInit = directory + argv[2] + ".init";
	string fpWork = directory + argv[2] + ".work";
	string fpResult = directory + argv[2] + ".result";
	vector<string> lnInit, lnWork, lnResult;
	ReadAllLines(fpInit, lnInit);
	ReadAllLines(fpWork, lnWork);
	ReadAllLines(fpResult, lnResult);
	fprintf(stdout, "[Launcher] All test case files prepared.\n");
	fflush(stdout);

	//
	// Get pku45a618 process path & run the process
	//
	string exePath(260, '\0');
	exePath.resize(GetModuleFileNameA(NULL, (LPSTR)exePath.c_str(), (DWORD)exePath.max_size()));
	exePath = exePath.substr(0, exePath.find_last_of('\\')) + "\\pku45a618.exe";
	MyCreatePipe(stdin_r, stdin_w);
	MyCreatePipe(stdout_r, stdout_w);
	MyCreateProcess(exePath);

	//
	// Write the init file
	//
	for (auto it = lnInit.begin(); it != lnInit.end(); ++it) {
		WriteLine(*it);
	}
	WriteLine("S", true);

	string line = ReadLine();
	if (line != "R") {
		printf("[Launcher] Error! Expect 'R\\n' after processing initial file\n");
	}
	fprintf(stdout, "[Launcher] Initial file processed. Start working...\n");
	fflush(stdout);

	ULONGLONG stTime = GetTickCount64();
	for (auto it = lnWork.begin(); it != lnWork.end(); ++it) {
		WriteLine(*it);
	}
	WriteLine("F", true);
	CloseHandle(stdin_w);

	for (size_t i = 0; i < lnWork.size(); ++i) {
		line = ReadLine();
		//fprintf(stdout, "%s\n", line.c_str());
	}
	fprintf(stdout, "[Launcher] Time:  %.3lfs\n", (GetTickCount64() - stTime) / 1000.0);
	fprintf(stdout, "[Launcher] Total: %.3lfs\n", (GetTickCount64() - mainStartTime) / 1000.0);
	fflush(stdout);
	return 0;
}

#endif
