#include <mpl/type_list.h>
#include <mpl/fold.h>
#include <mpl/map.h>
#include <mpl/filter.h>

using namespace mpl;

template<int> struct CompileTimeError;
template<> struct CompileTimeError<true> {};

#define STATIC_CHECK(expr, msg) { CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; }

struct test_map {
    typedef make_type_list<int, float, double>::type types;

    template <typename T>
    struct make_pointer {
        typedef T* type;
    };

    typedef map<types, make_pointer>::type pointer_types;

    typedef pointer_types::head type1;
    typedef pointer_types::tail::head type2;

    type1 a;
    type2 b;

    void test() {
        STATIC_CHECK(sizeof(type1) == sizeof(int*), Type_Is_Not_A_Pointer_Type);
        STATIC_CHECK(sizeof(*a) == sizeof(int), IntegerTypeExpected);

        STATIC_CHECK(sizeof(type2) == sizeof(float*), TypeIsNotAPointerType);
        STATIC_CHECK(sizeof(*b) == sizeof(float), FloatTypeExpected);
    }
};


struct test_filter {
    template <int flag>
    struct tag {
        static const int value = flag;
    };

    typedef make_type_list<tag<1>, tag<2>, tag<3> >::type types;

    template <typename T>
    struct predicate {
        static const bool value = T::value > 1;
    };

    typedef filter<types, predicate>::type filtered_types;

    typedef filtered_types::head type1;
    typedef filtered_types::tail::head type2;

    int a[type1::value];
    int b[type2::value];

    void test() {
        STATIC_CHECK(sizeof(a) == sizeof(int[2]), Second_item_is_expected);
        STATIC_CHECK(sizeof(b) == sizeof(int[3]), Third_item_is_expected);
    }
};

struct test_fold {
    template <int flag>
    struct tag {
        static const int value = flag;
    };

    typedef make_type_list<tag<1>, tag<2>, tag<3> >::type types;

    template <typename State, typename T>
    struct sum {
        typedef tag<State::value + T::value> type;
    };

    typedef fold<types, tag<0>, sum>::type folded_type;

    int a[folded_type::value];

    void test() {
        STATIC_CHECK(sizeof(a) == sizeof(int[6]), Expected_sum_is_6);
    }
};

struct test_foldr {
	template <int flag>
	struct tag {
		static const int value = flag;
	};

	typedef make_type_list<tag<1>, tag<2>, tag<3> >::type types;

	template <typename State, typename T>
	struct sum {
		typedef tag<State::value + T::value> type;
	};

	typedef foldr<types, tag<0>, sum>::type folded_type;

	int a[folded_type::value];

	void test() {
		STATIC_CHECK(sizeof(a) == sizeof(int[6]), Expected_sum_is_6);
	}
};

int main()
{
    return 0;
}