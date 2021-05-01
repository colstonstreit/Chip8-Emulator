#ifndef H_MAIN
#define H_MAIN

struct Data {
	int x = 0;
	int y = 43;
	double d = 2.0;
	const char* s = "Yes";

	Data() {

	}

	Data(int x, int y, double d, const char* s) : x(x), y(y), d(d), s(s){

	}
};

#endif /* H_MAIN */