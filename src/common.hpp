#ifndef COMMON_HPP
#define COMMON_HPP

#ifndef QT_NO_EXCEPTIONS

#  define TINY_VERIFY_EXCEPTION_THROWN(expression, exceptiontype)\
    do {\
        QT_TRY {\
            QT_TRY {\
                expression;\
                qFatal("Expected exception of type '" #exceptiontype "' to be "\
                       "thrown, but no exception caught. [%s(%u)]", __FILE__, __LINE__);\
                return;\
            } QT_CATCH (const exceptiontype &e) {\
                qDebug() << "Caught expected exception with message :" << e.what();\
            }\
        } QT_CATCH (const std::exception &e) {\
            qFatal("Expected exception of type '" #exceptiontype\
                   "' to be thrown, but 'std::exception' caught with "\
                   "message : %s. [%s(%u)]", e.what(), __FILE__, __LINE__);\
            return;\
        } QT_CATCH (...) {\
            qFatal("Expected exception of type '" #exceptiontype "' to be thrown, "\
                   "but unknown exception caught. [%s(%u)]", __FILE__, __LINE__);\
            return;\
        }\
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

#endif // COMMON_HPP
