#ifndef MAIN_HPP
# define MAIN_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>

typedef std::string str;

class Directive {
	private:
		str name;
		std::vector<str> values;
		std::vector<Directive> children;

	public:
		// Constructors
		Directive() {}
		Directive(const str& n) { name = n;}

		// Setters
		void	setName(const str& n) {name = n;}
		void	addValue(const str& value) { values.push_back(value); }
		void	addChild(const Directive& child) { children.push_back(child); }

		// Getters
		const str& getName() const { return name; }
		const std::vector<str> &getValues() const { return values; }
		const std::vector<Directive> &getChildren() const { return children; }

		// parser
		static	Directive *AST_Constructor(const str &config_file);
};


#endif