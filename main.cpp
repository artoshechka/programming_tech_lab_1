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

    QString path;

    if (argc > 1)
    {
        path = argv[1];
    }
    else
    {
        cout << "Enter file path to observe: ";
        cout.flush();
        path = cin.readLine();
    }

    QFileInfo info(path);
    if (!info.exists())
    {
        cout << "Error: No such file: " << path << "\n";
        return 1;
    }

    observer->AddFile(path);

    observer->Start();

    cout << "Started observing file: " << path << "\n";
    cout << "Press Ctrl+C to exit.\n";

    return app.exec();
}