/// @file
/// @brief Реализация настроек логгера
/// @author Artemenko Anton

#include <log_settings.hpp>

namespace logger
{

LoggerSettings::LoggerSettings() : logFilePath_(QString()), logLevel_(LogLevel::Debug), output_(LogOutput::Console)
{
}

LoggerSettings::LoggerSettings(const QString &logFilePath, LogLevel logLevel, LogOutput output)
    : logFilePath_(logFilePath), logLevel_(logLevel), output_(output)
{
}

} // namespace logger
