/// @file
/// @brief Определение класса, ответственного за мониторинг директорий
/// @author Artemenko Anton

#include <file_observer.hpp>
#include <logger_macros.hpp>

using file_observer::FileObserver;

FileObserver::FileObserver(std::unique_ptr<IFileWatcher> watcher, std::shared_ptr<logger::ILogger> observerLogger,
                           QObject* parent)
    : QObject(parent), watcher_(std::move(watcher)), logger_(std::move(observerLogger))
{
    if (watcher_)
    {
        connect(watcher_.get(), &IFileWatcher::FileChanged, this, &FileObserver::OnFileChanged);
        connect(watcher_.get(), &IFileWatcher::FileExistence, this, &FileObserver::OnFileExistence);
        connect(watcher_.get(), &IFileWatcher::FileRemoved, this, &FileObserver::OnFileRemoved);
    } else
    {
        LogWarning(logger_) << "FileObserver created without watcher";
    }
}
void FileObserver::SetWatcher(std::unique_ptr<IFileWatcher> watcher)
{
    if (watcher_)
    {
        disconnect(watcher_.get(), nullptr, this, nullptr);
    }
    watcher_ = std::move(watcher);
    if (watcher_)
    {
        connect(watcher_.get(), &IFileWatcher::FileChanged, this, &FileObserver::OnFileChanged);
        connect(watcher_.get(), &IFileWatcher::FileExistence, this, &FileObserver::OnFileExistence);
        connect(watcher_.get(), &IFileWatcher::FileRemoved, this, &FileObserver::OnFileRemoved);
    } else
    {
        LogWarning(logger_) << "FileObserver watcher cleared";
    }
}

FileObserver::~FileObserver() = default;

void FileObserver::AddFile(const QString& filePath)
{
    if (filePath.isEmpty()) return;

    if (!watcher_)
    {
        LogWarning(logger_) << "Cannot add file because watcher is not set: " << filePath;
        return;
    }

    watcher_->AddFile(filePath);
}

void FileObserver::RemoveFile(const QString& filePath)
{
    if (!watcher_)
    {
        LogWarning(logger_) << "Cannot remove file because watcher is not set: " << filePath;
        return;
    }

    watcher_->RemoveFile(filePath);
}

QStringList FileObserver::ListAllFiles() const
{
    if (!watcher_)
    {
        LogWarning(logger_) << "Cannot list files because watcher is not set";
        return {};
    }

    return watcher_->ListFiles();
}

void FileObserver::OnFileChanged(const QString& path, qint64 size)
{
    LogInfo(logger_) << "File changed: " << path << " Size: " << size;
}

void FileObserver::OnFileExistence(const QString& path, bool isExists, qint64 size)
{
    LogInfo(logger_) << "File: " << path << " Existence: " << isExists << " Size:" << size;
}

void FileObserver::OnFileRemoved(const QString& path)
{
    LogInfo(logger_) << "File removed: " << path;
}