/// @file
/// @brief Определение логгера приложения
/// @author Artemenko Anton

#include <app_logger.hpp>

#include <QDateTime>
#include <QDir>

#include <iostream>

using logger::AppLogger;
using logger::LogLevel;

namespace
{
QString LogLevelToStringImpl(logger::LogLevel level)
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

AppLogger::AppLogger(LogOutput output) : currentLogLevel_(LogLevel::Debug), outputMode_(output)
{
}

AppLogger::~AppLogger()
{
    if (logFile_.isOpen())
    {
        logFile_.close();
    }
}

void AppLogger::SetLogFile(const QString &filename)
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

void AppLogger::SetLogLevel(LogLevel level)
{
    currentLogLevel_ = level;
}

void AppLogger::SetOutput(LogOutput output)
{
    QMutexLocker locker(&syncMutex_);
    outputMode_ = output;
}

QString AppLogger::LogLevelToString(LogLevel level) const
{
    return ::LogLevelToStringImpl(level);
}

QString AppLogger::FormatMessage(LogLevel level, const QString &message, const char *file, int line,
                                 const char *function) const
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString logLevel = LogLevelToString(level);

    QString fileInfo;
    if (file != nullptr)
    {
        QFileInfo fi(file);
        fileInfo = QString("%1:%2").arg(fi.fileName()).arg(line);
    }

    QString functionInfo;
    if (function != nullptr)
    {
        functionInfo = QString::fromUtf8(function);
    }

    return QString("[%1] [%2] [APP] %3 - %4 - %5")
        .arg(timestamp)
        .arg(logLevel)
        .arg(fileInfo)
        .arg(functionInfo)
        .arg(message);
}

void AppLogger::Log(LogLevel level, const QString &message, const char *file, int line, const char *function)
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
