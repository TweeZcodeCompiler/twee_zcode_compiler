#pragma once
/**
 * Example usage:
 * Twee::TweeParser parser;
 * return parser.parse();
*/
#include "../TweeScanner.h"

namespace Twee {

	class TweeParser {
		public:
			TweeParser() : parser(scanner) {}
		
			int parse();
		
		private:
			Twee::TweeScanner scanner;
			Twee::BisonParser parser;
	};
}
