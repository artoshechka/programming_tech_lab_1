/// @file
/// @brief Объявление логгера приложения
/// @author Artemenko Anton
#ifndef GUID_14f30de6_d704_49d2_a2d5_649fb7c41a3b
#define GUID_14f30de6_d704_49d2_a2d5_649fb7c41a3b

#include <ilogger.hpp>

#include <QFile>
#include <QMutex>
#include <QTextStream>

namespace logger
{
/// @brief Логгер приложения
class AppLogger final : public ILogger
{
  public:
    /// @brief Конструктор логгера приложения
    /// @param[in] output Режим вывода логов (по умолчанию консоль)
    explicit AppLogger(LogOutput output = LogOutput::Console);

    /// @brief Деструктор логгера приложения
    ~AppLogger() override;

    /// @brief Установить файл для записи логов
    /// @param[in] filename Путь к файлу лога
    void SetLogFile(const QString &filename) override;

    /// @brief Установить минимальный уровень логирования
    /// @param[in] level Уровень логирования
    void SetLogLevel(LogLevel level) override;

    /// @brief Установить канал вывода логов
    /// @param[in] output Режим вывода
    void SetOutput(LogOutput output) override;

    /// @brief Записать сообщение в лог
    /// @param[in] level Уровень логирования
    /// @param[in] message Текст сообщения
    /// @param[in] file Имя файла
    /// @param[in] line Номер строки
    /// @param[in] function Имя функции
    void Log(LogLevel level, const QString &message, const char *file = nullptr, int line = 0,
             const char *function = nullptr) override;

  private:
    /// @brief Преобразовать уровень логирования в строку
    /// @param[in] level Уровень логирования
    /// @return Строковое представление
    QString LogLevelToString(LogLevel level) const;

    /// @brief Сформировать сообщение для записи
    /// @param[in] level Уровень логирования
    /// @param[in] message Текст сообщения
    /// @param[in] file Имя файла
    /// @param[in] line Номер строки
    /// @param[in] function Имя функции
    /// @return Отформатированное сообщение
    QString FormatMessage(LogLevel level, const QString &message, const char *file, int line,
                          const char *function) const;

  private:
    QFile logFile_;            ///< Файл лога
    QTextStream textStream_;   ///< Поток для записи
    QMutex syncMutex_;         ///< Мьютекс для синхронизации
    LogLevel currentLogLevel_; ///< Текущий уровень логирования
    LogOutput outputMode_;     ///< Канал вывода логов
};

} // namespace logger

#endif // GUID_14f30de6_d704_49d2_a2d5_649fb7c41a3b
