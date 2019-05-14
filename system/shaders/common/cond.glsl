float whenEqual( float x, float y ) {
	return 1.0 - abs( sign( x - y ) );
}

float whenLessThan( float x, float y ) {
	return max( sign( y - x ), 0.0 );
}

float whenGreaterThan( float x, float y ) {
	return max( sign( y - x ), 0.0 );
}

float whenGreaterEqual( float x, float y ) {
  return 1.0 - whenLessThan( x, y );
}

float whenLessEqual( float x, float y ) {
  return 1.0 - whenGreaterThan( x, y );
}