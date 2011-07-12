// Error.h

#ifndef _PPBOX_LIVE_ERROR_H_
#define _PPBOX_LIVE_ERROR_H_

namespace ppbox
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
} // namespace ppbox

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<ppbox::live::error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using ppbox::live::error::make_error_code;
#endif

    }
}

#endif // _PPBOX_PPAP_ERROR_H_
