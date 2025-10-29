#include "main.hpp"

Directive   *Directive::AST_Constructor(const str &conf)
{
	std::ifstream file(conf);
	std::stringstream buffer;
	str	content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

}
