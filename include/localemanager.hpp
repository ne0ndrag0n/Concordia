#ifndef L10NMANAGER
#define L10NMANAGER

#include <jsoncpp/json/json.h>

namespace BlueBear {

  class LocaleManager {

    public:
      static LocaleManager& getInstance() {
        static LocaleManager instance;
        return instance;
      }

      void setCurrentLocale( const std::string& locale );
      std::string getString( const std::string& key );

    private:
      static constexpr const char* LOCALE_PATH = "assets/locale/";
      Json::Value localeTable;
      std::string currentLocale;

      LocaleManager();
      LocaleManager( LocaleManager const& );
      void operator=( LocaleManager const& );
  };

}

#endif
