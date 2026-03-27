#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>
#include <gtest/gtest.h>
#include <memory>

#include <file_observer.hpp>
#include <src/polling_file_watcher.hpp>

using file_observer::FileObserver;
using file_observer::PollingFileWatcher;

class FileObserverTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        if (!QCoreApplication::instance())
        {
            int argc = 0;
            char *argv[] = {nullptr};
            app_ = new QCoreApplication(argc, argv);
        }

        // Создаем реальный watcher с интервалом 50ms для быстрых тестов
        auto watcher = std::make_shared<PollingFileWatcher>(50, nullptr);
        observer_ = std::make_unique<FileObserver>(watcher, nullptr);
    }

    void TearDown() override
    {
        observer_.reset();

        for (auto &file : tempFiles_)
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

    QTemporaryFile *CreateTempFile(const QString &content = QString())
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

    void ModifyFile(const QString &path, const QString &newContent)
    {
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            file.write(newContent.toUtf8());
            file.close();
        }
    }

    std::unique_ptr<FileObserver> observer_;
    QList<QTemporaryFile *> tempFiles_;
    QCoreApplication *app_ = nullptr;
};

TEST_F(FileObserverTest, AddFile_ValidPath_AddsToWatcher)
{
    QTemporaryFile *tempFile = CreateTempFile("test content");

    observer_->AddFile(tempFile->fileName());

    QStringList files = observer_->ListAllFiles();
    EXPECT_EQ(files.size(), 1);
    EXPECT_TRUE(files.contains(tempFile->fileName()));
}

TEST_F(FileObserverTest, RemoveFile_ExistingPath_RemovesFromWatcher)
{
    QTemporaryFile *tempFile = CreateTempFile("test content");

    observer_->AddFile(tempFile->fileName());
    observer_->RemoveFile(tempFile->fileName());

    EXPECT_TRUE(observer_->ListAllFiles().isEmpty());
}

TEST_F(FileObserverTest, ListAllFiles_ReturnsAllWatchedFiles)
{
    QTemporaryFile *file1 = CreateTempFile();
    QTemporaryFile *file2 = CreateTempFile();
    QTemporaryFile *file3 = CreateTempFile();

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
    QTemporaryFile *tempFile = CreateTempFile("content");

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
        QTemporaryFile *tempFile = CreateTempFile("content" + QString::number(i));
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
        QTemporaryFile *tempFile = CreateTempFile("content" + QString::number(i));
        QString path = tempFile->fileName();
        paths.append(path);
        observer_->AddFile(path);
    }

    EXPECT_EQ(observer_->ListAllFiles().size(), 6);

    for (const QString &path : observer_->ListAllFiles())
    {
        observer_->RemoveFile(path);
    }

    EXPECT_TRUE(observer_->ListAllFiles().isEmpty());
}