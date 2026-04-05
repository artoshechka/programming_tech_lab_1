/// @file
/// @brief Интерфейс наблюдателя за файлами
/// @author Artemenko Anton

#ifndef GUID_9c6c6d4a_2e5a_4a3b_9b7a_1c2e8f6c1111
#define GUID_9c6c6d4a_2e5a_4a3b_9b7a_1c2e8f6c1111

#include <QObject>
#include <QString>
#include <QStringList>

namespace file_observer
{

/// @brief Интерфейс для наблюдения за файлами
class IFileWatcher : public QObject
{
    Q_OBJECT
   public:
    /// @brief Конструктор
    explicit IFileWatcher(QObject* parent = nullptr) : QObject(parent)
    {
    }
    /// @brief Деструктор по умолчанию
    ~IFileWatcher() override = default;

    /// @brief Добавить файл для наблюдения
    /// @param[in] path Путь к файлу
    virtual bool AddFile(const QString& path) = 0;

    /// @brief Удалить файл из наблюдения
    /// @param[in] path Путь к файлу
    virtual bool RemoveFile(const QString& path) = 0;

    /// @brief Получить список файлов
    /// @return Список путей
    virtual QStringList ListFiles() const = 0;

   signals:
    /// @brief Файл был изменён
    /// @param[out] path Путь к файлу
    /// @param[out] size Размер файл
    void FileChanged(const QString& path, qint64 size);

    /// @brief Файл был создан
    /// @param[out] path Путь к файлу
    void FileExistence(const QString& path, bool isExists, qint64 size);

    /// @brief Файл был удалён
    /// @param[out] path Путь к файлу
    void FileRemoved(const QString& path);
};

}  // namespace file_observer

#endif  // GUID_9c6c6d4a_2e5a_4a3b_9b7a_1c2e8f6c1111