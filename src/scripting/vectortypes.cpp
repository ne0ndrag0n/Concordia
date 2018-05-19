#include "scripting/vectortypes.hpp"
#include <glm/glm.hpp>

namespace BlueBear::Scripting {

  void VectorTypes::submitLuaContributions( sol::table types ) {
    types.new_usertype< glm::vec2 >( "Vec2",
      sol::constructors< glm::vec2(), glm::vec2( double, double ) >(),
      "x", &glm::vec2::x,
      "y", &glm::vec2::y,
      sol::meta_function::addition, []( const glm::vec2& left, const glm::vec2& right ) { return left + right; },
      sol::meta_function::subtraction, []( const glm::vec2& left, const glm::vec2& right ) { return left - right; },
      sol::meta_function::multiplication, []( const glm::vec2& left, const glm::vec2& right ) { return left * right; },
      sol::meta_function::division, []( const glm::vec2& left, const glm::vec2& right ) { return left / right; }
    );
    types.new_usertype< glm::vec3 >( "Vec3",
      sol::constructors< glm::vec3(), glm::vec3( double, double, double ) >(),
      "x", &glm::vec3::x,
      "y", &glm::vec3::y,
      "z", &glm::vec3::z,
      sol::meta_function::addition, []( const glm::vec3& left, const glm::vec3& right ) { return left + right; },
      sol::meta_function::subtraction, []( const glm::vec3& left, const glm::vec3& right ) { return left - right; },
      sol::meta_function::multiplication, []( const glm::vec3& left, const glm::vec3& right ) { return left * right; },
      sol::meta_function::division, []( const glm::vec3& left, const glm::vec3& right ) { return left / right; }
    );
    types.new_usertype< glm::vec4 >( "Vec4",
      sol::constructors< glm::vec4(), glm::vec4( double, double, double, double ) >(),
      "x", &glm::vec4::x,
      "y", &glm::vec4::y,
      "z", &glm::vec4::z,
      "w", &glm::vec4::w,
      sol::meta_function::addition, []( const glm::vec4& left, const glm::vec4& right ) { return left + right; },
      sol::meta_function::subtraction, []( const glm::vec4& left, const glm::vec4& right ) { return left - right; },
      sol::meta_function::multiplication, []( const glm::vec4& left, const glm::vec4& right ) { return left * right; },
      sol::meta_function::division, []( const glm::vec4& left, const glm::vec4& right ) { return left / right; }
    );

    types.new_usertype< glm::uvec2 >( "Uvec2",
      sol::constructors< glm::uvec2(), glm::uvec2( double, double ) >(),
      "x", &glm::uvec2::x,
      "y", &glm::uvec2::y,
      sol::meta_function::addition, []( const glm::uvec2& left, const glm::uvec2& right ) { return left + right; },
      sol::meta_function::subtraction, []( const glm::uvec2& left, const glm::uvec2& right ) { return left - right; },
      sol::meta_function::multiplication, []( const glm::uvec2& left, const glm::uvec2& right ) { return left * right; },
      sol::meta_function::division, []( const glm::uvec2& left, const glm::uvec2& right ) { return left / right; }
    );
    types.new_usertype< glm::uvec3 >( "Uvec3",
      sol::constructors< glm::uvec3(), glm::uvec3( double, double, double ) >(),
      "x", &glm::uvec3::x,
      "y", &glm::uvec3::y,
      "z", &glm::uvec3::z,
      sol::meta_function::addition, []( const glm::uvec3& left, const glm::uvec3& right ) { return left + right; },
      sol::meta_function::subtraction, []( const glm::uvec3& left, const glm::uvec3& right ) { return left - right; },
      sol::meta_function::multiplication, []( const glm::uvec3& left, const glm::uvec3& right ) { return left * right; },
      sol::meta_function::division, []( const glm::uvec3& left, const glm::uvec3& right ) { return left / right; }
    );
    types.new_usertype< glm::uvec4 >( "Uvec4",
      sol::constructors< glm::uvec4(), glm::uvec4( double, double, double, double ) >(),
      "x", &glm::uvec4::x,
      "y", &glm::uvec4::y,
      "z", &glm::uvec4::z,
      "w", &glm::uvec4::w,
      sol::meta_function::addition, []( const glm::uvec4& left, const glm::uvec4& right ) { return left + right; },
      sol::meta_function::subtraction, []( const glm::uvec4& left, const glm::uvec4& right ) { return left - right; },
      sol::meta_function::multiplication, []( const glm::uvec4& left, const glm::uvec4& right ) { return left * right; },
      sol::meta_function::division, []( const glm::uvec4& left, const glm::uvec4& right ) { return left / right; }
    );

    types.new_usertype< glm::ivec2 >( "Ivec2",
      sol::constructors< glm::ivec2(), glm::ivec2( double, double ) >(),
      "x", &glm::ivec2::x,
      "y", &glm::ivec2::y,
      sol::meta_function::addition, []( const glm::ivec2& left, const glm::ivec2& right ) { return left + right; },
      sol::meta_function::subtraction, []( const glm::ivec2& left, const glm::ivec2& right ) { return left - right; },
      sol::meta_function::multiplication, []( const glm::ivec2& left, const glm::ivec2& right ) { return left * right; },
      sol::meta_function::division, []( const glm::ivec2& left, const glm::ivec2& right ) { return left / right; }
    );
    types.new_usertype< glm::ivec3 >( "Ivec3",
      sol::constructors< glm::ivec3(), glm::ivec3( double, double, double ) >(),
      "x", &glm::ivec3::x,
      "y", &glm::ivec3::y,
      "z", &glm::ivec3::z,
      sol::meta_function::addition, []( const glm::ivec3& left, const glm::ivec3& right ) { return left + right; },
      sol::meta_function::subtraction, []( const glm::ivec3& left, const glm::ivec3& right ) { return left - right; },
      sol::meta_function::multiplication, []( const glm::ivec3& left, const glm::ivec3& right ) { return left * right; },
      sol::meta_function::division, []( const glm::ivec3& left, const glm::ivec3& right ) { return left / right; }
    );
    types.new_usertype< glm::ivec4 >( "Ivec4",
      sol::constructors< glm::ivec4(), glm::ivec4( double, double, double, double ) >(),
      "x", &glm::ivec4::x,
      "y", &glm::ivec4::y,
      "z", &glm::ivec4::z,
      "w", &glm::ivec4::w,
      sol::meta_function::addition, []( const glm::ivec4& left, const glm::ivec4& right ) { return left + right; },
      sol::meta_function::subtraction, []( const glm::ivec4& left, const glm::ivec4& right ) { return left - right; },
      sol::meta_function::multiplication, []( const glm::ivec4& left, const glm::ivec4& right ) { return left * right; },
      sol::meta_function::division, []( const glm::ivec4& left, const glm::ivec4& right ) { return left / right; }
    );
  }

}
