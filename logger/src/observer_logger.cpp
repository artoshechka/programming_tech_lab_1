/// @file
/// @brief Определение логгера наблюдения за файлами
/// @author Artemenko Anton

#include <QDateTime>
#include <observer_logger.hpp>

using logger::LogLevel;
using logger::ObserverLogger;

ObserverLogger::ObserverLogger(LogOutput output) : ThreadSafeLogger("OBSERVER", output)
{
}

ObserverLogger::~ObserverLogger() = default;

QString ObserverLogger::FormatMessage(LogLevel level, const QString& message, const char* file, int line,
                                      const char* function) const
{
    Q_UNUSED(level);
    Q_UNUSED(file);
    Q_UNUSED(line);
    Q_UNUSED(function);

    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    return QString("[%1] [%2] %3").arg(timestamp).arg(componentName_).arg(message);
}
