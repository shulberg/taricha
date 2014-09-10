#include "taricha_hash_stream.h"
#include "tarichasum.h"
	
int main(int argc, char **argv)
{
	return tarichasum_main(argc, argv, taricha2_512_hash_stream,
			"taricha2_512sum", "taricha2-512", 64);
}
