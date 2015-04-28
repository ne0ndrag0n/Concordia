#ifndef BLUEBEAR
#define BLUEBEAR

#include <squirrel.h>
#include <string>
#define INITIAL_SQVM_STACK_SIZE 1024

/**
 * BlueBear objects
 *
 *
 */
namespace BlueBear {
	class Engine {
		private:
			HSQUIRRELVM sqvm;

		public:
			Engine();
	};

	class BBObject {
		private:
			std::string fileName;
		public:
			BBObject(std::string fileName);
	};
}


namespace SquirrelUtils {
	void squirrel_print_last_error(HSQUIRRELVM sqvm);

	void squirrel_print_function(HSQUIRRELVM sqvm, const SQChar *format, ...);
}

#endif
