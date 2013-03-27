#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		string mode = argv[1];
		int threads = 1;
		if (argc > 2)
		{
			threads = ctoi(argv[2]);
		}

		if (threads > 1)
		{
		}
		else
		{
		}
	}

	return 0;
}
