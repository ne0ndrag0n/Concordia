#include "bluebear.hpp"
#include <cstdio>
#include <cstdarg>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <iostream>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace BlueBear {
	
	Engine::Engine() {
		L = luaL_newstate();
		luaL_openlibs( L );
	}
	
	Engine::~Engine() {
		lua_close( L );
	}
	
	BBObject::BBObject( const char* fileName ) {
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
	
	
	bool BBObject::good() {
		return this->fileContents != NULL;
	}
	
	const char* BBObject::getFileContents() {
		return this->fileContents;
	}

	BBObject Engine::getObjectFromFile( const char* fileName ) {
		BlueBear::BBObject bbObject( fileName );
		
		if( bbObject.good() ) {
			this->objects.push_back( bbObject );
			return bbObject;
		}
		
		return NULL;
	}

}
