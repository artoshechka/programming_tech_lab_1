/// @file
/// @brief Точка входа в программу
/// @author Artemenko Anton
#include <QCoreApplication>
#include <QSet>
#include <QSocketNotifier>
#include <QTextStream>
#include <QVector>
#include <atomic>
#include <cstdio>
#include <file_observer.hpp>
#include <file_watcher_factory.hpp>
#include <logger_factory.hpp>
#include <logger_macros.hpp>
#include <thread>

namespace
{
/// @brief Выводит список доступных команд интерактивной консоли.
/// @param[in,out] cout поток вывода в консоль
void PrintHelp(QTextStream& cout)
{
    cout << "Commands:\n";
    cout << "  add <path>    - add file to monitoring\n";
    cout << "  remove <path> - remove file from monitoring\n";
    cout << "  list          - show all monitored files\n";
    cout << "  quit          - exit program\n";
}

/// @brief Считывает начальный список файлов для наблюдения из консоли.
/// @param[in,out] cin поток ввода из консоли
/// @param[in,out] cout поток вывода в консоль
/// @return список путей, введенных пользователем
QVector<QString> ReadInitialPaths(QTextStream& cin, QTextStream& cout)
{
    QVector<QString> paths;

    cout << "Enter file paths to observe (empty line to finish):\n";
    while (true)
    {
        cout << "Path: ";
        cout.flush();

        const QString enteredPath = cin.readLine().trimmed();
        if (enteredPath.isEmpty())
        {
            break;
        }

        paths.append(enteredPath);
    }

    return paths;
}

/// @brief Обрабатывает одну команду, введенную пользователем в консоли.
/// @param[in] line исходная строка команды
/// @param[in,out] cout поток вывода в консоль
/// @param[in] observer объект наблюдателя файлов
/// @param[in] appLogger логгер приложения
/// @param[in,out] app экземпляр приложения для завершения по команде `quit`
/// @param[in,out] shouldExit флаг для остановки потока консоли
void ProcessConsoleCommand(const QString& line, QTextStream& cout,
                           const std::shared_ptr<file_observer::FileObserver>& observer,
                           const std::shared_ptr<logger::ILogger>& appLogger, QCoreApplication& app,
                           std::atomic<bool>& shouldExit)
{
    const QString trimmedLine = line.trimmed();
    if (trimmedLine.isEmpty())
    {
        return;
    }

    if (trimmedLine.toLower() == "help")
    {
        PrintHelp(cout);
    } else if (trimmedLine.toLower() == "list")
    {
        const auto files = observer->ListAllFiles();
        if (files.isEmpty())
        {
            LogInfo(appLogger) << "No files are monitored.";
        } else
        {
            LogInfo(appLogger) << "Monitored files:";
            for (const auto& path : files)
            {
                LogInfo(appLogger) << "  " << path;
            }
        }
        cout.flush();
    } else if (trimmedLine.toLower() == "quit" || trimmedLine.toLower() == "exit")
    {
        LogInfo(appLogger) << "Monitoring stopped by user command.";
        shouldExit = true;
        app.exit();
    } else if (trimmedLine.toLower() == "add" || trimmedLine.startsWith("add "))
    {
        const QString commandPath = trimmedLine.mid(4).trimmed();
        if (commandPath.isEmpty())
        {
            cout << "Usage: add <path>\n";
        } else
        {
            if (observer->AddFile(commandPath))
            {
                LogInfo(appLogger) << "Added file to monitoring: " << commandPath;
            } else
            {
                LogWarning(appLogger) << "Failed to add file to monitoring: " << commandPath;
            }
        }
    } else if (trimmedLine.toLower() == "remove" || trimmedLine.startsWith("remove "))
    {
        const QString commandPath = trimmedLine.mid(7).trimmed();
        if (commandPath.isEmpty())
        {
            cout << "Usage: remove <path>\n";
        } else
        {
            if (observer->RemoveFile(commandPath))
            {
                LogInfo(appLogger) << "Removed file from monitoring: " << commandPath;
            } else
            {
                LogWarning(appLogger) << "Failed to remove file from monitoring: " << commandPath;
            }
        }
    } else
    {
        LogWarning(appLogger) << "Unknown console command: " << trimmedLine
                              << ". Type 'help' to see available commands.";
    }

    cout.flush();
}
}  // namespace

/// @brief Точка входа в программу.
/// @param[in] argc количество аргументов командной строки
/// @param[in] argv массив аргументов командной строки
/// @return код завершения приложения
int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream cin(stdin);
    QTextStream cout(stdout);

    QString observerLogPath;
    QString appLogPath;

    for (int i = 1; i < argc; ++i)
    {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg.startsWith("--app-log-path="))
        {
            appLogPath = arg.mid(QString("--app-log-path=").length()).trimmed();
        } else if (arg.startsWith("--observer-log-path="))
        {
            observerLogPath = arg.mid(QString("--observer-log-path=").length()).trimmed();
        }
    }

    auto appLogger = logger::GetLogger<logger::AppSysLoggerTag>();
    auto observerLogger = logger::GetLogger<logger::AppLoggerTag>();

    const logger::LoggerSettings appLoggerSettings(
        appLogPath.isEmpty() ? QString() : appLogPath, logger::LogLevel::Debug,
        appLogPath.isEmpty() ? logger::LogOutput::Console : logger::LogOutput::File);

    const logger::LoggerSettings observerLoggerSettings(
        observerLogPath.isEmpty() ? QString() : observerLogPath, logger::LogLevel::Debug,
        observerLogPath.isEmpty() ? logger::LogOutput::Console : logger::LogOutput::File);

    appLogger->SetSettings(appLoggerSettings);
    observerLogger->SetSettings(observerLoggerSettings);

    constexpr int kDefaultIntervalMs = 1000;
    auto watcher =
        file_observer::CreateFileWatcher<file_observer::PollingWatcherTag>(kDefaultIntervalMs, observerLogger);
    const auto observer = std::make_shared<file_observer::FileObserver>(std::move(watcher), observerLogger);

    const QVector<QString> paths = ReadInitialPaths(cin, cout);

    for (const auto& path : paths)
    {
        if (observer->AddFile(path))
        {
            LogInfo(appLogger) << "Added file to monitoring: " << path;
        } else
        {
            LogWarning(appLogger) << "Failed to add file to monitoring: " << path;
        }
    }

    LogInfo(appLogger) << "Monitoring started";
    LogInfo(appLogger) << "Commands: add <path>, remove <path>, list, help, quit";

    std::atomic<bool> shouldExit{false};

    std::thread consoleThread([&]() {
        while (!shouldExit)
        {
            QString line;
            try
            {
                line = cin.readLine();
                if (line.isNull() && shouldExit)
                {
                    break;
                }

                QMetaObject::invokeMethod(
                    &app, [&, line]() { ProcessConsoleCommand(line, cout, observer, appLogger, app, shouldExit); },
                    Qt::QueuedConnection);
            } catch (const std::exception& e)
            {
                LogError(appLogger) << "Unexpected error occurred: " << e.what();
                break;
            } catch (...)
            {
                LogError(appLogger) << "Unknown error occurred";
                break;
            }
        }
    });

    int result = app.exec();

    shouldExit = true;
    if (consoleThread.joinable())
    {
        consoleThread.join();
    }

    return result;
}