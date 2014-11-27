// Error.h

#ifndef _JUST_LIVE_ERROR_H_
#define _JUST_LIVE_ERROR_H_

namespace just
{
    namespace live
    {

        namespace error {

            enum errors
            {
                start_failed, 
            };

            namespace detail {

                class live_category
                    : public boost::system::error_category
                {
                public:
                    const char* name() const
                    {
                        return "live";
                    }

                    std::string message(int value) const
                    {
                        switch (value) {
                            case start_failed:
                                return "Live start faield";
                            default:
                                return "Live other error";
                        }
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::live_category instance;
                return instance;
            }

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

        } // namespace live_error

    } // namespace live
} // namespace just

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<just::live::error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using just::live::error::make_error_code;
#endif

    }
}

#endif // _JUST_PPAP_ERROR_H_
