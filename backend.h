#ifndef CHIRP_BACKEND_H
#define CHIRP_BACKEND_H

namespace backendheader {
	class Backend {
		public:
			bool put();
			string get(string key);
			bool delete(string key);

		private:
			std::map<>
	}
} // backendheader
#endif //CHIRP_BACKEND_H
