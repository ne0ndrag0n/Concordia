#ifndef BLUEBEAR
#define BLUEBEAR

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
			std::vector< BlueBear::BBObject > objects;

		public:
			Engine();
			~Engine();
			BlueBear::BBObject getObjectFromFile( const char* fileName );
	};
	
}

#endif
