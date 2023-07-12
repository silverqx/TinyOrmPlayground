#pragma once
#ifndef TINYPLAY_SUPPORT_UTILS_HPP
#define TINYPLAY_SUPPORT_UTILS_HPP

#include <QtGlobal>

namespace TinyPlay::Support
{

    /*! Common utility library class. */
    class Utils
    {
        Q_DISABLE_COPY(Utils)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Utils() = delete;
        /*! Deleted destructor. */
        ~Utils() = delete;

        /*! Throw when a connection parameter was passed in the main thread. */
        static void throwIfNonEmptyConn(const QString &connection);
        /*! Throw when a connection parameter was not passed in a worker thread. */
        static void throwIfEmptyConn(const QString &connection);
    };

} // namespace TinyPlay::Support

#endif // TINYPLAY_SUPPORT_UTILS_HPP
