/// @file
/// @brief Реализация polling-наблюдателя за файлами
/// @author Artemenko Anton

#include <logger_macros.hpp>
#include <src/polling_file_watcher.hpp>

using namespace file_observer;

ObservedFileState::ObservedFileState(bool existsState, qint64 sizeState, const QDateTime& modifiedState)
    : exists_(existsState), size_(sizeState), modified_(modifiedState)
{
}

PollingFileWatcher::PollingFileWatcher(int intervalMs, std::shared_ptr<logger::ILogger> logger, QObject* parent)
    : IFileWatcher(parent), logger_(std::move(logger))
{
    timer_.setInterval(intervalMs);

    connect(&timer_, &QTimer::timeout, this, &PollingFileWatcher::CheckFiles);

    timer_.start();
}

PollingFileWatcher::~PollingFileWatcher() = default;

void PollingFileWatcher::AddFile(const QString& path)
{
    if (path.isEmpty()) return;

    QFileInfo info(path);

    files_[path] = ObservedFileState(info.exists(), info.exists() ? info.size() : 0,
                                     info.exists() ? info.lastModified() : QDateTime());
}

void PollingFileWatcher::RemoveFile(const QString& path)
{
    files_.remove(path);
}

QStringList PollingFileWatcher::ListFiles() const
{
    return files_.keys();
}

void PollingFileWatcher::CheckFiles()
{
    for (auto it = files_.begin(); it != files_.end(); ++it)
    {
        CheckFileChanges(it.key());
    }
}

void PollingFileWatcher::CheckFileChanges(const QString& path)
{
    auto it = files_.find(path);
    if (it == files_.end()) return;

    QFileInfo info(path);

    ObservedFileState& prev = it.value();

    const bool existsNow = info.exists();
    const qint64 sizeNow = existsNow ? info.size() : 0;
    const QDateTime modifiedNow = existsNow ? info.lastModified() : QDateTime();

    const bool existenceChanged = (prev.exists_ != existsNow);
    const bool sizeChanged = existsNow && prev.exists_ && (prev.size_ != sizeNow);
    const bool modifiedChanged = existsNow && prev.exists_ && (prev.modified_ != modifiedNow);

    if (existenceChanged)
    {
        if (existsNow)
        {
            emit FileCreated(path);
        } else
        {
            emit FileRemoved(path);
        }
    } else if (sizeChanged || modifiedChanged)
    {
        emit FileChanged(path, sizeNow);
    }

    prev.exists_ = existsNow;
    prev.size_ = sizeNow;
    prev.modified_ = modifiedNow;
}