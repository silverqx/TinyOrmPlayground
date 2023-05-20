#pragma once
#ifndef TINYPLAY_MACROS_COMMON_HPP
#define TINYPLAY_MACROS_COMMON_HPP

#ifndef QT_NO_EXCEPTIONS

#  include "orm/utils/type.hpp" // IWYU pragma: keep

#  define TINY_VERIFY_EXCEPTION_THROWN(expression, exceptiontype)                        \
    do {                                                                                 \
        try {                                                                            \
            try {                                                                        \
                expression;                                                              \
                qFatal("Expected exception of type '" #exceptiontype "' to be "          \
                       "thrown, but no exception caught. [%s(%u)]", __FILE__, __LINE__); \
                return;                                                                  \
            } catch (const exceptiontype &e) {                                           \
                qDebug().noquote().nospace()                                             \
                        << "Caught expected '"                                           \
                        << Orm::Utils::Type::classPureBasename<exceptiontype>(true)      \
                        << "' Exception : " << e.what();                                 \
            }                                                                            \
        } catch (const std::exception &e) {                                              \
            qFatal("Expected exception of type '" #exceptiontype                         \
                   "' to be thrown, but 'std::exception' caught with "                   \
                   "message : %s. [%s(%u)]", e.what(), __FILE__, __LINE__);              \
            return;                                                                      \
        } catch (...) {                                                                  \
            qFatal("Expected exception of type '" #exceptiontype "' to be thrown, "      \
                   "but unknown exception caught. [%s(%u)]", __FILE__, __LINE__);        \
            return;                                                                      \
        }                                                                                \
    } while (false)

#else // QT_NO_EXCEPTIONS

/*
 * The expression passed to the macro should throw an exception and we can't
 * catch it because Qt has been compiled without exception support. We can't
 * skip the expression because it may have side effects and must be executed.
 * So, users must use Qt with exception support enabled if they use exceptions
 * in their code.
 */
#  define TINY_VERIFY_EXCEPTION_THROWN(expression, exceptiontype)\
     Q_STATIC_ASSERT_X(false, "Support of exceptions is disabled.")

#endif // !QT_NO_EXCEPTIONS

#endif // TINYPLAY_MACROS_COMMON_HPP
