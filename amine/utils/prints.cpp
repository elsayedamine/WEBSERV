#include "../Configuration/Directive.hpp"

void    Directive::PrintDirective(int indent = 0) const
{
    // create padding
    std::string pad;
    for (int i = 0; i < indent * 2; ++i)
        pad += "  ";

    // print directive name
    std::cout << pad << this->getName();

    // print values
    const std::vector<std::string> &values = this->getValues();
    for (size_t i = 0; i < values.size(); ++i)
        std::cout << " " << values[i];

    // if there are children, open a block
    std::vector<Directive> children = this->getChildren();
    if (!children.empty())
    {
        std::cout << std::endl;
        for (size_t i = 0; i < children.size(); ++i)
            children[i].PrintDirective(indent + 1);
    }
    else
        std::cout << std::endl;
}
