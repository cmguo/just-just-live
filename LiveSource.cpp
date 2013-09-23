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

        boost::uint64_t LiveSource::total(
            boost::system::error_code & ec)
        {
            ec.clear();
            return boost::uint64_t(0x8000000000000000ULL); // 一个非常大的数值，假设永远下载不完
        }

        void LiveSource::on_demux_stat(
            ppbox::demux::DemuxStatistic const & stat)
        {
            (void)stat;
        }

        void LiveSource::parse_param(
            std::string const & params)
        {
            const_cast<ppbox::data::SegmentSource &>(seg_source()).set_time_out(0);
        }

        bool LiveSource::prepare(
            framework::string::Url & url, 
            boost::uint64_t & beg, 
            boost::uint64_t & end, 
            boost::system::error_code & ec)
        {
            LOG_DEBUG("Use live worker, BWType: " << pptv_media().jump().bw_type);

            if (beg > 0) {
                // 不能断点续传
                ec = framework::system::logic_error::not_supported;
                return false;
            }

            std::string url_str = url.param("url");
            framework::string::Url::param_const_iterator iter = url.param_begin();
            for (; iter != url.param_end(); ++iter) {
                framework::string::Url::Parameter const & p = *iter;
                if (p.key() != "url") {
                    url_str += "&";
                    url_str += p.to_string();
                }
            }

            url = framework::string::Url();
            url.protocol("http");
            url.host("127.0.0.1");
            url.svc(format(module_.port()));

            std::string key = "pplive";
            url.path("/" + pptv::base64_encode(url_str, key));

            ec = boost::system::error_code();
            return true;
        }

    } // namespace live
} // namespace ppbox
