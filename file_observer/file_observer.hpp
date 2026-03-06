/// @file
/// @brief Объявление класса, ответственного за мониторинг директорий
/// @author Artemenko Anton
#include <logger.hpp>

#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QHash>
#include <QObject>
#include <QString>
#include <QTimer>
#ifndef GUID_f2735f4b_5fd5_444d_b275_684e413b6822
#define GUID_f2735f4b_5fd5_444d_b275_684e413b6822
#pragma once
namespace file_observer
{

using ObservingFileContainer = QHash<QString, QFileInfo>;

/// @brief Класс, ответственный за мониторинг
class FileObserver : public QObject
{
    Q_OBJECT

  public:
    explicit FileObserver(QObject *parent = nullptr);
    ~FileObserver();

    /// @brief Добавить файл для наблюдения
    /// @param[in] filePath Путь к файлу
    void AddFile(const QString &filePath);

    /// @brief Удалить файл из наблюдения
    /// @param[in] filePath Путь к файлу
    void RemoveFile(const QString &filePath);

  private:
    /// @brief Проверить изменения в файле
    /// @param[in] filePath Путь к файлу
    void CheckFileChanges(const QString &filePath);

  private slots:
    /// @brief Проверить состояние всех файлов
    void CheckFiles();

    /// @brief Обработчик изменения файла
    void OnFileChanged(const QString &path);

  private:
    QFileSystemWatcher systemWatcher_;             ///< Наблюдатель за файловой системой
    ObservingFileContainer fileContainer_;         ///< Контейнер наблюдаемых файлов
};

} // namespace file_observer
#endif // GUID_f2735f4b_5fd5_444d_b275_684e413b6822