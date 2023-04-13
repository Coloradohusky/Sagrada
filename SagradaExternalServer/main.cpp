#include "Bundler.hpp"
#include "sagradaUtils.hpp"
#include "Server.hpp"


int main(void) 
{
	int errorState = 0;
	Server svr;
	errorState = svr.start();

	return errorState;
}