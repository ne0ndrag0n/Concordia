#ifndef CONCORDIA_SHADER_GLOBAL
#define CONCORDIA_SHADER_GLOBAL

#include <GL/glew.h>
#include <functional>

namespace BlueBear::Graphics::Shader {

    template< typename Std140Struct >
    class ShaderGlobal {
        Std140Struct data;
        GLuint ubo;

    public:
        ShaderGlobal( const Std140Struct& data ) : data( data ) {
            glGenBuffers( 1, &ubo );
            glBindBuffer( GL_UNIFORM_BUFFER, ubo );
                glBufferData( GL_UNIFORM_BUFFER, sizeof( Std140Struct ), &data, GL_DYNAMIC_DRAW );
            glBindBuffer( GL_UNIFORM_BUFFER, 0 );
        }

        ~ShaderGlobal() {
            glDeleteBuffers( 1, &ubo );
        }

        void update( const std::function< void( Std140Struct& ) >& functor ) {
            functor( data );

            glBindBuffer( GL_UNIFORM_BUFFER, ubo );
                glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof( Std140Struct ), &data );
            glBindBuffer( GL_UNIFORM_BUFFER, 0 );
        }
    };

}

#endif