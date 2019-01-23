#include <string>
#ifndef CHIRP_SERVICE_H
#define CHIRP_SERVICE_H

namespace serviceheader {
	class Service {
		public:
			bool registerUser(string username);
			bool loginUser(string username);
			bool chirp(string text);
			bool reply(string chirpID);
			bool follow(string username);
			string read(string chirpID);
			bool monitor();

		private:
			// include followers?
	}
} // serviceheader namespace
#endif //CHIRP_SERVICE_H