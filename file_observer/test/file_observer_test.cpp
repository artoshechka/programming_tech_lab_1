#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QSet>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>
#include <file_observer.hpp>
#include <ilogger.hpp>
#include <memory>
#include <src/polling_file_watcher.hpp>

using file_observer::FileObserver;
using file_observer::PollingFileWatcher;

class RecordingLogger final : public logger::ILogger
{
   public:
    RecordingLogger() : settings_()
    {
    }

    void SetSettings(const logger::LoggerSettings& settings) override
    {
        settings_ = settings;
    }

    logger::LoggerSettings GetSettings() const override
    {
        return settings_;
    }

    void Log(logger::LogLevel level, const QString& message, const char* file = nullptr, int line = 0,
             const char* function = nullptr) override
    {
        levels_.append(level);
        messages_.append(message);
        files_.append(file != nullptr ? QString::fromUtf8(file) : QString());
        lines_.append(line);
        functions_.append(function != nullptr ? QString::fromUtf8(function) : QString());
    }

    int Count() const
    {
        return messages_.size();
    }

    QString LastMessage() const
    {
        return messages_.isEmpty() ? QString() : messages_.last();
    }

   private:
    logger::LoggerSettings settings_;
    QList<logger::LogLevel> levels_;
    QStringList messages_;
    QStringList files_;
    QList<int> lines_;
    QStringList functions_;
};

class FakeFileWatcher final : public file_observer::IFileWatcher
{
   public:
    explicit FakeFileWatcher(QObject* parent = nullptr) : IFileWatcher(parent)
    {
    }

    void AddFile(const QString& path) override
    {
        addedPaths_.append(path);
        files_.insert(path);
    }

    void RemoveFile(const QString& path) override
    {
        removedPaths_.append(path);
        files_.remove(path);
    }

    QStringList ListFiles() const override
    {
        return QStringList(files_.values());
    }

    int AddCalls() const
    {
        return addedPaths_.size();
    }

    int RemoveCalls() const
    {
        return removedPaths_.size();
    }

    bool Contains(const QString& path) const
    {
        return files_.contains(path);
    }

    void EmitChanged(const QString& path, qint64 size)
    {
        emit FileChanged(path, size);
    }

    void EmitCreated(const QString& path)
    {
        emit FileCreated(path);
    }

    void EmitRemoved(const QString& path)
    {
        emit FileRemoved(path);
    }

   private:
    QSet<QString> files_;
    QStringList addedPaths_;
    QStringList removedPaths_;
};

class FileObserverTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        if (!QCoreApplication::instance())
        {
            int argc = 0;
            char* argv[] = {nullptr};
            app_ = new QCoreApplication(argc, argv);
        }

        // Создаем реальный watcher с интервалом 50ms для быстрых тестов
        auto watcher = std::make_unique<PollingFileWatcher>(50, nullptr);
        observer_ = std::make_unique<FileObserver>(std::move(watcher), nullptr);
    }

    void TearDown() override
    {
        observer_.reset();

        for (auto& file : tempFiles_)
        {
            delete file;
        }
        tempFiles_.clear();

        if (app_)
        {
            delete app_;
            app_ = nullptr;
        }
    }

    QTemporaryFile* CreateTempFile(const QString& content = QString())
    {
        auto file = new QTemporaryFile();
        file->open();

        if (!content.isEmpty())
        {
            file->write(content.toUtf8());
            file->flush();
        }

        tempFiles_.append(file);
        return file;
    }

    void ModifyFile(const QString& path, const QString& newContent)
    {
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            file.write(newContent.toUtf8());
            file.close();
        }
    }

    std::unique_ptr<FileObserver> observer_;
    QList<QTemporaryFile*> tempFiles_;
    QCoreApplication* app_ = nullptr;
};

TEST_F(FileObserverTest, AddFile_ValidPath_AddsToWatcher)
{
    QTemporaryFile* tempFile = CreateTempFile("test content");

    observer_->AddFile(tempFile->fileName());

    QStringList files = observer_->ListAllFiles();
    EXPECT_EQ(files.size(), 1);
    EXPECT_TRUE(files.contains(tempFile->fileName()));
}

TEST_F(FileObserverTest, RemoveFile_ExistingPath_RemovesFromWatcher)
{
    QTemporaryFile* tempFile = CreateTempFile("test content");

    observer_->AddFile(tempFile->fileName());
    observer_->RemoveFile(tempFile->fileName());

    EXPECT_TRUE(observer_->ListAllFiles().isEmpty());
}

TEST_F(FileObserverTest, ListAllFiles_ReturnsAllWatchedFiles)
{
    QTemporaryFile* file1 = CreateTempFile();
    QTemporaryFile* file2 = CreateTempFile();
    QTemporaryFile* file3 = CreateTempFile();

    observer_->AddFile(file1->fileName());
    observer_->AddFile(file2->fileName());
    observer_->AddFile(file3->fileName());

    QStringList files = observer_->ListAllFiles();

    EXPECT_EQ(files.size(), 3);
    EXPECT_TRUE(files.contains(file1->fileName()));
    EXPECT_TRUE(files.contains(file2->fileName()));
    EXPECT_TRUE(files.contains(file3->fileName()));
}

TEST_F(FileObserverTest, AddFile_DuplicatePath_DoesNotDuplicate)
{
    QTemporaryFile* tempFile = CreateTempFile("content");

    observer_->AddFile(tempFile->fileName());
    observer_->AddFile(tempFile->fileName());

    QStringList files = observer_->ListAllFiles();
    EXPECT_EQ(files.size(), 1);
}

TEST_F(FileObserverTest, MultipleOperations_StressTest_NoErrors)
{
    QList<QString> paths;

    for (int i = 0; i < 5; ++i)
    {
        QTemporaryFile* tempFile = CreateTempFile("content" + QString::number(i));
        QString path = tempFile->fileName();
        paths.append(path);
        observer_->AddFile(path);
    }

    EXPECT_EQ(observer_->ListAllFiles().size(), 5);

    observer_->RemoveFile(paths[1]);
    observer_->RemoveFile(paths[3]);

    QStringList files = observer_->ListAllFiles();
    EXPECT_EQ(files.size(), 3);

    for (int i = 5; i < 8; ++i)
    {
        QTemporaryFile* tempFile = CreateTempFile("content" + QString::number(i));
        QString path = tempFile->fileName();
        paths.append(path);
        observer_->AddFile(path);
    }

    EXPECT_EQ(observer_->ListAllFiles().size(), 6);

    for (const QString& path : observer_->ListAllFiles())
    {
        observer_->RemoveFile(path);
    }

    EXPECT_TRUE(observer_->ListAllFiles().isEmpty());
}

TEST(FileObserverStandaloneTest, NullWatcher_LogsWarningsAndDoesNotCrash)
{
    auto logger = std::make_shared<RecordingLogger>();
    auto observer = std::make_unique<FileObserver>(std::unique_ptr<file_observer::IFileWatcher>{}, logger);

    observer->AddFile("/tmp/missing.txt");
    observer->RemoveFile("/tmp/missing.txt");

    EXPECT_TRUE(observer->ListAllFiles().isEmpty());
    EXPECT_EQ(logger->Count(), 4);
    EXPECT_TRUE(logger->LastMessage().contains("watcher is not set"));
}