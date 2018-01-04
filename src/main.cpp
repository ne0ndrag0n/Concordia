#include "application.hpp"

int main() {
	BlueBear::Application application;
	application.setupMainState();
	return application.run();
}
