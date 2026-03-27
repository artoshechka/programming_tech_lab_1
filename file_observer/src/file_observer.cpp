/// @file
/// @brief Определение класса, ответственного за мониторинг директорий
/// @author Artemenko Anton

#include <file_observer.hpp>
#include <logger_macros.hpp>

using file_observer::FileObserver;

FileObserver::FileObserver(std::shared_ptr<IFileWatcher> watcher, std::shared_ptr<logger::ILogger> observerLogger,
                           QObject* parent)
    : QObject(parent), watcher_(std::move(watcher)), logger_(std::move(observerLogger))
{
    connect(watcher_.get(), &IFileWatcher::FileChanged, this, &FileObserver::OnFileChanged);
    connect(watcher_.get(), &IFileWatcher::FileCreated, this, &FileObserver::OnFileCreated);
    connect(watcher_.get(), &IFileWatcher::FileRemoved, this, &FileObserver::OnFileRemoved);
}

FileObserver::~FileObserver() = default;

void FileObserver::AddFile(const QString& filePath)
{
    if (filePath.isEmpty()) return;

    watcher_->AddFile(filePath);
}

void FileObserver::RemoveFile(const QString& filePath)
{
    watcher_->RemoveFile(filePath);
}

QStringList FileObserver::ListAllFiles() const
{
    return watcher_->ListFiles();
}

void FileObserver::OnFileChanged(const QString& path, qint64 size)
{
    LogInfo(logger_) << "File changed: " << path << " Size: " << size;
}

void FileObserver::OnFileCreated(const QString& path)
{
    LogInfo(logger_) << "File created: " << path;
}

void FileObserver::OnFileRemoved(const QString& path)
{
    LogInfo(logger_) << "File removed: " << path;
}