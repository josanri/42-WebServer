#ifndef EXTRACTOR_HPP
# define EXTRACTOR_HPP

# include <string>
# include <vector>

class Extractor {
	public:
		Extractor();
		~Extractor();

		static std::string str(std::string & text, std::string const & property);

		static bool b(std::string & text, std::string const & property);

		static int i(std::string & text, std::string const & property);
		static unsigned int ui(std::string & text, std::string const & property);

		static std::vector<std::string> v_str(std::string & text, std::string const & property);
		static std::vector<int> v_i(std::string & text, std::string const & property);
	
		static std::map<std::string, std::string> m_str_str(std::string & text, std::string const & property);
		static std::map<int, std::string> m_i_str(std::string & text, std::string const & property);
	private:
		Extractor(Extractor const & src);
		Extractor & operator=(Extractor const & rhs);
};

#endif
