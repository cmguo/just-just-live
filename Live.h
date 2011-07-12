// Live.h

#ifndef _PPBOX_LIVE_LIVE_H_
#define _PPBOX_LIVE_LIVE_H_

#include <ppbox/common/CommonModuleBase.h>

#include <framework/process/NamedMutex.h>
using namespace framework::process;

namespace framework
{
    namespace process { class Process; }
    namespace timer { class Timer; }
}

namespace ppbox
{

    namespace live
    {

        class Live
            : public ppbox::common::CommonModuleBase<Live>
        {
        public:
            Live(
                util::daemon::Daemon & daemon);

            ~Live();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        public:
            boost::uint16_t port() const
            {
                return port_;
            }

            bool is_alive();

            framework::process::Process const & process() const
            {
                return *process_;
            }

        public:
            static std::string version();

            static std::string name();

        private:
            void check();

            bool is_lock();

        private:
            boost::uint16_t port_;

        private:
            framework::process::Process * process_;
            framework::timer::Timer * timer_;

            NamedMutex mutex_;

            bool is_locked_;
        };

    }
}

#endif // _PPBOX_LIVE_LIVE_H_
