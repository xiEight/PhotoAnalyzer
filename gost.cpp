#include "gost.h"

gost::gost()
{

}

std::uint32_t gost::rol(std::uint32_t x, std::size_t shift = 11)
{
    std::string s = std::bitset<32>(x).to_string();
    std::string f = s.substr(0,shift + 1);
    s.erase(0,shift + 1);
    return std::bitset<32>(s + f).to_ulong();
}

void gost::set_key(std::string filepath)
{
    std::ifstream file(filepath, std::ios_base::binary);
    std::uint8_t a,b,c,d;
    for (size_t i = 0; i < 8; i++)
    {
        file.read(reinterpret_cast<char*>(&a),1);
        file.read(reinterpret_cast<char*>(&b),1);
        file.read(reinterpret_cast<char*>(&c),1);
        file.read(reinterpret_cast<char*>(&d),1);
        key[i] = concat(a,b,c,d);

    }
}


std::uint32_t gost::concat(byte a, byte b, byte c, byte d)
{
    std::function<std::uint32_t(std::uint8_t)> func = [](std::uint8_t x) -> std::uint32_t
    {return static_cast<std::uint32_t>(x);};

    std::uint32_t result = func(a) << 24;
    result |= func(b) << 16;
    result |= func(c) << 8;
    result |= func(d);

    return result;
}

void gost::start(std::string inputFile, std::string outputFile, ushort mode)
{

//    ОСНОВНОЙ ЦИКЛ

    buffQueue = new std::queue<std::pair<std::uint32_t,std::uint32_t>>;
    this->inputFile = inputFile;
    this->mode = mode;
    output.open(outputFile, std::ios_base::binary);
    std::thread([this](){reading();}).detach();
    std::thread([this](){crypt();}).join();
    delete buffQueue;
    buffQueue = nullptr;
}

void gost::crypt()
{
    size_t round = 0;

    std::pair<std::uint32_t, std::uint32_t> chank;

    while(true)
    {
        std::unique_lock<std::mutex> lk(queueMutex);
        cv.wait(lk,[this](){return !buffQueue->empty();});
        auto x = buffQueue->front();
        buffQueue->pop();

        chank = x;

        for (round = 0; round < 32;)
           chank = oneStepCrypto(chank, round);


        write(chank);

        if(buffQueue->empty() && complete)
            return;
    }
    output.close();

}

void gost::reading()
{
    std::ifstream file(this->inputFile, std::ios_base::binary);
    std::uint8_t buf[8];
    std::uint32_t lower,higher;
    bool _complete = false;


    size_t count = 0;

    std::uintmax_t readed = 0;

    while(true)
    {
        if (!file.read(reinterpret_cast<char*>(&buf[count]),1) || file.eof())
        {
            if (count == 0)
                count = 7;
            break;
        }

        if (count == 7)
        {
            lower = concat(buf[0],buf[1],buf[2],buf[3]);
            higher = concat(buf[4],buf[5],buf[6],buf[7]);
            queueMutex.lock();
            buffQueue->push(std::make_pair(lower,higher));
            queueMutex.unlock();
            cv.notify_one();
            count = 0;
        }
        else
            count++;
    }

    if (count < 7)
    {
        while(count < 7)
            buf[count++] = 0x00;
        lower = concat(buf[0],buf[1],buf[2],buf[3]);
        higher = concat(buf[4],buf[5],buf[6],buf[7]);
        queueMutex.lock();
        buffQueue->push(std::make_pair(lower,higher));
        queueMutex.unlock();
        cv.notify_one();
        count = 0;
    }

    complete = true;

    file.close();
}

std::uint32_t gost::concat(std::uint8_t arr[4])
{
    std::uint32_t result = 0x00000000;
    for (size_t i = 0; i < 4; i++)
        result |= (static_cast<std::uint32_t>(arr[i]) << (8 * (3 - i)));
    return result;
}

void gost::write(std::pair<std::uint32_t, std::uint32_t> x)
{
    unsigned char *arr = reinterpret_cast<unsigned char*>(&x.first);

    for (int i = 3; i >= 0; i--)
        output << arr[i];

    arr = reinterpret_cast<unsigned char*>(&x.second);

    for (int i = 3; i >= 0; i--)
        output << arr[i];

}

std::pair<std::uint32_t, std::uint32_t> gost::oneStepCrypto(std::pair<std::uint32_t, std::uint32_t> buf, size_t &round)
{
    std::uint32_t lower, high; //Блок входных данных, разбитых на две половины

    unsigned char* byteBuffer;//Указатель для табля побайтовой работы с таблицой

    lower = buf.first;
    high = buf.second;

    if (this->mode == GAMMA)
    {
        lower = (lower + 0x01010104) % 4294967296;
        high = (high + 0x01010104) % 4294967296;
    }

    if (this->mode == SIMPLECRYPT)
        lower = (lower +  (round < 24 ? key[round % 8] : key[31 - round])) % 4294967296;
    else if (this->mode == SIMPLEENCRYPT)
        lower = (lower  +  (round < 8? key[round] : key[7 - (round - 8) % 8])) % 4294967296;

    byteBuffer = reinterpret_cast<unsigned char*>(&lower);

    //ЗАМЕНА ПО ТАБЛИЦЕ

    for (size_t i = 0; i < 4; i++)
    {
        byteBuffer[i] = (((table[2 * i][byteBuffer[i] & 0x0f]) << 4) | (table[2 * i + 1][byteBuffer[i] >> 4]));
        byteBuffer[i] = (byteBuffer[i] << 4) | (byteBuffer[i] >> 4);
    }
#ifdef __linux__
    asm("roll %1, %0" : "+g" (lower) : "I" ((unsigned char)11));
#endif

#ifdef _WIN32
    lower = _rotl(lower,11);
#endif

    lower ^= high;
    return (round++ < 31? (std::make_pair(lower, buf.first)) : (std::make_pair(buf.first, lower)));
}

gost::~gost()
{
    if (output.is_open())
        output.close();

    if (buffQueue != nullptr)
        delete buffQueue;

}
