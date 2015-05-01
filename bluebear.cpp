#include "bluebear.hpp"
#include <cstdio>
#include <cstdarg>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <iostream>
#include <squirrel.h>

void SquirrelUtils::squirrel_print_last_error(HSQUIRRELVM sqvm) {
	const SQChar *error;
	sq_getlasterror(sqvm);
	if (SQ_SUCCEEDED(sq_getstring(sqvm, -1, &error))) {
		fprintf(stderr, "sqvm error: %s\n", error);
	}
}

void SquirrelUtils::squirrel_print_function(HSQUIRRELVM sqvm, const SQChar *format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
}

BlueBear::Engine::Engine() {
	this->sqvm = sq_open( INITIAL_SQVM_STACK_SIZE );

	sq_setprintfunc( this->sqvm, SquirrelUtils::squirrel_print_function, NULL );
}

BlueBear::Engine::~Engine() {
	sq_close( this->sqvm );
	
}

BlueBear::BBObject::BBObject( const char* fileName ) {
	this->fileName = fileName;
	this->fileContents = NULL;

	std::ifstream fileStream( fileName );
	if( fileStream.good() ) {
		std::string content( 
			( std::istreambuf_iterator< char >( fileStream ) ),
			( std::istreambuf_iterator< char >()    	     ) 
		);
		this->fileContents = content.c_str();
	}
	
	fileStream.close();
}

bool BlueBear::BBObject::good() {
	return this->fileContents != NULL;
}

const char* BlueBear::BBObject::getFileContents() {
	return this->fileContents;
}

/**
 * Instantiate a BlueBear BBObject and add it to the vector
 */
BlueBear::BBObject BlueBear::Engine::getObjectFromFile( const char* fileName ) {
	BlueBear::BBObject bbObject( fileName );
	
	if( bbObject.good() ) {
		this->objects.push_back( bbObject );
		return bbObject;
	}
	
	return NULL;
}
