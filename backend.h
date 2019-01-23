#include <string>

#ifndef CHIRP_BACKEND_H
#define CHIRP_BACKEND_H

namespace backendheader {
	class Backend {
		public:
			bool put(string key, string value);
			string get(string key);
			bool delete(string key);

		private:
			std::map<string, string> key_value_pairs;
	}
} // backendheader
#endif //CHIRP_BACKEND_H
