#include "Bundler.hpp"
#include "sagradaUtils.hpp"
#include "Server.hpp"


int main(void) 
{
	Server svr(46580);
	svr.start();
	return 0;
}