/// @file
/// @brief Объявление класса логгера для приложения
/// @author Artemenko Anton
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QTextStream>
#include <QTimer>

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

/// @brief Класс, ответственный за логирование событий приложения
class Logger : public QObject
{
    Q_OBJECT

  public:
    /// @brief Получить экземпляр логгера (Singleton)
    static Logger &Instance();

    /// @brief Установить файл для записи логов
    /// @param[in] filename Путь к файлу лога
    void SetLogFile(const QString &filename);

    /// @brief Установить минимальный уровень логирования
    /// @param[in] level Уровень логирования
    void SetLogLevel(LogLevel level);

    /// @brief Установить максимальный размер файла лога
    /// @param[in] size Максимальный размер в байтах
    void SetMaxFileSize(qint64 size);

    /// @brief Установить максимальное количество backup файлов
    /// @param[in] count Количество файлов
    void SetMaxBackupFiles(int count);

    /// @brief Записать сообщение в лог
    /// @param[in] level Уровень логирования
    /// @param[in] message Текст сообщения
    /// @param[in] file Имя файла
    /// @param[in] line Номер строки
    /// @param[in] function Имя функции
    void Log(LogLevel level, const QString &message, const char *file = nullptr, int line = 0,
             const char *function = nullptr);

  private slots:
    /// @brief Сбросить буфер логов в файл
    void FlushBuffer();

  private:
    Logger();
    ~Logger();

    /// @brief Выполнить ротацию лог-файла
    void RotateLogFile();

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
    QTimer *flushTimer_;       ///< Таймер сброса буфера
    QQueue<QString> buffer_;   ///< Буфер сообщений
    LogLevel currentLogLevel_; ///< Текущий уровень логирования
    qint64 maxFileSize_;       ///< Максимальный размер файла
    int maxBackupFiles_;       ///< Максимальное количество backup файлов

    static constexpr int BUFFER_SIZE = 100;                           ///< Размер буфера
    static constexpr int FLUSH_INTERVAL_MS = 5000;                    ///< Интервал сброса (5 сек)
    static constexpr qint64 DEFAULT_MAX_FILE_SIZE = 10 * 1024 * 1024; ///< 10 MB
    static constexpr int DEFAULT_MAX_BACKUP_FILES = 5;                ///< Количество backup файлов по умолчанию
};

#endif // LOGGER_HPP