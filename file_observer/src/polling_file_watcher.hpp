/// @file
/// @brief Реализация наблюдателя за файлами через polling
/// @author Artemenko Anton

#ifndef GUID_1a2b3c4d_5e6f_47aa_8b9c_123456789abc
#define GUID_1a2b3c4d_5e6f_47aa_8b9c_123456789abc

#include <QDateTime>
#include <QFileInfo>
#include <QHash>
#include <QTimer>
#include <ifile_watcher.hpp>
#include <ilogger.hpp>
#include <memory>

namespace file_observer
{

/// @brief Состояние наблюдаемого файла
struct ObservedFileState
{
    ObservedFileState(bool existsState = false, const QDateTime& modifiedState = QDateTime());

    bool exists_;         ///< Признак существования файла
    QDateTime modified_;  ///< Дата последнего изменения
};

/// @brief Контейнер наблюдаемых файлов
using ObservingFileContainer = QHash<QString, ObservedFileState>;

/// @brief Реализация наблюдателя на основе polling
class PollingFileWatcher : public IFileWatcher
{
    Q_OBJECT
   public:
    /// @brief Конструктор
    /// @param[in] intervalMs интервал для поллинга
    /// @param[in] logger логгер
    explicit PollingFileWatcher(int intervalMs, std::shared_ptr<logger::ILogger> logger, QObject* parent = nullptr);

    /// @brief Деструктор
    ~PollingFileWatcher() override;

    /// @brief Добавить файл
    /// @param[in] path Путь к файлу
    void AddFile(const QString& path) override;

    /// @brief Удалить файл
    /// @param[in] path Путь к файлу
    void RemoveFile(const QString& path) override;

    /// @brief Вернуть список файлов
    /// @param[in] path Путь к файлу
    QStringList ListFiles() const override;

   private slots:
    /// @brief Проверка всех файлов
    void CheckFiles();

   private:
    /// @brief Проверка конкретного файла
    /// @param[in] path Путь к файлу
    void CheckFileChanges(const QString& path);

   private:
    QTimer timer_;                             ///< Таймер polling
    ObservingFileContainer files_;             ///< Контейнер файлов
    std::shared_ptr<logger::ILogger> logger_;  ///< Логгер
};

}  // namespace file_observer

#endif  // GUID_1a2b3c4d_5e6f_47aa_8b9c_123456789abc