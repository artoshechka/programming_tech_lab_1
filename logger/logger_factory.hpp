/// @file
/// @brief Фабричные функции для singleton-логгеров
/// @author Artemenko Anton
#ifndef GUID_32ddf2a4_eb2e_4d0f_b5b2_6f6b683f7ae0
#define GUID_32ddf2a4_eb2e_4d0f_b5b2_6f6b683f7ae0
#pragma once

#include <ilogger.hpp>

#include <memory>

namespace logger
{
/// @brief Тип канала вывода для всех singleton-логгеров
enum class LoggerOutputMode
{
    Console, ///< Вывод только в консоль
    File     ///< Вывод только в файл
};

/// @brief Установить общий канал вывода для всех singleton-логгеров
/// @param[in] mode Режим вывода
void SetGlobalLogOutput(LoggerOutputMode mode);

/// @brief Получить текущий общий канал вывода
/// @return Текущий режим вывода
LoggerOutputMode GetGlobalLogOutput();

/// @brief Получить singleton-логгер приложения
/// @return Умный указатель на логгер приложения
std::shared_ptr<ILogger> GetAppLogger();

/// @brief Получить singleton-логгер наблюдения
/// @return Умный указатель на логгер наблюдения
std::shared_ptr<ILogger> GetObserverLogger();

} // namespace logger

#endif // GUID_32ddf2a4_eb2e_4d0f_b5b2_6f6b683f7ae0
