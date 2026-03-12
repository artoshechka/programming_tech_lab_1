/// @file
/// @brief Определение класса, ответственного за мониторинг директорий
/// @author Artemenko Anton

#include <file_observer.hpp>
#include <logger_macros.hpp>

#include <utility>

using file_observer::FileObserver;

FileObserver::FileObserver(std::shared_ptr<logger::ILogger> observerLogger, QObject *parent)
    : QObject(parent), observerLogger_(std::move(observerLogger))
{
    connect(&systemWatcher_, &QFileSystemWatcher::fileChanged, this, &FileObserver::OnFileChanged);
    connect(&pollTimer_, &QTimer::timeout, this, &FileObserver::CheckFiles);
    pollTimer_.start(1000);
}

void FileObserver::AddFile(const QString &filePath)
{
    if (filePath.isEmpty())
        return;

    QFileInfo currentInfo(filePath);
    // Сохраняем инвертированное состояние, чтобы первая проверка дала начальное уведомление.
    fileContainer_[filePath] = file_observer::ObservedFileState(!currentInfo.exists(), 0);

    if (currentInfo.exists() && !systemWatcher_.files().contains(filePath))
    {
        systemWatcher_.addPath(filePath);
    }

    CheckFileChanges(filePath);
}

void FileObserver::RemoveFile(const QString &filePath)
{
    if (systemWatcher_.files().contains(filePath))
    {
        systemWatcher_.removePath(filePath);
    }
    fileContainer_.remove(filePath);
}

FileObserver::~FileObserver()
{

    if (!systemWatcher_.files().isEmpty())
    {
        systemWatcher_.removePaths(systemWatcher_.files());
    }
}

void FileObserver::CheckFiles()
{
    for (auto it = fileContainer_.cbegin(); it != fileContainer_.cend(); ++it)
    {
        CheckFileChanges(it.key());
    }
}

void FileObserver::OnFileChanged(const QString &path)
{
    CheckFileChanges(path);

    QFileInfo info(path);
    if (info.exists() && !systemWatcher_.files().contains(path))
    {
        systemWatcher_.addPath(path);
    }
}

void FileObserver::CheckFileChanges(const QString &filePath)
{
    auto it = fileContainer_.find(filePath);
    if (it == fileContainer_.end())
    {
        return;
    }

    QFileInfo currentInfo(filePath);
    file_observer::ObservedFileState &previous = it.value();

    const bool existsNow = currentInfo.exists();
    const qint64 sizeNow = existsNow ? currentInfo.size() : 0;

    const bool existenceChanged = previous.exists != existsNow;
    const bool sizeChanged = existsNow && previous.exists && (previous.size != sizeNow);

    if (existsNow)
    {
        if (sizeChanged)
        {
            LogInfo(observerLogger_) << "File changed: " << filePath << ", size=" << sizeNow;
        }
        else if (existenceChanged)
        {
            if (sizeNow > 0)
            {
                LogInfo(observerLogger_) << "File exists: " << filePath << ", size=" << sizeNow;
            }
            else
            {
                LogInfo(observerLogger_) << "File exists but empty: " << filePath;
            }
        }
    }
    else if (existenceChanged)
    {
        LogInfo(observerLogger_) << "File does not exist: " << filePath;
    }

    previous.exists = existsNow;
    previous.size = sizeNow;

    if (!existsNow && systemWatcher_.files().contains(filePath))
    {
        systemWatcher_.removePath(filePath);
    }
}