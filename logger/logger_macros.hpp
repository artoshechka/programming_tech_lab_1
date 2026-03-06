/// @file
/// @brief Макросы для логирования (скрытая реализация)
/// @author Artemenko Anton
#ifndef LOGGER_MACROS_HPP
#define LOGGER_MACROS_HPP

#include <logger.hpp>

/// @brief Макрос для логирования с уровнем Trace
/// @details Автоматически добавляет информацию о файле, строке и функции
/// @param[in] msg Сообщение для логирования
#define LogTrace(msg) Logger::Instance().Log(LogLevel::Trace, msg, __FILE__, __LINE__, __FUNCTION__)

/// @brief Макрос для логирования с уровнем Debug
/// @details Автоматически добавляет информацию о файле, строке и функции
/// @param[in] msg Сообщение для логирования
#define LogDebug(msg) Logger::Instance().Log(LogLevel::Debug, msg, __FILE__, __LINE__, __FUNCTION__)

/// @brief Макрос для логирования с уровнем Info
/// @details Автоматически добавляет информацию о файле, строке и функции
/// @param[in] msg Сообщение для логирования
#define LogInfo(msg) Logger::Instance().Log(LogLevel::Info, msg, __FILE__, __LINE__, __FUNCTION__)

/// @brief Макрос для логирования с уровнем Warning
/// @details Автоматически добавляет информацию о файле, строке и функции
/// @param[in] msg Сообщение для логирования
#define LogWarning(msg) Logger::Instance().Log(LogLevel::Warning, msg, __FILE__, __LINE__, __FUNCTION__)

/// @brief Макрос для логирования с уровнем Error
/// @details Автоматически добавляет информацию о файле, строке и функции
/// @param[in] msg Сообщение для логирования
#define LogError(msg) Logger::Instance().Log(LogLevel::Error, msg, __FILE__, __LINE__, __FUNCTION__)

/// @brief Макрос для логирования с уровнем Fatal
/// @details Автоматически добавляет информацию о файле, строке и функции
/// @param[in] msg Сообщение для логирования
#define LogFatal(msg) Logger::Instance().Log(LogLevel::Fatal, msg, __FILE__, __LINE__, __FUNCTION__)

#endif // LOGGER_MACROS_HPP