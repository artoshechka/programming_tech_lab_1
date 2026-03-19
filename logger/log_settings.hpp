/// @file
/// @brief Настройки вывода логов
/// @author Artemenko Anton
#ifndef GUID_0e57db76_e096_4f28_a3fd_3f04c43f2b6e
#define GUID_0e57db76_e096_4f28_a3fd_3f04c43f2b6e

#include <log_levels.hpp>

#include <QString>

namespace logger
{
/// @brief Режим вывода логов
enum class LogOutput
{
    Console, ///< Вывод только в консоль
    File     ///< Вывод только в файл
};

/// @brief Полный набор настроек логгера
struct LoggerSettings
{
    QString logFilePath_; ///< Путь к файлу лога
    LogLevel logLevel_;   ///< Минимальный уровень логирования
    LogOutput output_;    ///< Режим вывода логов

    /// @brief Конструктор настроек по умолчанию
    LoggerSettings();

    /// @brief Конструктор с явными параметрами
    /// @param[in] logFilePath Путь к файлу лога
    /// @param[in] logLevel Минимальный уровень логирования
    /// @param[in] output Режим вывода логов
    LoggerSettings(const QString &logFilePath, LogLevel logLevel, LogOutput output);
};

} // namespace logger

#endif // GUID_0e57db76_e096_4f28_a3fd_3f04c43f2b6e