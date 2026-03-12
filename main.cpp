/// @file
/// @brief Точка входа в программу
/// @author Artemenko Anton
#include <file_observer.hpp>
#include <logger.hpp>

#include <QCoreApplication>
#include <QFileInfo>
#include <QSet>
#include <QSocketNotifier>
#include <QTextStream>

#include <chrono>
#include <cstdio>
#include <thread>

/// @brief Точка входа в программу.
/// @param[in] argc количество аргументов командной строки
/// @param[in] argv массив аргументов командной строки
/// @return код завершения приложения
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream cin(stdin);
    QTextStream cout(stdout);

    const auto &logger = logger::Logger::Instance();

    auto observer = std::make_shared<file_observer::FileObserver>();
    QSet<QString> observedPaths;

    cout << "Enter file paths to observe (empty line to finish):\n";

    QVector<QString> paths;
    while (true)
    {
        cout << "Path: ";
        cout.flush();

        QString path = cin.readLine().trimmed();

        if (path.isEmpty())
            break;
        paths.append(path);
    }

    for (const auto &path : paths)
    {
        observer->AddFile(path);
        observedPaths.insert(path);
    }

    cout << "Monitoring started" << "\n";
    cout << "Commands:" << " add <path>, remove <path>, list, help, quit\n";
    cout << "Type command and press Enter.\n";
    cout.flush();

    auto stdinNotifier = std::make_shared<QSocketNotifier>(fileno(stdin), QSocketNotifier::Read);
    QObject::connect(stdinNotifier.get(), &QSocketNotifier::activated, &app, [&, observer]() {
        QString line = cin.readLine().trimmed();
        if (line.isEmpty())
        {
            return;
        }

        if (line == "help")
        {
            cout << "Commands:\n";
            cout << "  add <path>    - add file to monitoring\n";
            cout << "  remove <path> - remove file from monitoring\n";
            cout << "  list          - show all monitored files\n";
            cout << "  quit          - exit program\n";
        }
        else if (line == "list")
        {
            if (observedPaths.isEmpty())
            {
                cout << "No files are monitored.\n";
            }
            else
            {
                cout << "Monitored files:\n";
                for (const auto &path : observedPaths)
                {
                    cout << "  " << path << "\n";
                }
            }
        }
        else if (line == "quit" || line == "exit")
        {
            app.quit();
            return;
        }
        else if (line == "add" || line.startsWith("add "))
        {
            QString path = line.mid(4).trimmed();
            if (path.isEmpty())
            {
                cout << "Usage: add <path>\n";
            }
            else
            {
                observer->AddFile(path);
                observedPaths.insert(path);
            }
        }
        else if (line == "remove" || line.startsWith("remove "))
        {
            QString path = line.mid(7).trimmed();
            if (path.isEmpty())
            {
                cout << "Usage: remove <path>\n";
            }
            else
            {
                observer->RemoveFile(path);
                observedPaths.remove(path);
            }
        }
        else
        {
            cout << "Unknown command: " << line << "\n";
            cout << "Type 'help' to see available commands.\n";
        }

        cout.flush();
    });

    return app.exec();
}