/*
	error.h
	
	(c) Richard Thrippleton
	Licensing terms are in the 'LICENSE' file
	If that file is not included with this source then permission is not given to use this source in any way whatsoever.
*/

#include <cinttypes>
class error //Game failure exception (human readable by design)
{
	public:
	error(const char* str); //Constructor sets the string

	static void debug(const char* str,int32_t num); //Prints a debug string and integer immediately to console

	char str[129]; //Error string
};
