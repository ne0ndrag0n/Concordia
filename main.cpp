#include "bluebear.hpp"
#include <iostream>
#include <squirrel.h>

int main() {
	BlueBear::Engine engine;
	BlueBear::BBObject test = engine.getObjectFromFile( "hello.sq" );
	
	/*
	HSQUIRRELVM sqvm = sq_open(INITIAL_SQVM_STACK_SIZE);

	sq_setprintfunc(sqvm, SquirrelUtils::squirrel_print_function, NULL);
	const SQChar *program = "print(\"Hello World!\\n\");";

	if (
		SQ_FAILED(
			sq_compilebuffer(
				sqvm,
				program,
				sizeof(SQChar) * strlen(program),
				"program",
				SQFalse
			)
		)
	) {
        SquirrelUtils::squirrel_print_last_error(sqvm);
        return 1;
    }

	sq_pushroottable(sqvm);
	if (
		SQ_FAILED(
			sq_call(sqvm, 1, SQFalse, SQFalse)
		)
	) {
		 SquirrelUtils::squirrel_print_last_error(sqvm);
		 return 1;
	}

	sq_close(sqvm);
	*/
	return 0;
}
