/// @file
/// @brief Объявление класса, ответственного за мониторинг директорий
/// @author Artemenko Anton

#ifndef GUID_f2735f4b_5fd5_444d_b275_684e413b6822
#define GUID_f2735f4b_5fd5_444d_b275_684e413b6822

#include <QObject>
#include <ifile_watcher.hpp>
#include <ilogger.hpp>
#include <memory>

namespace file_observer
{

/// @brief Класс-наблюдатель, агрегирующий IFileWatcher
class FileObserver : public QObject
{
    Q_OBJECT
   public:
    /// @brief Конструктор
    /// @param[in] watcher Реализация наблюдателя (передаётся во владение)
    /// @param[in] logger Логгер
    explicit FileObserver(std::unique_ptr<IFileWatcher> watcher, std::shared_ptr<logger::ILogger> logger,
                          QObject* parent = nullptr);

    /// @brief Деструктор
    ~FileObserver();

    /// @brief Добавить файл
    void AddFile(const QString& path);

    /// @brief Удалить файл
    void RemoveFile(const QString& path);

    /// @brief Получить список файлов
    QStringList ListAllFiles() const;

    /// @brief Установить новый IFileWatcher
    void SetWatcher(std::unique_ptr<IFileWatcher> watcher);

   private slots:
    /// @brief Файл был изменён
    /// @param[in] path Путь к файлу
    /// @param[in] size Размер файл
    void OnFileChanged(const QString& path, qint64 size);

    /// @brief Файл был создан
    /// @param[in] path Путь к файлу
    void OnFileExistence(const QString& path, qint64 size);

    /// @brief Файл был удалён
    /// @param[in] path Путь к файлу
    void OnFileRemoved(const QString& path);

   private:
    std::unique_ptr<IFileWatcher> watcher_;    ///< наблюдатель за файлами (стратегия)
    std::shared_ptr<logger::ILogger> logger_;  ///< Логгер
};

}  // namespace file_observer

#endif  // GUID_f2735f4b_5fd5_444d_b275_684e413b6822