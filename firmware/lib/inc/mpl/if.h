#ifndef __MPL_IF_H
#define __MPL_IF_H

namespace mpl {

	//Selects left or right type depending on the condition
	template<bool condition, typename type_left, typename type_right>
	struct if_c
	{
		typedef type_left type;
	};

	template<typename type_left, typename type_right>
	struct if_c<false, type_left, type_right>
	{
		 typedef type_right type;
	};

	//The condition is a metafunction that should have the "value" member
	template<typename Condition, typename T1, typename T2>
	struct if_
	{
	 private:
	    typedef if_c<
			static_cast<bool>(Condition::value), 
			T1, 
			T2
		> almost_type_;
	 
	 public:
	    typedef typename almost_type_::type type;
	};
}

#endif //__MPL_IF_H