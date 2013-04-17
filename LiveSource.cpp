// LiveSource.cpp

#include "ppbox/live/Common.h"
#include "ppbox/live/LiveSource.h"

#include <ppbox/cdn/PptvMedia.h>

#include <ppbox/demux/base/DemuxEvent.h>
#include <ppbox/demux/segment/SegmentDemuxer.h>

#include <ppbox/merge/MergerBase.h>

#include <ppbox/data/segment/SegmentSource.h>

#include <util/protocol/pptv/Base64.h>
using namespace util::protocol;

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/string/Format.h>
using namespace framework::string;

#include <boost/bind.hpp>

namespace ppbox
{
    namespace live
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.live.LiveSource", framework::logger::Debug);

        LiveSource::LiveSource(
            boost::asio::io_service & io_svc)
            : ppbox::cdn::P2pSource(io_svc)
            , module_(util::daemon::use_module<LiveModule>(io_svc))
        {
        }

        LiveSource::~LiveSource()
        {
        }

        boost::system::error_code LiveSource::open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            boost::system::error_code & ec)
        {
            LOG_DEBUG("[open] url:"<<url.to_string()
                <<" range: "<< beg << " - " << end);

            if (beg > 0) {
                // 不能断点续传
                return ec = framework::system::logic_error::not_supported;
            }
            framework::string::Url peer_url;
            make_url(url, peer_url);
            return HttpSource::open(peer_url, beg, end, ec);
        }

        void LiveSource::async_open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            response_type const & resp)
        {
            LOG_DEBUG("[async_open] url:"<<url.to_string()
                <<" range: "<< beg << " - " << end);

            if (beg > 0) {
                get_io_service().post(boost::bind(resp, framework::system::logic_error::not_supported));
                return;
            }
            framework::string::Url peer_url;
            make_url(url, peer_url);
            HttpSource::async_open(peer_url, beg, end, resp);
        }

        boost::system::error_code LiveSource::close(
            boost::system::error_code & ec)
        {
            open_log(true);
            return HttpSource::close(ec);
        }

        boost::uint64_t LiveSource::total(
            boost::system::error_code & ec)
        {
            ec.clear();
            return boost::uint64_t(0x8000000000000000ULL); // 一个非常大的数值，假设永远下载不完
        }

        void LiveSource::on_event(
            util::event::Event const & e)
        {
            ppbox::demux::BufferingEvent const & event = *e.as<ppbox::demux::BufferingEvent>();
            (void)event;
        }

        void LiveSource::parse_param(
            std::string const & params)
        {
            const_cast<ppbox::data::SegmentSource &>(seg_source()).set_time_out(0);
        }

        boost::system::error_code LiveSource::make_url(
            framework::string::Url const & cdn_url,
            framework::string::Url & url)
        {
            LOG_DEBUG("Use vod worker, BWType: " << pptv_media().jump().bw_type);

            url.protocol("http");
            url.host("127.0.0.1");
            url.svc(format(module_.port()));

            std::string url_str = cdn_url.param("url");
            framework::string::Url::param_const_iterator iter = cdn_url.param_begin();
            for (; iter != cdn_url.param_end(); ++iter) {
                framework::string::Url::Parameter const & p = *iter;
                if (p.key() != "url") {
                    url_str += "&";
                    url_str += p.to_string();
                }
            }

            std::string key = "pplive";
            url.path("/" + pptv::base64_encode(url_str, key));

            open_log(false);

            return boost::system::error_code();
        }

    } // namespace live
} // namespace ppbox
