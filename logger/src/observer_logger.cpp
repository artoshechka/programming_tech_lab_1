/// @file
/// @brief Определение логгера наблюдения за файлами
/// @author Artemenko Anton

#include <observer_logger.hpp>

#include <QDateTime>
#include <QDir>

#include <iostream>

using logger::LogLevel;
using logger::ObserverLogger;

namespace
{
QString LogLevelToStringImpl(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Trace:
        return "TRACE";
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARNING";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Fatal:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}
} // namespace

ObserverLogger::ObserverLogger(LogOutput output) : currentLogLevel_(LogLevel::Debug), outputMode_(output)
{
}

ObserverLogger::~ObserverLogger()
{
    if (logFile_.isOpen())
    {
        logFile_.close();
    }
}

void ObserverLogger::SetLogFile(const QString &filename)
{
    QMutexLocker locker(&syncMutex_);
    if (logFile_.isOpen())
    {
        logFile_.close();
    }

    logFile_.setFileName(filename);

    QDir dir(QFileInfo(filename).absolutePath());
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    if (logFile_.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        textStream_.setDevice(&logFile_);
        textStream_.setCodec("UTF-8");
        textStream_.flush();
    }
}

void ObserverLogger::SetLogLevel(LogLevel level)
{
    currentLogLevel_ = level;
}

void ObserverLogger::SetOutput(LogOutput output)
{
    QMutexLocker locker(&syncMutex_);
    outputMode_ = output;
}

QString ObserverLogger::LogLevelToString(LogLevel level) const
{
    return ::LogLevelToStringImpl(level);
}

QString ObserverLogger::FormatMessage(LogLevel level, const QString &message, const char *file, int line,
                                      const char *function) const
{
    Q_UNUSED(level);
    Q_UNUSED(file);
    Q_UNUSED(line);
    Q_UNUSED(function);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    return QString("[%1] [OBSERVER] %2").arg(timestamp).arg(message);
}

void ObserverLogger::Log(LogLevel level, const QString &message, const char *file, int line, const char *function)
{
    if (level < currentLogLevel_)
    {
        return;
    }

    QString logEntry = FormatMessage(level, message, file, line, function);

    QMutexLocker locker(&syncMutex_);

    if (outputMode_ == LogOutput::Console)
    {
        std::cout << logEntry.toStdString() << std::endl;
    }
    else if (outputMode_ == LogOutput::File && logFile_.isOpen())
    {
        textStream_ << logEntry << Qt::endl;
        textStream_.flush();
    }
}
