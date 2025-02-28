#include "output.hpp"

int main()
{
	ExampleNS::ExampleFile f;
	f.load("test_in.xml");
	f.save("test_out.xml");
	return 0;
}
