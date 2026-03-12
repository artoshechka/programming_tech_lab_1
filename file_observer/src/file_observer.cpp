/// @file
/// @brief Определение класса, ответственного за мониторинг директорий
/// @author Artemenko Anton

#include <file_observer.hpp>
#include <logger_factory.hpp>
#include <logger_macros.hpp>

using file_observer::FileObserver;

FileObserver::FileObserver(QObject *parent) : QObject(parent)
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

    auto observerLogger = logger::GetObserverLogger();

    // Добавляем файл в наблюдатель
    if (!systemWatcher_.files().contains(filePath))
    {
        systemWatcher_.addPath(filePath);
    }

    // Сохраняем информацию о файле
    QFileInfo fileInfo(filePath);
    fileContainer_[filePath] = fileInfo;

    LogInfo(observerLogger) << "File: " << fileInfo.absoluteFilePath() << " added under observing.";
}

void FileObserver::RemoveFile(const QString &filePath)
{
    auto observerLogger = logger::GetObserverLogger();

    if (systemWatcher_.files().contains(filePath))
    {
        systemWatcher_.removePath(filePath);
        LogInfo(observerLogger) << "File: " << filePath << " removed from observing.";
    }
    fileContainer_.remove(filePath);
}

FileObserver::~FileObserver()
{
    watchTimer_.stop();

    // Очищаем наблюдатель
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
    auto observerLogger = logger::GetObserverLogger();

    auto it = fileContainer_.find(filePath);
    if (it == fileContainer_.end())
        return;

    QFileInfo currentInfo(filePath);
    const QFileInfo &oldInfo = it.value();

    bool existsChanged = (currentInfo.exists() != oldInfo.exists());
    bool sizeChanged = (currentInfo.exists() && oldInfo.exists() && currentInfo.size() != oldInfo.size());

    if (existsChanged || sizeChanged)
    {
        LogInfo(observerLogger) << "File: " << currentInfo.absoluteFilePath()
                                << " changed. Exists: " << currentInfo.exists()
                                << ", size: " << currentInfo.size();
        it.value() = currentInfo;
    }
}