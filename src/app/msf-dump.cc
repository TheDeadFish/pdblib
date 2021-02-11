#include <stdshit.h>
#include "../src/msf.h"
const char progName[] = "test";

int main(int argc, char** argv)
{
	if(argc < 3) {
		printf("msf-dump: <input> <output-prefix>\n");
		return 1;
	}
	
	MsfFile msf;
	int result = msf.load(argv[1]);
	if(result) {
		printf("msf load failed: %d\n", result);
		return 1; }
		
	int index = 0;
	for(auto& xa : msf) {
		xstr fileName = xstrfmt("%s-%d.bin", argv[2], index++);
		if(xa.len == 0) continue;
		int result = saveFile(fileName.data, xa.data, xa.len);
		if(result) {
			printf("failed to write file: %s\n", fileName.data);
		}
	}
		
	return 0;
}
