#include "utils.hpp"

#include <QDebug>

#include <orm/exceptions/invalidargumenterror.hpp>
#include <orm/utils/type.hpp>

#include "support/globals.hpp"

using Orm::Exceptions::InvalidArgumentError;

namespace TinyPlay::Support
{

void Utils::logException(const std::exception &e)
{
    // TODO future decide how qCritical()/qFatal() really works, also regarding to the Qt Creator's settings 'Ignore first chance access violations' and similar silverqx
    // TODO future alse how to correctly setup this in prod/dev envs. silverqx
    qCritical().nospace().noquote()
            << "\nCaught '"
            << Orm::Utils::Type::classPureBasename(e, true)
            << "' Exception:\n" << e.what();
}

void Utils::throwIfNonEmptyConn(const QString &connection)
{
    if (!Support::g_inThread && !connection.isEmpty())
        throw InvalidArgumentError(
                "The 'connection' argument is supported only when "
                "g_inThread = true.");
};

void Utils::throwIfEmptyConn(const QString &connection)
{
    if (Support::g_inThread && connection.isEmpty())
        throw InvalidArgumentError(
                "You have to pass the 'connection' argument when g_inThread = true.");
};

} // namespace TinyPlay::Support
