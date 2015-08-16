// LiveModule.h

#ifndef _JUST_LIVE_LIVE_MODULE_H_
#define _JUST_LIVE_LIVE_MODULE_H_

#include <just/common/CommonModuleBase.h>
#include <just/common/PortManager.h>

#ifndef JUST_DISABLE_DAC
#include <just/dac/DacModule.h>
#endif

#ifndef JUST_CONTAIN_LIVE_WORKER
#include <framework/process/NamedMutex.h>
using namespace framework::process;

namespace framework
{
    namespace process { class Process; }
    namespace timer { class Timer; }
}
#endif

namespace just
{

    namespace live
    {

        class LiveModule
            : public just::common::CommonModuleBase<LiveModule>
        {
        public:
            LiveModule(
                util::daemon::Daemon & daemon);

            ~LiveModule();

        public:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        public:
            boost::uint16_t port() const
            {
                return port_;
            }

            bool is_alive();

#ifndef JUST_CONTAIN_LIVE_WORKER
            framework::process::Process const & process() const
            {
                return *process_;
            }
#endif

        public:
            static std::string version();

            static std::string name();

        private:
            void check();

#ifndef JUST_CONTAIN_LIVE_WORKER
            bool is_lock();
#endif

        private:
#ifndef JUST_DISABLE_DAC
            just::dac::DacModule& dac_;
#endif
            just::common::PortManager &portMgr_;

            boost::uint16_t port_;

        private:
#ifndef JUST_CONTAIN_LIVE_WORKER
            framework::process::Process * process_;
            framework::timer::Timer * timer_;

            NamedMutex mutex_;

            bool is_locked_;
#endif
        };

    }
}

#endif // _JUST_LIVE_LIVE_MODULE_H_
