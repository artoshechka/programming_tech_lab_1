/// @file
/// @brief Определение класса, ответственного за мониторинг директорий
/// @author Artemenko Anton

#include <file_observer.hpp>
#include <logger_macros.hpp>

using file_observer::FileObserver;

FileObserver::FileObserver(const std::shared_ptr<logger::Logger> &logger, QObject *parent)
    : QObject(parent), logger_(logger)
{
    // Настройка таймера для периодической проверки
    watchTimer_.setInterval(CHECK_INTERVAL_MS);
    connect(&watchTimer_, &QTimer::timeout, this, &FileObserver::CheckFiles);

    // Подключение сигналов файловой системы
    connect(&systemWatcher_, &QFileSystemWatcher::fileChanged, this, &FileObserver::OnFileChanged);
}

void FileObserver::AddFile(const QString &filePath)
{
    if (filePath.isEmpty())
        return;

    // Добавляем файл в наблюдатель
    if (!systemWatcher_.files().contains(filePath))
    {
        systemWatcher_.addPath(filePath);
    }

    // Сохраняем информацию о файле
    QFileInfo fileInfo(filePath);
    fileContainer_[filePath] = fileInfo;

    LogInfo("File: " + fileInfo.absoluteFilePath() + " added under Observing!");
}

void FileObserver::RemoveFile(const QString &filePath)
{
    if (systemWatcher_.files().contains(filePath))
    {
        systemWatcher_.removePath(filePath);
        LogInfo("File: " + filePath + " removed from Observing!");
    }
    fileContainer_.remove(filePath);
}

void FileObserver::Start()
{
    watchTimer_.start();
}

void FileObserver::Stop()
{
    watchTimer_.stop();
}

void FileObserver::CheckFiles()
{
    for (auto it = fileContainer_.begin(); it != fileContainer_.end(); ++it)
    {
        CheckFileChanges(it.key());
    }
}

void FileObserver::OnFileChanged(const QString &path)
{
    CheckFileChanges(path);
}

void FileObserver::CheckFileChanges(const QString &filePath)
{
    auto it = fileContainer_.find(filePath);
    if (it == fileContainer_.end())
        return;

    QFileInfo currentInfo(filePath);
    const QFileInfo &oldInfo = it.value();

    bool existsChanged = (currentInfo.exists() != oldInfo.exists());
    bool sizeChanged = (currentInfo.exists() && oldInfo.exists() && currentInfo.size() != oldInfo.size());

    if (existsChanged || sizeChanged)
    {
        LogInfo("File: " + currentInfo.absoluteFilePath() + " changed!");
        it.value() = currentInfo;
    }
}