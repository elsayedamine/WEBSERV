#ifndef DIRECTIVE_HPP
# define DIRECTIVE_HPP

#include <signal.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <Utils.hpp>

# define TRUE 1
# define FALSE 0

typedef std::string str;
typedef std::vector<std::string>::const_iterator string_it;
typedef std::istreambuf_iterator<char> bufIt;

class Directive {
	private:
		std::string name;
		std::vector<std::string> values;
		std::vector<Directive> children;

	public:
		// Constructors
		Directive() {}
		Directive(const char *config_file);
		Directive(const std::string& n) { name = n;}
		Directive &operator=(const Directive& other) {
			if (this != &other) {
				name = other.name; values = other.values;
				children = other.children; }
			return *this; }

		// Setters
		void	setName(const std::string& n) { name = n; }
		void	addValue(const std::string& value) { values.push_back(value); }
		void	addChild(const Directive& child) { children.push_back(child); }

		// Getters
		const	std::string& getName() const { return name; }
		const	std::vector<std::string> &getValues() const { return values; }
		const	std::vector<Directive> &getChildren() const { return children; }

		// parser
		void				PrintDirective(int indent) const ;
		static	Directive	DirectiveBuilder(string_it &it, const string_it &end);
};

class ConfigException : public std::exception
{
	private:
		std::string _msg;

	public:
		ConfigException(const std::string& msg) : _msg("Config File Syntax Error: " + msg) {}
		virtual ~ConfigException() throw() {}
		virtual const char* what() const throw() { return _msg.c_str();}
};

enum NextState {
	EXPECT_DIRECTIVE,
	EXPECT_TERMINATOR_OR_VALUE
};

extern int serverRunning;
void	initSignals();
void	handler(int signum);


#endif