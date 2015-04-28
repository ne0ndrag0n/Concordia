#include "bluebear.hpp"
#include <cstdio>
#include <cstdarg>

void SquirrelExamples::squirrel_print_last_error(HSQUIRRELVM sqvm) {
	const SQChar *error;
	sq_getlasterror(sqvm);
	if (SQ_SUCCEEDED(sq_getstring(sqvm, -1, &error))) {
		fprintf(stderr, "sqvm error: %s\n", error);
	}
}

void SquirrelExamples::squirrel_print_function(HSQUIRRELVM sqvm, const SQChar *format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
}
