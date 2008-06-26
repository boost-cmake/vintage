// cpp_code.hpp
// Copyright (c) 2008 Ben Hanson
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_LEXER_EXAMPLE_CPP_CODE_HPP
#define BOOST_LEXER_EXAMPLE_CPP_CODE_HPP

#include "char_traits.hpp"
#include "consts.hpp"
#include <iostream>
#include <boost/detail/iterator.hpp>
#include "runtime_error.hpp"
#include "size_t.hpp"
#include "state_machine.hpp"
#include <vector>

namespace boost
{
namespace lexer
{
void generate_cpp (const state_machine &sm_, std::ostream &os_,
    const bool use_pointers_ = false, const bool skip_unknown_ = true,
    const bool optimise_parameters_ = true, const char *name_ = "next_token")
{
    if (sm_._lookup->size () == 0)
    {
        throw runtime_error ("Cannot generate code from an empty state machine");
    }

    std::string upper_name_ (__DATE__);
    const std::size_t lookups_ = sm_._lookup->front ()->size ();
    const std::size_t dfas_ = sm_._dfa->size ();
    std::string::size_type pos_ = upper_name_.find (' ');
    const char *iterator_ = 0;

    if (use_pointers_)
    {
        if (lookups_ == 256)
        {
            iterator_ = "const char *";
        }
        else
        {
            iterator_ = "const wchar_t *";
        }
    }
    else
    {
        iterator_ = "Iterator &";
    }

    while (pos_ != std::string::npos)
    {
        upper_name_.replace (pos_, 1, "_");
        pos_ = upper_name_.find (' ', pos_);
    }

    upper_name_ += '_';
    upper_name_ +=  __TIME__;

    pos_ = upper_name_.find (':');

    while (pos_ != std::string::npos)
    {
        upper_name_.erase (pos_, 1);
        pos_ = upper_name_.find (':', pos_);
    }

    upper_name_ = '_' + upper_name_;
    upper_name_ = name_ + upper_name_;
    std::transform (upper_name_.begin (), upper_name_.end (),
        upper_name_.begin (), ::toupper);
    os_ << "#ifndef " << upper_name_ + '\n';
    os_ << "#define " << upper_name_ + '\n';
    os_ << "// Copyright (c) 2008 Ben Hanson\n";
    os_ << "//\n";
    os_ << "// Distributed under the Boost Software License, "
        "Version 1.0. (See accompanying\n";
    os_ << "// file licence_1_0.txt or copy at "
        "http://www.boost.org/LICENSE_1_0.txt)\n\n";
    os_ << "// Auto-generated by boost::lexer\n";
    os_ << "template<typename Iterator>\n";
    os_ << "std::size_t " << name_  << " (";

    if (dfas_ > 1 || !optimise_parameters_)
    {
        os_ << "std::size_t &start_state_, ";
    }

    if (sm_._seen_BOL_assertion || !optimise_parameters_)
    {
        if (use_pointers_)
        {
            os_ << iterator_ << " const ";
        }
        else
        {
            os_ << "const " << iterator_;
        }

        os_ << "start_, ";
    }

    if (dfas_ > 1 || sm_._seen_BOL_assertion || !optimise_parameters_)
    {
        os_ << "\n    ";
    }

    if (use_pointers_)
    {
        os_ << iterator_ << " &";
    }
    else
    {
        os_ << iterator_;
    }

    os_ << "start_token_, ";

    if (use_pointers_)
    {
        os_ << iterator_ << " const ";
    }
    else
    {
        os_ << "const " << iterator_;
    }

    os_ << "end_)\n";
    os_ << "{\n";
    os_ << "    enum {end_state_index, id_index, state_index, bol_index, "
        "eol_index,\n";
    os_ << "        dead_state_index, dfa_offset};\n";
    os_ << "    static const std::size_t npos = static_cast"
        "<std::size_t>(~0);\n";

    if (dfas_ > 1)
    {
        std::size_t state_ = 0;

        for (; state_ < dfas_; ++state_)
        {
            std::size_t i_ = 0;
            std::size_t j_ = 1;
            std::size_t count_ = lookups_ / 8;
            const std::size_t *lookup_ = &sm_._lookup[state_]->front ();
            const std::size_t *dfa_ = &sm_._dfa[state_]->front ();

            os_ << "    static const std::size_t lookup" << state_ << "_[" <<
                lookups_ << "] = {";

            for (; i_ < count_; ++i_)
            {
                const std::size_t index_ = i_ * 8;

                os_ << lookup_[index_];

                for (; j_ < 8; ++j_)
                {
                    os_ << ", " << lookup_[index_ + j_];
                }

                if (i_ < count_ - 1)
                {
                    os_ << "," << std::endl << "        ";
                }

                j_ = 1;
            }

            os_ << "};\n";
            count_ = sm_._dfa[state_]->size ();
            os_ << "    static const std::size_t dfa" << state_ << "_[" <<
                count_ << "] = {";
            count_ /= 8;

            for (i_ = 0; i_ < count_; ++i_)
            {
                const std::size_t index_ = i_ * 8;

                os_ << dfa_[index_];

                for (j_ = 1; j_ < 8; ++j_)
                {
                    os_ << ", " << dfa_[index_ + j_];
                }

                if (i_ < count_ - 1)
                {
                    os_ << "," << std::endl << "        ";
                }
            }

            const std::size_t mod_ = sm_._dfa[state_]->size () % 8;

            if (mod_)
            {
                const std::size_t index_ = count_ * 8;

                if (count_)
                {
                    os_ << ",\n        ";
                }

                os_ << dfa_[index_];

                for (j_ = 1; j_ < mod_; ++j_)
                {
                    os_ << ", " << dfa_[index_ + j_];
                }
            }

            os_ << "};\n";
        }

        std::size_t count_ = sm_._dfa_alphabet.size ();
        std::size_t i_ = 1;

        os_ << "    static const std::size_t *lookup_arr_[" << count_ <<
            "] = {";
        os_ << "lookup0_";

        for (i_ = 1; i_ < count_; ++i_)
        {
            os_ << ", " << "lookup" << i_ << "_";
        }

        os_ << "};\n";
        os_ << "    static const std::size_t dfa_alphabet_arr_[" << count_ <<
            "] = {";
        os_ << sm_._dfa_alphabet.front ();

        for (i_ = 1; i_ < count_; ++i_)
        {
            os_ << ", " << sm_._dfa_alphabet[i_];
        }

        os_ << "};\n";
        os_ << "    static const std::size_t *dfa_arr_[" << count_ <<
            "] = {";
        os_ << "dfa0_";

        for (i_ = 1; i_ < count_; ++i_)
        {
            os_ << ", " << "dfa" << i_ << "_";
        }

        os_ << "};\n";
    }
    else
    {
        const std::size_t *lookup_ = &sm_._lookup[0]->front ();
        const std::size_t *dfa_ = &sm_._dfa[0]->front ();
        std::size_t i_ = 0;
        std::size_t j_ = 1;
        std::size_t count_ = lookups_ / 8;

        os_ << "    static const std::size_t lookup_[";
        os_ << sm_._lookup[0]->size () << "] = {";

        for (; i_ < count_; ++i_)
        {
            const std::size_t index_ = i_ * 8;

            os_ << lookup_[index_];

            for (; j_ < 8; ++j_)
            {
                os_ << ", " << lookup_[index_ + j_];
            }

            if (i_ < count_ - 1)
            {
                os_ << "," << std::endl << "        ";
            }

            j_ = 1;
        }

        os_ << "};\n";
        os_ << "    static const std::size_t dfa_alphabet_ = " <<
            sm_._dfa_alphabet.front () << ";\n";
        os_ << "    static const std::size_t dfa_[" <<
            sm_._dfa[0]->size () << "] = {";
        count_ = sm_._dfa[0]->size () / 8;

        for (i_ = 0; i_ < count_; ++i_)
        {
            const std::size_t index_ = i_ * 8;

            os_ << dfa_[index_];

            for (j_ = 1; j_ < 8; ++j_)
            {
                os_ << ", " << dfa_[index_ + j_];
            }

            if (i_ < count_ - 1)
            {
                os_ << "," << std::endl << "        ";
            }
        }

        const std::size_t mod_ = sm_._dfa[0]->size () % 8;

        if (mod_)
        {
            const std::size_t index_ = count_ * 8;

            if (count_)
            {
                os_ << ",\n        ";
            }

            os_ << dfa_[index_];

            for (j_ = 1; j_ < mod_; ++j_)
            {
                os_ << ", " << dfa_[index_ + j_];
            }
        }

        os_ << "};\n";
    }

    os_ << "\n    if (start_token_ == end_) return 0;\n\n";

    if (dfas_ > 1)
    {
        os_ << "again:\n";
        os_ << "    const std::size_t * lookup_ = lookup_arr_[start_state_];\n";
        os_ << "    std::size_t dfa_alphabet_ = dfa_alphabet_arr_[start_state_];\n";
        os_ << "    const std::size_t *dfa_ = dfa_arr_[start_state_];\n";
    }

    os_ << "    const std::size_t *ptr_ = dfa_ + dfa_alphabet_;\n";
    os_ << "    Iterator curr_ = start_token_;\n";
    os_ << "    bool end_state_ = *ptr_ != 0;\n";
    os_ << "    std::size_t id_ = *(ptr_ + id_index);\n";
    os_ << "    Iterator end_token_ = start_token_;\n";
    os_ << '\n';
    os_ << "    while (curr_ != end_)\n";
    os_ << "    {\n";

    if (sm_._seen_BOL_assertion)
    {
        os_ << "        const std::size_t BOL_state_ = ptr_[bol_index];\n";
    }

    if (sm_._seen_EOL_assertion)
    {
        os_ << "        const std::size_t EOL_state_ = ptr_[eol_index];\n";
    }

    if (sm_._seen_BOL_assertion || sm_._seen_EOL_assertion)
    {
        os_ << '\n';
    }

    if (sm_._seen_BOL_assertion && sm_._seen_EOL_assertion)
    {
        os_ << "        if (BOL_state_ && (start_token_ == start_ ||\n";
        os_ << "            *(start_token_ - 1) == '\\n'))\n";
        os_ << "        {\n";
        os_ << "            ptr_ = &dfa_[BOL_state_ * dfa_alphabet_];\n";
        os_ << "        }\n";
        os_ << "        else if (EOL_state_ && *curr_ == '\\n')\n";
        os_ << "        {\n";
        os_ << "            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];\n";
        os_ << "        }\n";
        os_ << "        else\n";
        os_ << "        {\n";
        os_ << "            const std::size_t state_ =\n";

        if (lookups_ == 256)
        {
            os_ << "                ptr_[lookup_[static_cast<unsigned char>\n";
            os_ << "                (*curr_++)]];\n";
        }
        else
        {
            os_ << "                ptr_[lookup_[*curr_++]];\n";
        }

        os_ << '\n';
        os_ << "            if (state_ == 0) break;\n";
        os_ << '\n';
        os_ << "            ptr_ = &dfa_[state_ * dfa_alphabet_];\n";
        os_ << "        }\n";
    }
    else if (sm_._seen_BOL_assertion)
    {
        os_ << "        if (BOL_state_ && (start_token_ == start_ ||\n";
        os_ << "            *(start_token_ - 1) == '\\n'))\n";
        os_ << "        {\n";
        os_ << "            ptr_ = &dfa_[BOL_state_ * dfa_alphabet_];\n";
        os_ << "        }\n";
        os_ << "        else\n";
        os_ << "        {\n";
        os_ << "            const std::size_t state_ =\n";

        if (lookups_ == 256)
        {
            os_ << "                ptr_[lookup_[static_cast<unsigned char>\n";
            os_ << "                (*curr_++)]];\n";
        }
        else
        {
            os_ << "                ptr_[lookup_[*curr_++]];\n";
        }

        os_ << '\n';
        os_ << "            if (state_ == 0) break;\n";
        os_ << '\n';
        os_ << "            ptr_ = &dfa_[state_ * dfa_alphabet_];\n";
        os_ << "        }\n";
    }
    else if (sm_._seen_EOL_assertion)
    {
        os_ << "        if (EOL_state_ && *curr_ == '\\n')\n";
        os_ << "        {\n";
        os_ << "            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];\n";
        os_ << "        }\n";
        os_ << "        else\n";
        os_ << "        {\n";
        os_ << "            const std::size_t state_ =\n";

        if (lookups_ == 256)
        {
            os_ << "                ptr_[lookup_[static_cast<unsigned char>\n";
            os_ << "                (*curr_++)]];\n";
        }
        else
        {
            os_ << "                ptr_[lookup_[*curr_++]];\n";
        }

        os_ << '\n';
        os_ << "            if (state_ == 0) break;\n";
        os_ << '\n';
        os_ << "            ptr_ = &dfa_[state_ * dfa_alphabet_];\n";
        os_ << "        }\n";
    }
    else
    {
        os_ << "        const std::size_t state_ =\n";

        if (lookups_ == 256)
        {
            os_ << "            ptr_[lookup_[static_cast<unsigned char>\n";
            os_ << "            (*curr_++)]];\n";
        }
        else
        {
            os_ << "            ptr_[lookup_[*curr_++]];\n";
        }

        os_ << '\n';
        os_ << "        if (state_ == 0) break;\n";
        os_ << '\n';
        os_ << "        ptr_ = &dfa_[state_ * dfa_alphabet_];\n";
    }

    os_ << '\n';
    os_ << "        if (*ptr_)\n";
    os_ << "        {\n";
    os_ << "            end_state_ = true;\n";
    os_ << "            id_ = *(ptr_ + id_index);\n";

    if (dfas_ > 1)
    {
        os_ << "            start_state_ = *(ptr_ + state_index);\n";
    }

    os_ << "            end_token_ = curr_;\n";
    os_ << "        }\n";
    os_ << "    }\n";
    os_ << '\n';

    if (sm_._seen_EOL_assertion)
    {
        os_ << "    const std::size_t EOL_state_ = ptr_[eol_index];\n";
        os_ << '\n';
        os_ << "    if (EOL_state_ && curr_ == end_)\n";
        os_ << "    {\n";
        os_ << "        ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];\n";
        os_ << '\n';
        os_ << "        if (*ptr_)\n";
        os_ << "        {\n";
        os_ << "            end_state_ = true;\n";
        os_ << "            id_ = *(ptr_ + id_index);\n";

        if (dfas_ > 1)
        {
            os_ << "            start_state_ = *(ptr_ + state_index);\n";
        }

        os_ << "            end_token_ = curr_;\n";
        os_ << "        }\n";
        os_ << "    }\n";
        os_ << '\n';
    }

    os_ << "    if (end_state_)\n";
    os_ << "    {\n";
    os_ << "        // return longest match\n";
    os_ << "        start_token_ = end_token_;\n";

    if (dfas_ > 1)
    {
        os_ << '\n';
        os_ << "        if (id_ == 0) goto again;\n";
    }

    os_ << "    }\n";
    os_ << "    else\n";
    os_ << "    {\n";

    if (skip_unknown_)
    {
        os_ << "        // No match causes char to be skipped\n";
        os_ << "        ++start_token_;\n";
    }

    os_ << "        id_ = npos;\n";
    os_ << "    }\n";
    os_ << '\n';
    os_ << "    return id_;\n";
    os_ << "}\n";
    os_ << "\n#endif\n";
}
}
}

#endif