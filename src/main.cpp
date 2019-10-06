#include <windows.h>

void release_main() {
	ExitProcess(0);
}

#if defined(DEBUG_BUILD)
int __cdecl main(int, char**) {
	release_main();
	return 0;
}
#endif
