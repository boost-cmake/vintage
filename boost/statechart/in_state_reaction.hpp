#ifndef BOOST_STATECHART_IN_STATE_REACTION_HPP_INCLUDED
#define BOOST_STATECHART_IN_STATE_REACTION_HPP_INCLUDED
//////////////////////////////////////////////////////////////////////////////
// (c) Copyright Andreas Huber Doenni 2005
// Distributed under the Boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////



#include <boost/statechart/result.hpp>

#include <boost/cast.hpp> // boost::polymorphic_downcast



namespace boost
{
namespace statechart
{



//////////////////////////////////////////////////////////////////////////////
template< class Event, 
          class ReactionContext,
          void ( ReactionContext::*pAction )( const Event & ) >
class in_state_reaction
{
  public:
    //////////////////////////////////////////////////////////////////////////
    // The following declarations should be private.
    // They are only public because many compilers lack template friends.
    //////////////////////////////////////////////////////////////////////////
    template< class State, class EventBase, class IdType >
    static detail::reaction_result react(
      State & stt, const EventBase & evt, const IdType & eventType )
    {
      if ( eventType == Event::static_type() )
      {
        ( stt.template context< ReactionContext >().*pAction )(
          *polymorphic_downcast< const Event * >( &evt ) );
        return detail::do_discard_event;
      }
      else
      {
        return detail::no_reaction;
      }
    }
};



} // namespace statechart
} // namespace boost



#endif