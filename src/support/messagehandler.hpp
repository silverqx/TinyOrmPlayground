#ifndef TINYPLAY_SUPPORT_MESSAGEHANDLER_HPP
#define TINYPLAY_SUPPORT_MESSAGEHANDLER_HPP

#include <qlogging.h>

class QMessageLogContext;
class QString;

namespace TinyPlay::Support
{

    /*! Custom message handler used in the TinyPlayground application. */
    void tinyMessageHandler(QtMsgType type, const QMessageLogContext &context,
                            const QString &msg);

} // namespace TinyPlay::Support

#endif // TINYPLAY_SUPPORT_MESSAGEHANDLER_HPP
