// LiveModule.h

#ifndef _PPBOX_LIVE_LIVE_MODULE_H_
#define _PPBOX_LIVE_LIVE_MODULE_H_

#include <ppbox/common/CommonModuleBase.h>
#include <ppbox/common/PortManager.h>

#ifndef PPBOX_DISABLE_DAC
#include <ppbox/dac/DacModule.h>
#endif

#ifndef PPBOX_CONTAIN_LIVE_WORKER
#include <framework/process/NamedMutex.h>
using namespace framework::process;

namespace framework
{
    namespace process { class Process; }
    namespace timer { class Timer; }
}
#endif

namespace ppbox
{

    namespace live
    {

        class LiveModule
            : public ppbox::common::CommonModuleBase<LiveModule>
        {
        public:
            LiveModule(
                util::daemon::Daemon & daemon);

            ~LiveModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        public:
            boost::uint16_t port() const
            {
                return port_;
            }

            bool is_alive();

#ifndef PPBOX_CONTAIN_LIVE_WORKER
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

#ifndef PPBOX_CONTAIN_LIVE_WORKER
            bool is_lock();
#endif

        private:
#ifndef PPBOX_DISABLE_DAC
            ppbox::dac::DacModule& dac_;
#endif
            ppbox::common::PortManager &portMgr_;

            boost::uint16_t port_;

        private:
#ifndef PPBOX_CONTAIN_LIVE_WORKER
            framework::process::Process * process_;
            framework::timer::Timer * timer_;

            NamedMutex mutex_;

            bool is_locked_;
#endif
        };

    }
}

#endif // _PPBOX_LIVE_LIVE_MODULE_H_
