#ifndef BLUEBEAR
#define BLUEBEAR

#include <squirrel.h>

#define INITIAL_SQVM_STACK_SIZE 1024

/**
 * BlueBear objects
 *
 *
 */
namespace BlueBear {
	class BlueBearVM {
		private:
			HSQUIRRELVM sqvm;
	};
}

namespace SquirrelExamples {
	void squirrel_print_last_error(HSQUIRRELVM sqvm);

	void squirrel_print_function(HSQUIRRELVM sqvm, const SQChar *format, ...);
}

#endif
