#include "TXCloudRecord.h"
#include "crashdump.h"

int main(int argc, char *argv[])
{
	CrashDump dump;

	return TXCloudRecord::instance().run(argc, argv);
}
