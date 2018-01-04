#include "application.hpp"
#include "log.hpp"
#include "localemanager.hpp"

int main() {
	BlueBear::Application application;
	application.setupMainState();
	return application.run();
}
