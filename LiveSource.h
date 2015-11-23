// LiveSource.h

#ifndef JUST_PEER_PEER_SOURCE_H_
#define JUST_PEER_PEER_SOURCE_H_

#include "just/live/LiveModule.h"

#include <just/cdn/pptv/PptvP2pSource.h>

namespace just
{
    namespace live
    {

        class LiveSource
            : public just::cdn::PptvP2pSource
        {
        public:
            LiveSource(
                boost::asio::io_service & io_svc);

            virtual ~LiveSource();

        public:
            virtual boost::uint64_t total(
                boost::system::error_code & ec);

        private:
            virtual void on_stream_status(
                just::avbase::StreamStatus const & stat);

            virtual void parse_param(
                std::string const & params);

            virtual bool prepare(
                framework::string::Url & url, 
                boost::uint64_t & beg, 
                boost::uint64_t & end, 
                boost::system::error_code & ec);

        protected:
            LiveModule & module_;
        };

        UTIL_REGISTER_URL_SOURCE("pplive", LiveSource);

    } // namespace peer
} // namespace just

#endif // JUST_PEER_PEER_SOURCE_H_
