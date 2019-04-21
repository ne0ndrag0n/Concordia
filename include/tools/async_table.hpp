#ifndef CONCORDIA_ASYNC_TABLE
#define CONCORDIA_ASYNC_TABLE

#include <functional>
#include <utility>
#include <vector>
#include <queue>
#include <list>

namespace BlueBear::Tools {

	template< typename... CallbackSignature >
	class AsyncTable {
	public:
		using Task = std::function< void() >;
		using Callback = std::function< void( CallbackSignature... ) >;
		using TaskCallbackPair = std::pair< std::queue< Task >, Callback >;

	private:
		std::list< TaskCallbackPair > table;

	public:
		void enqueue( const std::vector< Task >& tasks, const Callback& callback ) {
			if( tasks.empty() ) {
				return;
			}

			TaskCallbackPair pair{ {}, callback };

			for( const auto& task : tasks ) {
				pair.first.push( task );
			}

			table.emplace_back( std::move( pair ) );
		}

		void update() {
			auto it = table.begin();

			while( it != table.end() ) {
				auto& task = it->first.front();
				if( task ) { task(); }
				it->first.pop();
				if( it->first.empty() ) {
					// Hit callback then remove iterator
					if( it->second ) {
						it->second();
					}

					it = table.erase( it );
				} else {
					++it;
				}
			}
		}
	};
}

#endif