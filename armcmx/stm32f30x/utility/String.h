#ifndef _STRING_H_
#define _STRING_H_

#include <string>

class String {
	char str[256];
	int len;
	public:
		int length(void) const {return len; }
	char operator [] (unsigned int i) const { return str[i]; }
	char& operator [] (unsigned int i) { return str[i];}
};

#endif
