#include "taricha_hash_stream.h"
#include "tarichasum.h"

int main(int argc, char **argv)
{
	return tarichasum_main(argc, argv, taricha512_hash_stream,
			"taricha512sum", 64, "taricha512");
}
