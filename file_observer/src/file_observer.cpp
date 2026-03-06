/// @file
/// @brief Определение класса, ответственного за мониторинг директорий
/// @author Artemenko Anton

#include <file_observer.hpp>
#include <logger_macros.hpp>

using file_observer::FileObserver;

FileObserver::FileObserver(QObject *parent) : QObject(parent)
{
    watchTimer_.setInterval(CHECK_INTERVAL_MS);
    connect(&watchTimer_, &QTimer::timeout, this, &FileObserver::CheckFiles);
    connect(&systemWatcher_, &QFileSystemWatcher::fileChanged, this, &FileObserver::OnFileChanged);
}

void FileObserver::AddFile(const QString &filePath)
{
    if (filePath.isEmpty())
        return;

    if (!systemWatcher_.files().contains(filePath))
    {
        systemWatcher_.addPath(filePath);
    }

    QFileInfo fileInfo(filePath);
    fileContainer_[filePath] = fileInfo;

    LogInfo("File: " + fileInfo.absoluteFilePath() + " added under observing!");

    CheckFileChanges(filePath);
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

FileObserver::~FileObserver()
{
    watchTimer_.stop();

    if (!systemWatcher_.files().isEmpty())
    {
        systemWatcher_.removePaths(systemWatcher_.files());
    }
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
    for (auto it = fileContainer_.cbegin(); it != fileContainer_.cend(); ++it)
    {
        CheckFileChanges(it.key());
    }
}

void FileObserver::OnFileChanged(const QString &path)
{
    if (!systemWatcher_.files().contains(path))
    {
        systemWatcher_.addPath(path);
    }

    CheckFileChanges(path);
}

void FileObserver::CheckFileChanges(const QString &filePath)
{
    auto it = fileContainer_.find(filePath);
    if (it == fileContainer_.end())
        return;

    QFileInfo currentInfo(filePath);
    QFileInfo &oldInfo = it.value();

    // Файл не существует
    if (!currentInfo.exists())
    {
        if (oldInfo.exists())
        {
            LogInfo("File: " + filePath + " does not exist.");
        }

        oldInfo = currentInfo;
        return;
    }

    // Файл существует
    if (oldInfo.exists())
    {
        // Проверка изменения размера
        if (currentInfo.size() != oldInfo.size())
        {
            LogInfo("File: " + filePath + " was changed. New size: " + QString::number(currentInfo.size()));
        }
    }
    else
    {
        // Файл появился
        LogInfo("File: " + filePath + " appeared. Size: " + QString::number(currentInfo.size()));
    }

    // Файл существует и не пустой
    if (currentInfo.exists())
    {
        LogInfo("File: " + filePath + " exists. Size: " + QString::number(currentInfo.size()));
    }

    oldInfo = currentInfo;
}