#ifndef __FSM_STATE_MACHINE_H
#define __FSM_STATE_MACHINE_H

#include <mpl/type_list.h>
#include <mpl/fold.h>
#include <mpl/filter.h>
#include <mpl/is_same.h>



namespace fsm {
	template<typename Derived>
	class state_machine;

    namespace details {
        template<typename Transition, typename Next>
        struct event_dispatcher
        {
            typedef typename Transition::fsm_t fsm_t;
            typedef typename Transition::event event;

            static int dispatch(fsm_t& fsm, int state, const event& e)
            {
                if (state == Transition::current_state) {
                    Transition::execute(fsm, e);
                    return Transition::next_state;
                }
                else {
                    // move on to the next node in the chain.
                    return Next::dispatch(fsm, state, e);
                }
            }
        };

        struct default_event_dispatcher
        {
            template<typename FSM, typename Event>
            static int dispatch(state_machine<FSM> & m, int state, const Event & e)
            {
                return m.call_no_transition(state, e);
            }
        };

        template<typename Transition>
        struct transition_event
        {
            typedef typename Transition::event type;
        };

        //Filtering ports by port name
        template<typename Type>
        struct is_same_filter {
            template <typename Item>
            struct predicate {
                static const bool value = mpl::is_same<Type, typename transition_event<Item>::type>::value;
            };
        };

        //Defines a mask generator for the pin based on appropriated configured flag
        //from the pin configuration
        template <template <typename, typename> class Dispatcher>
        struct dispatch_generator {
            template<typename State, typename Item>
            struct dispatch {
				typedef Dispatcher<Item, State> type;
            };
        };


        template<typename Table, typename Event>
        struct generate_dispatcher
			: mpl::fold<
               typename mpl::filter<
                   Table,
                   is_same_filter<Event>::template predicate
               >::type,
               default_event_dispatcher,
               dispatch_generator<event_dispatcher>::template dispatch
            >
        {};
	}

    template<class Derived>
    class state_machine
    {
    private:
        int state;

    protected:
        //Transition with an action
        template<
            int CurrentState,
            typename Event,
            int NextState,
            void (Derived::*action)(const Event&)
        >
        struct row
        {
            // for later use by our metaprogram
            static int const current_state = CurrentState;
            static int const next_state = NextState;
            typedef Event event;
            typedef Derived fsm_t;

            // do the transition action
            static void execute(Derived& fsm, const Event& e)
            {
                (fsm.*action) (e);
            }
        };

        //Transition without an action
        template<
            int CurrentState,
            typename Event,
            int NextState
        >
        struct srow
        {
            // for later use by our metaprogram
            static int const current_state = CurrentState;
            static int const next_state = NextState;
            typedef Event event;
            typedef Derived fsm_t;

            // do the transition action
            static void execute(Derived& fsm, const Event& e)
            {
            }
        };

        template<typename Event>
        int no_transition(int state, const Event & e)
        {
            return this->state;
        }

        state_machine()
            : state(Derived::initial_state)
        {
        }

    public:
        template<typename Event>
        int process_event(const Event & e)
        {
            typedef typename details::generate_dispatcher<typename Derived::transition_table, Event>::type dispatcher;

            this->state = dispatcher::dispatch(*static_cast<Derived *>(this), this->state, e);

            return this->state;
        }


        template<typename Event>
        int call_no_transition(int state, const Event & e)
        {
            return static_cast<Derived *>(this)->no_transition(state, e);
        }
    };

}


#endif //__FSM_STATE_MACHINE_H