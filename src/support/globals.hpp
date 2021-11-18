#pragma once
#ifndef TINYPLAY_SUPPORT_GLOBALS_HPP
#define TINYPLAY_SUPPORT_GLOBALS_HPP

#include <QString>

#include <vector>

namespace TinyPlay::Support
{

    /*! Determine whether currently running in a thread. */
    thread_local
    inline bool g_inThread = false;

    /*! One log message from custom message handler. */
    struct MessageLogItem
    {
        /*! Type of a message. */
        QtMsgType type;
        /*! Message content. */
        QString   message;
    };

    /*! All log messages from a current thread. */
    thread_local
    inline std::vector<MessageLogItem> g_logFromThread {};

} // namespace TinyPlay::Support

#endif // TINYPLAY_SUPPORT_GLOBALS_HPP
