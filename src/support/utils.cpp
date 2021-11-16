#include "utils.hpp"

#include <QDebug>

#include <orm/utils/type.hpp>

namespace TinyPlay {

void Utils::logException(const std::exception &e)
{
    // TODO future decide how qCritical()/qFatal() really works, also regarding to the Qt Creator's settings 'Ignore first chance access violations' and similar silverqx
    // TODO future alse how to correctly setup this in prod/dev envs. silverqx
    qCritical().nospace().noquote()
            << "\nCaught '"
            << Orm::Utils::Type::classPureBasename(e, true)
            << "' Exception:\n" << e.what() << "\n";
}

} // namespace TinyPlay
