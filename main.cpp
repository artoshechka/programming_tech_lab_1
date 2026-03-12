#include <file_observer.hpp>
#include <logger_factory.hpp>

#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>

#include <chrono>
#include <thread>

#include <logger_macros.hpp>
/// @brief Точка входа в программу.
/// @param[in] argc количество аргументов командной строки
/// @param[in] argv массив аргументов командной строки
/// @return код завершения приложения
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream cin(stdin);
    QTextStream cout(stdout);

    logger::LoggerOutputMode logOutputMode = logger::LoggerOutputMode::Console;
    QString path;

    for (int i = 1; i < argc; ++i)
    {
        const QString arg = argv[i];
        if (arg == "--log-output=file")
        {
            logOutputMode = logger::LoggerOutputMode::File;
        }
        else if (arg == "--log-output=console")
        {
            logOutputMode = logger::LoggerOutputMode::Console;
        }
        else if (!arg.startsWith("--") && path.isEmpty())
        {
            path = arg;
        }
    }

    logger::SetGlobalLogOutput(logOutputMode);

    auto appLogger = logger::GetAppLogger();

    // Создаем наблюдатель за файлами
    auto observer = std::make_shared<file_observer::FileObserver>();

    if (path.isEmpty())
    {
        cout << "Enter file path to observe: ";
        cout.flush();
        path = cin.readLine();
    }

    QFileInfo info(path);
    if (!info.exists())
    {
        LogError(appLogger) << "No such file: " << path;
        cout << "Error: No such file: " << path << "\n";
        return 1;
    }

    observer->AddFile(path);

    observer->Start();

    LogInfo(appLogger) << "Started observing file: " << path;
    cout << "Started observing file: " << path << "\n";
    cout << "Press Ctrl+C to exit.\n";

    return app.exec();
}