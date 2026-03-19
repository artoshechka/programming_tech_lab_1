/// @file
/// @brief Точка входа в программу
/// @author Artemenko Anton
#include <file_observer.hpp>
#include <logger_factory.hpp>
#include <logger_macros.hpp>

#include <QCoreApplication>
#include <QSet>
#include <QSocketNotifier>
#include <QTextStream>
#include <QVector>

#include <cstdio>
#include <thread>

namespace
{
/// @brief Выводит список доступных команд интерактивной консоли.
/// @param[in,out] cout поток вывода в консоль
void PrintHelp(QTextStream &cout)
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
QVector<QString> ReadInitialPaths(QTextStream &cin, QTextStream &cout)
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

/// @brief Выводит текущий список наблюдаемых файлов.
/// @param[in] observedPaths множество наблюдаемых путей
/// @param[in,out] cout поток вывода в консоль
void PrintObservedPaths(const QSet<QString> &observedPaths, QTextStream &cout)
{
    if (observedPaths.isEmpty())
    {
        cout << "No files are monitored.\n";
        return;
    }

    cout << "Monitored files:\n";
    for (const auto &path : observedPaths)
    {
        cout << "  " << path << "\n";
    }
}

/// @brief Обрабатывает одну команду, введенную пользователем в консоли.
/// @param[in] line исходная строка команды
/// @param[in,out] cout поток вывода в консоль
/// @param[in,out] observedPaths множество наблюдаемых путей
/// @param[in] observer объект наблюдателя файлов
/// @param[in] appLogger логгер приложения
/// @param[in,out] app экземпляр приложения для завершения по команде `quit`
void ProcessConsoleCommand(const QString &line, QTextStream &cout, QSet<QString> &observedPaths,
                           const std::shared_ptr<file_observer::FileObserver> &observer,
                           const std::shared_ptr<logger::ILogger> &appLogger, QCoreApplication &app)
{
    const QString trimmedLine = line.trimmed();
    if (trimmedLine.isEmpty())
    {
        return;
    }

    if (trimmedLine == "help")
    {
        PrintHelp(cout);
    }
    else if (trimmedLine == "list")
    {
        PrintObservedPaths(observedPaths, cout);
    }
    else if (trimmedLine == "quit" || trimmedLine == "exit")
    {
        LogInfo(appLogger) << "Monitoring stopped by user command.";
        app.quit();
    }
    else if (trimmedLine == "add" || trimmedLine.startsWith("add "))
    {
        const QString commandPath = trimmedLine.mid(4).trimmed();
        if (commandPath.isEmpty())
        {
            cout << "Usage: add <path>\n";
        }
        else
        {
            observer->AddFile(commandPath);
            observedPaths.insert(commandPath);
            LogInfo(appLogger) << "Added file to monitoring: " << commandPath;
        }
    }
    else if (trimmedLine == "remove" || trimmedLine.startsWith("remove "))
    {
        const QString commandPath = trimmedLine.mid(7).trimmed();
        if (commandPath.isEmpty())
        {
            cout << "Usage: remove <path>\n";
        }
        else
        {
            observer->RemoveFile(commandPath);
            observedPaths.remove(commandPath);
            LogInfo(appLogger) << "Removed file from monitoring: " << commandPath;
        }
    }
    else
    {
        cout << "Unknown command: " << trimmedLine << "\n";
        cout << "Type 'help' to see available commands.\n";
        LogWarning(appLogger) << "Unknown console command: " << trimmedLine;
    }

    cout.flush();
}
} // namespace

/// @brief Точка входа в программу.
/// @param[in] argc количество аргументов командной строки
/// @param[in] argv массив аргументов командной строки
/// @return код завершения приложения
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream cin(stdin);
    QTextStream cout(stdout);

    auto appLogger = logger::GetLogger<logger::AppLoggerTag>();
    auto observerLogger = logger::GetLogger<logger::ObserverLoggerTag>();

    const logger::LoggerSettings appLoggerSettings("logs/app.log", logger::LogLevel::Debug, logger::LogOutput::Console);
    const logger::LoggerSettings observerLoggerSettings("logs/observer.log", logger::LogLevel::Debug,
                                                        logger::LogOutput::Console);

    appLogger->SetSettings(appLoggerSettings);
    observerLogger->SetSettings(observerLoggerSettings);

    auto observer = std::make_shared<file_observer::FileObserver>(observerLogger);
    QSet<QString> observedPaths;
    const QVector<QString> paths = ReadInitialPaths(cin, cout);

    for (const auto &path : paths)
    {
        observer->AddFile(path);
        observedPaths.insert(path);
        LogInfo(appLogger) << "Added file to monitoring: " << path;
    }

    if (observedPaths.isEmpty())
    {
        cout << "No files were added. Use command: add <path>\n";
    }

    cout << "Monitoring started\n";
    cout << "Commands: add <path>, remove <path>, list, help, quit\n";
    cout << "Type command and press Enter.\n";
    cout.flush();

    LogInfo(appLogger) << "Monitoring started. Initial files count: " << observedPaths.size();

#ifdef _WIN32
    std::thread consoleThread([&]() {
        while (true)
        {
            const QString line = cin.readLine();
            QMetaObject::invokeMethod(
                &app, [&, line]() { ProcessConsoleCommand(line, cout, observedPaths, observer, appLogger, app); },
                Qt::QueuedConnection);
        }
    });

    consoleThread.detach();
#else
    auto stdinNotifier = std::make_shared<QSocketNotifier>(fileno(stdin), QSocketNotifier::Read);
    QObject::connect(stdinNotifier.get(), &QSocketNotifier::activated, &app, [&, observer, appLogger]() {
        const QString line = cin.readLine();
        ProcessConsoleCommand(line, cout, observedPaths, observer, appLogger, app);
    });
#endif

    return app.exec();
}
