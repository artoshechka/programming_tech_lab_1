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

    QString path;

    if (argc > 1)
        path = argv[1];
    else
    {
        cout << "Enter file path: ";
        cout.flush();
        path = cin.readLine();
    }

    QFileInfo info(path);

    while (true)
    {
        if (!info.exists())
        {
            cout << "No such file: " + path<<"\n";
            return 0;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
        cout << info.size()<<"\n";
    }

    return 0;
}