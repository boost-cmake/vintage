
// Copyright (C) 2003-2004 Jeremy B. Maitin-Shepard.
// Copyright (C) 2005-2009 Daniel James
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNORDERED_DETAIL_EQUIVALENT_HPP_INCLUDED
#define BOOST_UNORDERED_DETAIL_EQUIVALENT_HPP_INCLUDED

#include <boost/unordered/detail/table.hpp>
#include <boost/unordered/detail/extract_key.hpp>

namespace boost { namespace unordered_detail {

    ////////////////////////////////////////////////////////////////////////////
    // Equality

    template <class H, class P, class A, class K>
    bool hash_equivalent_table<H, P, A, K>
        ::equals(hash_equivalent_table<H, P, A, K> const& other) const
    {
        if(this->size_ != other.size_) return false;
        if(!this->size_) return true;

        bucket_ptr end = this->get_bucket(this->bucket_count_);
        for(bucket_ptr i = this->cached_begin_bucket_; i != end; ++i)
        {
            node_ptr it1 = i->next_;
            while(BOOST_UNORDERED_BORLAND_BOOL(it1))
            {
                node_ptr it2 = other.find_iterator(get_key_from_ptr(it1));
                if(!BOOST_UNORDERED_BORLAND_BOOL(it2)) return false;
                
                node_ptr end1 = node::next_group(it1);
                node_ptr end2 = node::next_group(it2);

                do {
                    if(!extractor::compare_mapped(
                        node::get_value(it1), node::get_value(it2)))
                        return false;
                    it1 = it1->next_;
                    it2 = it2->next_;
                } while(it1 != end1 && it2 != end2);
                if(it1 != end1 || it2 != end2) return false;
            }
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////
    // A convenience method for adding nodes.

    template <class H, class P, class A, class K>
    inline BOOST_DEDUCED_TYPENAME hash_equivalent_table<H, P, A, K>::node_ptr
        hash_equivalent_table<H, P, A, K>
            ::add_node(node_constructor& a, bucket_ptr bucket, node_ptr pos)
    {
        node_ptr n = a.release();
        if(BOOST_UNORDERED_BORLAND_BOOL(pos)) {
            node::add_after_node(n, pos);                
        }
        else {
            node::add_to_bucket(n, *bucket);
            if(bucket < this->cached_begin_bucket_)
                this->cached_begin_bucket_ = bucket;
        }
        ++this->size_;
        return n;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Insert methods

    template <class H, class P, class A, class K>
    inline BOOST_DEDUCED_TYPENAME
        hash_equivalent_table<H, P, A, K>::iterator_base
        hash_equivalent_table<H, P, A, K>::emplace_impl(node_constructor& a)
    {
        key_type const& k = get_key(a.value());
        std::size_t hash_value = this->hash_function()(k);
        
        if(!this->size_) {
            return this->emplace_empty_impl_with_node(a, 1);
        }
        else {
            bucket_ptr bucket = this->bucket_ptr_from_hash(hash_value);
            node_ptr position = find_iterator(bucket, k);

            // reserve has basic exception safety if the hash function
            // throws, strong otherwise.
            if(this->reserve_for_insert(this->size_ + 1))
                bucket = this->bucket_ptr_from_hash(hash_value);

            return iterator_base(bucket, add_node(a, bucket, position));
        }
    }
    
    template <class H, class P, class A, class K>
    inline void hash_equivalent_table<H, P, A, K>
            ::emplace_impl_no_rehash(node_constructor& a)
    {
        key_type const& k = get_key(a.value());
        bucket_ptr bucket = this->get_bucket(this->bucket_index(k));
        add_node(a, bucket, find_iterator(bucket, k));
    }

#if defined(BOOST_UNORDERED_STD_FORWARD)

    // Emplace (equivalent key containers)
    // (I'm using an overloaded emplace for both 'insert' and 'emplace')

    // if hash function throws, basic exception safety
    // strong otherwise
    template <class H, class P, class A, class K>
    template <class... Args>
    BOOST_DEDUCED_TYPENAME hash_equivalent_table<H, P, A, K>::iterator_base
        hash_equivalent_table<H, P, A, K>
            ::emplace(Args&&... args)
    {
        // Create the node before rehashing in case it throws an
        // exception (need strong safety in such a case).
        node_constructor a(*this);
        a.construct(std::forward<Args>(args)...);

        return emplace_impl(a);
    }

#else

#define BOOST_UNORDERED_INSERT_IMPL(z, num_params, _)                       \
    template <class H, class P, class A, class K>                           \
    template <BOOST_UNORDERED_TEMPLATE_ARGS(z, num_params)>                 \
    BOOST_DEDUCED_TYPENAME hash_equivalent_table<H, P, A, K>::iterator_base \
        hash_equivalent_table<H, P, A, K>                                   \
            ::emplace(BOOST_UNORDERED_FUNCTION_PARAMS(z, num_params))       \
    {                                                                       \
        node_constructor a(*this);                                          \
        a.construct(BOOST_UNORDERED_CALL_PARAMS(z, num_params));            \
        return emplace_impl(a);                                             \
    }

    BOOST_PP_REPEAT_FROM_TO(1, BOOST_UNORDERED_EMPLACE_LIMIT,
        BOOST_UNORDERED_INSERT_IMPL, _)

#undef BOOST_UNORDERED_INSERT_IMPL
#endif

    ////////////////////////////////////////////////////////////////////////////
    // Insert range methods

    // if hash function throws, or inserting > 1 element, basic exception safety
    // strong otherwise
    template <class H, class P, class A, class K>
    template <class I>
    inline void hash_equivalent_table<H, P, A, K>
        ::insert_for_range(I i, I j, forward_traversal_tag)
    {
        if(i == j) return;
        std::size_t distance = unordered_detail::distance(i, j);
        if(distance == 1) {
            emplace(*i);
        }
        else {
            node_constructor a(*this);

            // Only require basic exception safety here
            if(this->size_) {
                this->reserve_for_insert(this->size_ + distance);
            }
            else {
                a.construct(*i++);
                this->emplace_empty_impl_with_node(a, distance);
            }

            for (; i != j; ++i) {
                a.construct(*i);
                emplace_impl_no_rehash(a);
            }
        }
    }

    // if hash function throws, or inserting > 1 element, basic exception safety
    // strong otherwise
    template <class H, class P, class A, class K>
    template <class I>
    inline void hash_equivalent_table<H, P, A, K>
        ::insert_for_range(I i, I j, boost::incrementable_traversal_tag)
    {
        node_constructor a(*this);
        for (; i != j; ++i) {
            a.construct(*i);
            emplace_impl(a);
        }
    }

    // if hash function throws, or inserting > 1 element, basic exception safety
    // strong otherwise
    template <class H, class P, class A, class K>
    template <class I>
    void hash_equivalent_table<H, P, A, K>::insert_range(I i, I j)
    {
        BOOST_DEDUCED_TYPENAME boost::iterator_traversal<I>::type
            iterator_traversal_tag;
        insert_for_range(i, j, iterator_traversal_tag);
    }
}}

#endif
