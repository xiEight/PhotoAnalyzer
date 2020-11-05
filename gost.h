#ifndef GOST_H
#define GOST_H

#include <cstdint>
#include <string>
#include <fstream>
#include <functional>
#include <QDebug>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <bitset>
#include <cmath>
#include <experimental/filesystem>

#define SIMPLECRYPT 0
#define SIMPLEENCRYPT 1
#define GAMMA 2


class gost
{
public:
    gost();
    ~gost();
    //Инициализация ключа из файла
    void set_key(std::string filepath);
    //Начало шифрования
    //TODO: проверить, инициироан ли путь!
    void start(std::string inputFile, std::string outputFile, ushort mode = SIMPLECRYPT);

private:

    //Замена типа
    using byte = std::uint8_t;

    //Таблица замены
    std::uint8_t table[8][16] =
    {
                        {5,9,2,4,8,7,3,0x0a,0x0f,1,0x0d,0x0b,6,0x0e,0,0x0c}, //Таблица S
                        {7,0x0d,5,0x0c,2,0x0e,4,3,0,8,0x0a,9,1,6,0x0b,0x0f},
                        {0x0c,9,0x0b,4,0x0f,5,0x0a,1,0x0e,3,7,0x0d,8,6,0,2},
                        {0x0a,8,4,0,6,0x0f,0x0b,3,0x0c,0x0d,2,1,7,5,9,0x0e},
                        {8,0x0a,0x0f,6,9,0x0b,4,0x0c,7,1,0x0e,3,5,0,2,0x0d},
                        {0x0d,1,0x0a,0,5,8,2,0x0e,7,0x0f,9,0x0c,0x0b,3,6,4},
                        {0x0f,7,6,0x0a,0x0c,4,1,0x0b,0x0e,9,8,0,2,5,0x0d,3},
                        {0x0f,7,6,0x0a,0x0c,4,1,0x0b,0x0e,9,8,0,2,5,0x0d,3}
                                                                                    };

    //256 битовый ключ шифрования, разбитый на блоки по 32 бита
    std::uint32_t key[8];

    std::uint32_t concat(byte a, byte b, byte c, byte d);

    //Группировка 4 байт
    std::uint32_t concat(std::uint8_t arr[4]);

    //Входной файл
    std::string inputFile;

    //Выходной файловый поток
    std::ofstream output;

    std::mutex coutMutex;

    //Мбютекс буффера
    std::mutex queueMutex;
    //Услованя переменная для блокировки потока шифрования, если буффер пуст
    std::condition_variable cv;

    //Очередь буфер для шифрования
    std::queue<std::pair<std::uint32_t,std::uint32_t>> *buffQueue; //buffer. First - lower, Second - Higher

    void reading(); //Поточная функция для чтения файла
    //Поточная функция для шифрования файла
    void crypt();
    //Функция для записи блока байт в файл
    //TODO: разбить данные на чанки!
    void write(std::pair<std::uint32_t,std::uint32_t> buff);
    //Флаг завершения чтения файла
    bool complete = false;

    template<typename INT>
    inline INT rol(INT var, size_t shift)
    {
        return (var << shift) | (var >> (sizeof(var)*CHAR_BIT-shift));
    }


    std::pair<std::uint32_t, std::uint32_t> oneStepCrypto(std::pair<std::uint32_t, std::uint32_t> buf, size_t &round);

    std::uint32_t rol(std::uint32_t x, size_t shift);

    unsigned short mode;

    std::uint32_t sync;
};

#endif // GOST_H
