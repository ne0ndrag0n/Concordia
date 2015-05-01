#ifndef BLUEBEAR
#define BLUEBEAR

#include <squirrel.h>
#include <string>
#include <vector>
#include <fstream>
#define INITIAL_SQVM_STACK_SIZE 1024

/**
 * BlueBear objects
 *
 *
 */
namespace BlueBear {
	
	class BBObject {
		private:
			const char* fileName;
			const char* fileContents;
		public:
			BBObject( const char* fileName );
			bool good();
			const char* getFileContents();
	};

	class Engine {
		private:
			HSQUIRRELVM sqvm;
			std::vector< BlueBear::BBObject > objects;

		public:
			Engine();
			~Engine();
			BlueBear::BBObject getObjectFromFile( const char* fileName );
	};
	
}


namespace SquirrelUtils {
	void squirrel_print_last_error(HSQUIRRELVM sqvm);

	void squirrel_print_function(HSQUIRRELVM sqvm, const SQChar *format, ...);
}

#endif
