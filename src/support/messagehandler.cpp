#include "support/messagehandler.hpp"

#include "support/globals.hpp"

namespace TinyPlay::Support
{

namespace
{
    /*! Qt's default message handler. */
    const auto DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);
}

void tinyMessageHandler(QtMsgType type, const QMessageLogContext &context,
                        const QString &msg)
{
    /* When called from a worker thread, then do not pollute the console log and
       temporarily log all output to the global variable. */
    if (Support::g_inThread) {
#ifdef __clang__
        Support::g_logFromThread.push_back({type, msg});
#else
        Support::g_logFromThread.emplace_back(type, msg);
#endif
        return;
    }

    // Call the Qt's default message handler
    std::invoke(DEFAULT_MESSAGE_HANDLER, type, context, msg);
}

} // namespace TinyPlay::Support
