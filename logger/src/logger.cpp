/// @file
/// @brief Реализация класса логгера
/// @author Artemenko Anton
#include <logger.hpp>

#include <QCoreApplication>
#include <QDir>
#include <iostream>

using logger::Logger;
namespace
{
/// @brief Получить строковое представление уровня логирования
/// @param[in] level Уровень логирования
/// @return Строковое представление
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

Logger::Logger()
    : QObject(nullptr), flushTimer_(nullptr), currentLogLevel_(LogLevel::Debug), maxFileSize_(DEFAULT_MAX_FILE_SIZE),
      maxBackupFiles_(DEFAULT_MAX_BACKUP_FILES)
{
    flushTimer_ = new QTimer(this);
    flushTimer_->setInterval(FLUSH_INTERVAL_MS);
    connect(flushTimer_, &QTimer::timeout, this, &Logger::FlushBuffer);
    flushTimer_->start();
}

Logger::~Logger()
{
    FlushBuffer();
    if (logFile_.isOpen())
    {
        logFile_.close();
    }
}

Logger &Logger::Instance()
{
    static Logger instance;
    return instance;
}

void Logger::SetLogFile(const QString &filename)
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

        QString separator(80, '=');
        textStream_ << "\n" << separator << "\n";
        textStream_ << "Log started at: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
        textStream_ << separator << "\n\n";
        textStream_.flush();
    }
}

void Logger::SetLogLevel(LogLevel level)
{
    currentLogLevel_ = level;
}

void Logger::SetMaxFileSize(qint64 size)
{
    maxFileSize_ = size;
}

void Logger::SetMaxBackupFiles(int count)
{
    maxBackupFiles_ = count;
}

QString Logger::LogLevelToString(LogLevel level) const
{
    return ::LogLevelToStringImpl(level);
}

QString Logger::FormatMessage(LogLevel level, const QString &message, const char *file, int line,
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

    return QString("[%1] [%2] %3 - %4 - %5").arg(timestamp).arg(logLevel).arg(fileInfo).arg(functionInfo).arg(message);
}

void Logger::Log(LogLevel level, const QString &message, const char *file, int line, const char *function)
{
    if (level < currentLogLevel_)
    {
        return;
    }

    QString logEntry = FormatMessage(level, message, file, line, function);

    std::cout << logEntry.toStdString() << std::endl;

    QMutexLocker locker(&syncMutex_);

    buffer_.enqueue(logEntry);

    if (buffer_.size() >= BUFFER_SIZE || level == LogLevel::Fatal)
    {
        locker.unlock();
        FlushBuffer();
    }
}

void Logger::FlushBuffer()
{
    QMutexLocker locker(&syncMutex_);

    if (!logFile_.isOpen() || buffer_.isEmpty())
    {
        return;
    }

    if (logFile_.size() >= maxFileSize_)
    {
        RotateLogFile();
    }

    while (!buffer_.isEmpty())
    {
        textStream_ << buffer_.dequeue() << Qt::endl;
    }
    textStream_.flush();
}

void Logger::RotateLogFile()
{
    if (!logFile_.isOpen())
    {
        return;
    }

    logFile_.close();

    QString baseName = logFile_.fileName();

    QString oldestBackup = QString("%1.%2").arg(baseName).arg(maxBackupFiles_);
    if (QFile::exists(oldestBackup))
    {
        QFile::remove(oldestBackup);
    }

    for (int i = maxBackupFiles_ - 1; i >= 1; --i)
    {
        QString oldName = QString("%1.%2").arg(baseName).arg(i);
        QString newName = QString("%1.%2").arg(baseName).arg(i + 1);

        if (QFile::exists(oldName))
        {
            QFile::rename(oldName, newName);
        }
    }

    if (QFile::exists(baseName))
    {
        QFile::rename(baseName, baseName + ".1");
    }

    if (logFile_.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        textStream_.setDevice(&logFile_);

        QString separator(80, '=');
        textStream_ << separator << "\n";
        textStream_ << "Log file rotated at: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
        textStream_ << separator << "\n\n";
    }
}