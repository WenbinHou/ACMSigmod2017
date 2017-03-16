#include "common.h"
#include "hashmap.h"
#include <cassert>


int main(int argc, char* argv[])
{
	static char buffer[1048576 * 256];
	std::string out;
	while (true) {
		scanf("%[^\n]%*c", buffer);
		if (buffer[0] == 'S' && buffer[1] == '\0') {
			printf("R\n");
			fflush(stdout);
			break;
		}
	}

    //PatternMap.GetOrAdd(98);
    
    std::atomic_bool Lock;
    Lock = false;

	bool lastIsF = false;
	while (true) {
		int readCnt;
		while (true) {
			readCnt = scanf("%[^\n]%*c", buffer);
			if (readCnt == EOF) {
				assert(lastIsF);
				break;
			}
			else if (buffer[0] == 'F' && buffer[1] == '\0') {
				fprintf(stderr, "[Worker] Get 'F\\n': a batch of lines!\n");
				lastIsF = true;
				break;
			}
			lastIsF = false;
			out += buffer;
			out += '\n';
		}
		if (readCnt == EOF) {
			break;
		}
	}

	printf("%s", out.c_str());
	fflush(stdout);

	fprintf(stderr, "[Worker] Done! Exit now.\n");
	return 0;
}
