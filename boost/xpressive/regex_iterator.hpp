///////////////////////////////////////////////////////////////////////////////
/// \file regex_iterator.hpp
/// Contains the definition of the regex_iterator type, an STL-compatible iterator
/// for stepping through all the matches in a sequence.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_REGEX_ITERATOR_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_REGEX_ITERATOR_HPP_EAN_10_04_2005

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/detail/core/access.hpp>

namespace boost { namespace xpressive { namespace detail
{

//////////////////////////////////////////////////////////////////////////
// regex_iterator_impl
//
template<typename BidiIterT>
struct regex_iterator_impl
  : private noncopyable
{
    regex_iterator_impl
    (
        BidiIterT begin
      , BidiIterT cur
      , BidiIterT end
      , basic_regex<BidiIterT> const *rex
      , regex_constants::match_flag_type flags
      , bool not_null = false
    )
      : what_()
      , state_(begin, end, what_, *rex, flags)
      , rex_(rex)
      , flags_(flags)
      , not_null_(not_null)
    {
        this->state_.cur_ = cur;
    }

    bool next()
    {
        this->state_.reset(this->what_, *this->rex_);
        if(!regex_search_impl(this->state_, *this->rex_, this->not_null_))
        {
            return false;
        }

        // Report position() correctly by setting the base different from prefix().first
        detail::core_access<BidiIterT>::set_base(this->what_, this->state_.begin_);

        this->state_.cur_ = this->what_[0].second;
        this->not_null_ = (0 == this->what_.length());

        return true;
    }

    bool equal_to(regex_iterator_impl<BidiIterT> const &that) const
    {
        return this->rex_           == that.rex_
            && this->state_.begin_  == that.state_.begin_
            && this->state_.cur_    == that.state_.cur_
            && this->state_.end_    == that.state_.end_
            && this->flags_         == that.flags_
            ;
    }

    match_results<BidiIterT> what_;
    state_type<BidiIterT> state_;
    basic_regex<BidiIterT> const *const rex_;
    regex_constants::match_flag_type const flags_;
    bool not_null_;
};

} // namespace detail

//////////////////////////////////////////////////////////////////////////
// regex_iterator
//
template<typename BidiIterT>
struct regex_iterator
{
    typedef basic_regex<BidiIterT> regex_type;
    typedef match_results<BidiIterT> value_type;
    typedef typename iterator_difference<BidiIterT>::type difference_type;
    typedef value_type const *pointer;
    typedef value_type const &reference;
    typedef std::forward_iterator_tag iterator_category;

    /// INTERNAL ONLY
    typedef detail::regex_iterator_impl<BidiIterT> impl_type_;

    regex_iterator()
      : impl_()
    {
    }

    regex_iterator
    (
        BidiIterT begin
      , BidiIterT end
      , basic_regex<BidiIterT> const &rex
      , regex_constants::match_flag_type flags = regex_constants::match_default
    )
      : impl_(new impl_type_(begin, begin, end, &rex, flags))
    {
        this->next_();
    }

    regex_iterator(regex_iterator<BidiIterT> const &that)
      : impl_(that.impl_) // COW
    {
    }

    regex_iterator<BidiIterT> &operator =(regex_iterator<BidiIterT> const &that)
    {
        this->impl_ = that.impl_; // COW
        return *this;
    }

    friend bool operator ==(regex_iterator<BidiIterT> const &left, regex_iterator<BidiIterT> const &right)
    {
        if(!left.impl_ || !right.impl_)
        {
            return !left.impl_ && !right.impl_;
        }

        return left.impl_->equal_to(*right.impl_);
    }

    friend bool operator !=(regex_iterator<BidiIterT> const &left, regex_iterator<BidiIterT> const &right)
    {
        return !(left == right);
    }

    value_type const &operator *() const
    {
        return this->impl_->what_;
    }

    value_type const *operator ->() const
    {
        return &this->impl_->what_;
    }

    /// If what.prefix().first != what[0].second and if the element match_prev_avail is not set in
    /// flags then sets it. Then behaves as if by calling regex_search(what[0].second, end, what, *pre, flags),
    /// with the following variation: in the event that the previous match found was of zero length
    /// (what[0].length() == 0) then attempts to find a non-zero length match starting at what[0].second,
    /// only if that fails and provided what[0].second != suffix().second does it look for a (possibly
    /// zero length) match starting from what[0].second + 1.  If no further match is found then sets
    /// *this equal to the end of sequence iterator.
    /// \post (*this)-\>size() == pre-\>mark_count()
    /// \post (*this)-\>empty() == false
    /// \post (*this)-\>prefix().first == An iterator denoting the end point of the previous match found
    /// \post (*this)-\>prefix().last == (**this)[0].first
    /// \post (*this)-\>prefix().matched == (*this)-\>prefix().first != (*this)-\>prefix().second
    /// \post (*this)-\>suffix().first == (**this)[0].second
    /// \post (*this)-\>suffix().last == end
    /// \post (*this)-\>suffix().matched == (*this)-\>suffix().first != (*this)-\>suffix().second
    /// \post (**this)[0].first == The starting iterator for this match.
    /// \post (**this)[0].second == The ending iterator for this match.
    /// \post (**this)[0].matched == true if a full match was found, and false if it was a partial match (found as a result of the match_partial flag being set).
    /// \post (**this)[n].first == For all integers n \< (*this)-\>size(), the start of the sequence that matched sub-expression n. Alternatively, if sub-expression n did not participate in the match, then end.
    /// \post (**this)[n].second == For all integers n \< (*this)-\>size(), the end of the sequence that matched sub-expression n. Alternatively, if sub-expression n did not participate in the match, then end.
    /// \post (**this)[n].matched == For all integers n \< (*this)-\>size(), true if sub-expression n participated in the match, false otherwise.
    /// \post (*this)-\>position() == The distance from the start of the original sequence being iterated, to the start of this match.
    regex_iterator<BidiIterT> &operator ++()
    {
        this->fork_(); // un-share the implementation
        this->next_();
        return *this;
    }

    regex_iterator<BidiIterT> operator ++(int)
    {
        regex_iterator<BidiIterT> tmp(*this);
        ++*this;
        return tmp;
    }

private:

    /// INTERNAL ONLY
    void fork_()
    {
        if(!this->impl_.unique())
        {
            this->impl_.reset
            (
                new impl_type_
                (
                    this->impl_->state_.begin_
                  , this->impl_->state_.cur_
                  , this->impl_->state_.end_
                  , this->impl_->rex_
                  , this->impl_->flags_
                  , this->impl_->not_null_
                )
            );
        }
    }

    /// INTERNAL ONLY
    void next_()
    {
        BOOST_ASSERT(this->impl_ && this->impl_.unique());
        if(!this->impl_->next())
        {
            this->impl_.reset();
        }
    }

    shared_ptr<impl_type_> impl_;
};

}} // namespace boost::xpressive

#endif