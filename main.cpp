/// @file
/// @brief Точка входа в программу
/// @author Artemenko Anton
#include <file_observer.hpp>
#include <logger.hpp>

#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>

#include <chrono>
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
    }

    return app.exec();
}