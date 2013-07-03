// LiveSource.h

#ifndef PPBOX_PEER_PEER_SOURCE_H_
#define PPBOX_PEER_PEER_SOURCE_H_

#include "ppbox/live/LiveModule.h"

#include <ppbox/cdn/P2pSource.h>

namespace ppbox
{
    namespace live
    {

        class LiveSource
            : public ppbox::cdn::P2pSource
        {
        public:
            LiveSource(
                boost::asio::io_service & io_svc);

            virtual ~LiveSource();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            virtual void async_open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);

            virtual boost::system::error_code close(
                boost::system::error_code & ec);

            virtual boost::uint64_t total(
                boost::system::error_code & ec);

        private:
            virtual void on_demux_stat(
                ppbox::demux::DemuxStatistic const & stat) = 0;

            virtual void parse_param(
                std::string const & params);

            boost::system::error_code make_url(
                framework::string::Url const & cdn_url, 
                framework::string::Url & url);

        protected:
            LiveModule & module_;
        };

        PPBOX_REGISTER_URL_SOURCE("pplive", LiveSource);

    } // namespace peer
} // namespace ppbox

#endif // PPBOX_PEER_PEER_SOURCE_H_
