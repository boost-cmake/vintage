/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "mini_cd.hpp"

// This is not really called. Its only purpose is to
// instantiate the constructor of the grammar.
void instantiate_program()
{
    typedef std::string::const_iterator iterator_type;
    std::vector<int> code;
    program<iterator_type> g(code);
}
