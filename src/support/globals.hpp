#pragma once
#ifndef TINYPLAY_SUPPORT_GLOBALS_HPP
#define TINYPLAY_SUPPORT_GLOBALS_HPP

#include <QString>

#include <vector>

#include <orm/macros/threadlocal.hpp>

namespace TinyPlay::Support
{

    /*! Determine whether currently running in a thread. */
    T_THREAD_LOCAL
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
    T_THREAD_LOCAL
    inline std::vector<MessageLogItem> g_logFromThread {};

} // namespace TinyPlay::Support

#endif // TINYPLAY_SUPPORT_GLOBALS_HPP
