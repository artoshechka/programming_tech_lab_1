/// @file
/// @brief Объявление интерфейса логгера
/// @author Artemenko Anton
#ifndef GUID_76731180_6258_4b6b_b2a7_4ef2b0039d4d
#define GUID_76731180_6258_4b6b_b2a7_4ef2b0039d4d

#include <QString>

namespace logger
{
/// @brief Уровни логирования
enum class LogLevel
{
    Trace,   ///< Трассировка
    Debug,   ///< Отладка
    Info,    ///< Информация
    Warning, ///< Предупреждение
    Error,   ///< Ошибка
    Fatal    ///< Фатальная ошибка
};

/// @brief Режим вывода логов
enum class LogOutput
{
    Console, ///< Вывод только в консоль
    File     ///< Вывод только в файл
};

/// @brief Интерфейс логгера
class ILogger
{
  public:
    /// @brief Виртуальный деструктор интерфейса
    virtual ~ILogger() = default;

    /// @brief Установить файл для записи логов
    /// @param[in] filename Путь к файлу лога
    virtual void SetLogFile(const QString &filename) = 0;

    /// @brief Установить минимальный уровень логирования
    /// @param[in] level Уровень логирования
    virtual void SetLogLevel(LogLevel level) = 0;

    /// @brief Установить канал вывода логов
    /// @param[in] output Режим вывода
    virtual void SetOutput(LogOutput output) = 0;

    /// @brief Записать сообщение в лог
    /// @param[in] level Уровень логирования
    /// @param[in] message Текст сообщения
    /// @param[in] file Имя файла
    /// @param[in] line Номер строки
    /// @param[in] function Имя функции
    virtual void Log(LogLevel level, const QString &message, const char *file = nullptr, int line = 0,
                     const char *function = nullptr) = 0;
};

} // namespace logger

#endif // GUID_76731180_6258_4b6b_b2a7_4ef2b0039d4d
