// LiveModule.cpp

#include "just/live/Common.h"
#include "just/live/LiveModule.h"
#include "just/live/ClassRegister.h"

#ifndef JUST_DISABLE_DAC
#include <just/dac/DacModule.h>
#include <just/dac/DacInfoWorker.h>
using namespace just::dac;
#endif
//#include <just/live_worker/Version.h>
#include <just/live_worker/Name.h>


#ifdef JUST_CONTAIN_LIVE_WORKER
#include <just/live_worker/LiveProxy.h>
using namespace just::live_worker;
#else
#include <framework/process/Process.h>
#include <framework/timer/Timer.h>
using namespace framework::process;
using namespace framework::timer;
#endif

#include <framework/system/ErrorCode.h>
#include <framework/system/LogicError.h>
#include <framework/string/Format.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
using namespace framework::system;
using namespace framework::string;

#include <boost/bind.hpp>
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.live.LiveModule", framework::logger::Debug)

namespace just
{
    namespace live
    {

        LiveModule::LiveModule(
            util::daemon::Daemon & daemon)
            : just::common::CommonModuleBase<LiveModule>(daemon, "LiveModule")
#ifndef JUST_DISABLE_DAC
            , dac_(util::daemon::use_module<just::dac::DacModule>(daemon))
#endif
            , portMgr_(util::daemon::use_module<just::common::PortManager>(daemon))
            , port_(9001)
#ifndef JUST_CONTAIN_LIVE_WORKER
            , mutex_(9001)
            , is_locked_(false)
#endif
        {

#ifdef JUST_CONTAIN_LIVE_WORKER
            util::daemon::use_module<just::live_worker::LiveProxy>(daemon);
#else
            process_ = new Process;
            timer_ = new Timer(timer_queue(), 
                10, // 5 seconds
                boost::bind(&LiveModule::check, this));
#endif
        }

        LiveModule::~LiveModule()
        {
#ifndef JUST_CONTAIN_LIVE_WORKER
            if (is_lock()) {
                mutex_.unlock();
                is_locked_ = false;
            }
            if (process_) {
                delete process_;
                process_ = NULL;
            }
            if (timer_) {
                delete timer_;
                timer_ = NULL;
            }
#endif
        }

        bool LiveModule::startup(
            error_code & ec)
        {
            LOG_INFO("[startup]");
#ifndef JUST_DISABLE_DAC
            dac_.set_live_version(version());
            dac_.set_live_name(name());
#endif
#ifndef JUST_CONTAIN_LIVE_WORKER
            timer_->start();

            if (is_lock()) {
                LOG_INFO("[startup] try_lock");

                boost::filesystem::path cmd_file(just::live_worker::name_string());
                Process::CreateParamter param;
                param.wait = true;
                process_->open(cmd_file, param, ec);
                if (!ec) {
                    portMgr_.get_port(just::common::live,port_);
                    LOG_INFO("[startup] ok port:"<<port_);
                } else {
                    LOG_WARN("[startup] ec = " << ec.message());
                    //port_ = 0;
                    if (ec == boost::system::errc::no_such_file_or_directory) {
                        ec.clear();
                    }

                    timer_->stop();
                }
            }
#else
            portMgr_.get_port(just::common::live,port_);
            LOG_INFO("[startup] ok port:"<<port_);	
#endif
            return !ec;
        }

        void LiveModule::check()
        {
#ifndef JUST_CONTAIN_LIVE_WORKER
            error_code ec;
            if (is_lock()) {
                if (process_ && !process_->is_alive(ec)) {
                    LOG_ERROR("[check] worker is dead: " << ec.message());

#ifndef JUST_DISABLE_DAC
                    util::daemon::use_module<just::dac::DacModule>(get_daemon())
                        .submit(DacRestartInfo(DacRestartInfo::live));
#endif
                    process_->close(ec);
                    boost::filesystem::path cmd_file(just::live_worker::name_string());
                    Process::CreateParamter param;
                    param.wait = true;
                    process_->open(cmd_file, param, ec);
                    if (!ec) {
                        portMgr_.get_port(just::common::live,port_);
                        LOG_INFO("[check] ok port:"<<port_);
                    } else {
                        LOG_WARN("[check] ec = " << ec.message());
                        port_ = 0;

                        timer_->stop();
                    }
                }
            }
#endif
        }

        bool LiveModule::is_alive()
        {
            error_code ec;
#ifdef JUST_CONTAIN_LIVE_WORKER
            return true;
#else
            if (is_locked_) {
                return process_ && process_->is_alive(ec);
            } else {
                framework::process::Process process;
                boost::filesystem::path cmd_file(just::live_worker::name_string());
                process.open(cmd_file, ec);
                return !ec;
            }
#endif
        }

        bool LiveModule::shutdown(
            error_code & ec)
        {
#ifndef JUST_CONTAIN_LIVE_WORKER
            if (process_) {
                error_code ec;
                process_->signal(Signal::sig_int, ec);
                process_->timed_join(1000, ec);
                if (!ec) {
                    LOG_INFO("[shutdown] ok");
                } else {
                    LOG_WARN("[shutdown] ec = " << ec.message());
                }
                process_->kill(ec);
                process_->close(ec);
            }
            if (timer_) {
                timer_->stop();
            }
            if (is_locked_) {
                mutex_.unlock();
                is_locked_ = false;
            }
#endif
            return !ec;
        }

        std::string LiveModule::version()
        {
            //return just::live_worker::version_string();
            return "1.0.0.1";
        }

        std::string LiveModule::name()
        {
            return just::live_worker::name_string();
        }

#ifndef JUST_CONTAIN_LIVE_WORKER
        bool LiveModule::is_lock()
        {
            if (!is_locked_) {
                is_locked_ = mutex_.try_lock();
            }

            return is_locked_;
        }
#endif

    } // namespace live
} // namespace just
