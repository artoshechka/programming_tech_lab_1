/// @file
/// @brief Тесты для polling_watcher'a
/// @author Artemenko Anton
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>
#include <QTimer>
#include <memory>
#include <src/polling_file_watcher.hpp>

using file_observer::PollingFileWatcher;
class PollingFileWatcherTest : public ::testing::Test
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

        logger_ = nullptr;
    }

    void TearDown() override
    {
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

    bool WaitForSignal(QSignalSpy& spy, int timeoutMs = 1000)
    {
        QElapsedTimer timer;
        timer.start();

        while (spy.count() == 0 && timer.elapsed() < timeoutMs)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
            QTest::qWait(10);
        }

        return spy.count() > 0;
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

    std::shared_ptr<logger::ILogger> logger_;
    QList<QTemporaryFile*> tempFiles_;
    QCoreApplication* app_ = nullptr;
};

TEST_F(PollingFileWatcherTest, AddFile_ValidPath_AddsToWatchList)
{
    PollingFileWatcher watcher(100, logger_);
    QTemporaryFile* tempFile = CreateTempFile("test content");

    watcher.AddFile(tempFile->fileName());

    QStringList files = watcher.ListFiles();
    EXPECT_EQ(files.size(), 1);
    EXPECT_TRUE(files.contains(tempFile->fileName()));
}

TEST_F(PollingFileWatcherTest, AddFile_EmptyPath_DoesNothing)
{
    PollingFileWatcher watcher(100, logger_);

    watcher.AddFile("");

    EXPECT_TRUE(watcher.ListFiles().isEmpty());
}

TEST_F(PollingFileWatcherTest, AddFile_DuplicatePath_UpdatesState)
{
    PollingFileWatcher watcher(100, logger_);
    QTemporaryFile* tempFile = CreateTempFile("content");

    watcher.AddFile(tempFile->fileName());
    watcher.AddFile(tempFile->fileName());

    QStringList files = watcher.ListFiles();
    EXPECT_EQ(files.size(), 1);
    EXPECT_TRUE(files.contains(tempFile->fileName()));
}

TEST_F(PollingFileWatcherTest, RemoveFile_ExistingPath_RemovesFromWatchList)
{
    PollingFileWatcher watcher(100, logger_);
    QTemporaryFile* tempFile = CreateTempFile("test content");

    watcher.AddFile(tempFile->fileName());
    watcher.RemoveFile(tempFile->fileName());

    EXPECT_TRUE(watcher.ListFiles().isEmpty());
}

TEST_F(PollingFileWatcherTest, CheckFiles_FileCreated_EmitsFileCreated)
{
    PollingFileWatcher watcher(50, logger_);
    QTemporaryFile* tempFile = CreateTempFile();
    QString path = tempFile->fileName();

    delete tempFile;
    tempFiles_.removeOne(tempFile);

    QSignalSpy createdSpy(&watcher, &PollingFileWatcher::FileCreated);

    watcher.AddFile(path);

    QCoreApplication::processEvents();
    QTest::qWait(60);

    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write("test");
    file.close();

    ASSERT_TRUE(WaitForSignal(createdSpy, 500));
    EXPECT_EQ(createdSpy.count(), 1);
    EXPECT_EQ(createdSpy.first().first().toString(), path);
}

TEST_F(PollingFileWatcherTest, CheckFiles_FileRemoved_EmitsFileRemoved)
{
    PollingFileWatcher watcher(50, logger_);
    QTemporaryFile* tempFile = CreateTempFile("test content");
    QString path = tempFile->fileName();

    QSignalSpy removedSpy(&watcher, &PollingFileWatcher::FileRemoved);

    watcher.AddFile(path);

    QCoreApplication::processEvents();
    QTest::qWait(60);

    delete tempFile;
    tempFiles_.removeOne(tempFile);

    ASSERT_TRUE(WaitForSignal(removedSpy, 500));
    EXPECT_EQ(removedSpy.count(), 1);
    EXPECT_EQ(removedSpy.first().first().toString(), path);
}

TEST_F(PollingFileWatcherTest, CheckFiles_MultipleFiles_EmitsCorrectSignals)
{
    PollingFileWatcher watcher(50, logger_);

    QTemporaryFile* file1 = CreateTempFile("content1");
    QTemporaryFile* file2 = CreateTempFile("content2");
    QString path1 = file1->fileName();
    QString path2 = file2->fileName();

    QSignalSpy changedSpy(&watcher, &PollingFileWatcher::FileChanged);

    watcher.AddFile(path1);
    watcher.AddFile(path2);

    QCoreApplication::processEvents();
    QTest::qWait(60);

    ModifyFile(path1, "new content1");
    ModifyFile(path2, "new content2");

    ASSERT_TRUE(WaitForSignal(changedSpy, 500));
    EXPECT_GE(changedSpy.count(), 1);
}

TEST_F(PollingFileWatcherTest, CheckFiles_CreateThenModify_EmitsBothSignals)
{
    PollingFileWatcher watcher(50, logger_);
    QTemporaryFile* tempFile = CreateTempFile();
    QString path = tempFile->fileName();

    delete tempFile;
    tempFiles_.removeOne(tempFile);

    QSignalSpy createdSpy(&watcher, &PollingFileWatcher::FileCreated);
    QSignalSpy changedSpy(&watcher, &PollingFileWatcher::FileChanged);

    watcher.AddFile(path);

    QCoreApplication::processEvents();
    QTest::qWait(60);

    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write("initial");
    file.close();

    ASSERT_TRUE(WaitForSignal(createdSpy, 500));
    EXPECT_EQ(createdSpy.count(), 1);

    createdSpy.clear();

    QTest::qWait(60);
    ModifyFile(path, "modified");

    ASSERT_TRUE(WaitForSignal(changedSpy, 500));
    EXPECT_EQ(changedSpy.count(), 1);
}

TEST_F(PollingFileWatcherTest, CheckFiles_SameSizeContentChange_EmitsFileChanged)
{
    PollingFileWatcher watcher(50, logger_);
    QTemporaryFile* tempFile = CreateTempFile("1");
    QString path = tempFile->fileName();

    QSignalSpy changedSpy(&watcher, &PollingFileWatcher::FileChanged);

    watcher.AddFile(path);

    QCoreApplication::processEvents();
    QTest::qWait(60);

    ModifyFile(path, "2");

    QTest::qWait(1100);

    ASSERT_TRUE(WaitForSignal(changedSpy, 500));
    EXPECT_EQ(changedSpy.count(), 1);
    EXPECT_EQ(changedSpy.first().first().toString(), path);
}

TEST_F(PollingFileWatcherTest, AddFile_NonExistentFile_AddsToWatchList)
{
    PollingFileWatcher watcher(50, logger_);
    QString nonExistentPath =
        QDir::tempPath() + "/nonexistent_test_file_" + QString::number(QDateTime::currentMSecsSinceEpoch());

    QSignalSpy createdSpy(&watcher, &PollingFileWatcher::FileCreated);

    watcher.AddFile(nonExistentPath);

    QCoreApplication::processEvents();
    QTest::qWait(60);

    QFile file(nonExistentPath);
    file.open(QIODevice::WriteOnly);
    file.write("test");
    file.close();

    ASSERT_TRUE(WaitForSignal(createdSpy, 500));
    EXPECT_EQ(createdSpy.count(), 1);

    file.remove();
}

TEST_F(PollingFileWatcherTest, ListFiles_ReturnsAllWatchedFiles)
{
    PollingFileWatcher watcher(100, logger_);
    QTemporaryFile* file1 = CreateTempFile();
    QTemporaryFile* file2 = CreateTempFile();
    QTemporaryFile* file3 = CreateTempFile();

    watcher.AddFile(file1->fileName());
    watcher.AddFile(file2->fileName());
    watcher.AddFile(file3->fileName());

    QStringList files = watcher.ListFiles();

    EXPECT_EQ(files.size(), 3);
    EXPECT_TRUE(files.contains(file1->fileName()));
    EXPECT_TRUE(files.contains(file2->fileName()));
    EXPECT_TRUE(files.contains(file3->fileName()));
}