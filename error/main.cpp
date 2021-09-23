#include <iostream>
#include <typeinfo>
#include <cassert>
#include <chrono>

using namespace std;
constexpr size_t ITERATIONS = 1000000;

class err_t
{
public:
    virtual std::string message() const = 0;
    virtual int code() const = 0;
};

class io_err_t : public err_t
{
public:
    virtual std::string message() const override
    {
        return "generic io error";
    }

    virtual int code() const override
    {
        return EPERM;
    }
};

class dns_err_t : public err_t
{
public:
    virtual std::string message() const override
    {
        return "server is unavailable";
    }

    virtual int code() const override
    {
        return -1;
    }
};

class http_err_t : public err_t
{
public:
    virtual std::string message() const override
    {
        return "unauthorized";
    }

    virtual int code() const override
    {
        return 401;
    }
};

void test_dynamic_cast()
{
    const io_err_t io_err;
    const err_t *io_ptr = &io_err;

    if (auto err_ptr = dynamic_cast<const http_err_t *>(io_ptr))
    {
        assert(!"invalid match");
    }
    else if (auto err_ptr = dynamic_cast<const io_err_t *>(io_ptr))
    {
        assert(typeid(*io_ptr) == typeid(io_err));
        assert(typeid(*err_ptr) == typeid(io_err));
        assert(typeid(*err_ptr) == typeid(io_err_t));
    }
}

void test_dynamic_typeinfo()
{
    const io_err_t io_err;
    const http_err_t http_err;

    const err_t &io_ref = io_err;

    assert(typeid(io_ref) == typeid(io_err));
    assert(typeid(io_ref) == typeid(io_err_t));
    assert(typeid(io_ref) != typeid(err_t));
}

template <typename T>
inline constexpr const T* to_err(const err_t* base) {
    return dynamic_cast<const T*>(base);
}

void benchmark_dynamic_cast()
{
    const io_err_t io_err;
    const http_err_t http_err;
    const dns_err_t dns_err;

    auto start = chrono::steady_clock::now();
    const err_t *errors[] = {&io_err, &http_err, &dns_err};

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        for (size_t j = 0; j < sizeof(errors) / sizeof(*errors); j++)
        {
            const err_t *err_ptr = errors[j];

            if (auto err = to_err<http_err_t>(err_ptr))
            {
                int code = err->code();
            }
            else if (auto err = to_err<io_err_t>(err_ptr))
            {
                int code = err->code();
            }
            else if (auto err = to_err<dns_err_t>(err_ptr))
            {
                int code = err->code();
            }
            else
            {
                assert(!"failed");
            }
        }
    }

    auto total = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start).count();
    auto per_iteration = static_cast<double>(total) / ITERATIONS / (sizeof(errors) / sizeof(*errors));
    cout << __FUNCTION__ << ": " << total << " uS"
         << ", " << per_iteration << " uS" << endl;
}

void benchmark_typeinfo()
{
    const io_err_t io_err;
    const http_err_t http_err;
    const dns_err_t dns_err;

    auto start = chrono::steady_clock::now();
    const err_t *errors[] = {&io_err, &http_err, &dns_err};

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        for (size_t j = 0; j < sizeof(errors) / sizeof(*errors); j++)
        {
            const err_t &err_ref = *errors[j];

            if (typeid(err_ref) == typeid(http_err_t))
            {
                const auto &err = dynamic_cast<const http_err_t &>(err_ref);
                int code = err.code();
            }
            else if (typeid(err_ref) == typeid(io_err_t))
            {
                const auto &err = dynamic_cast<const io_err_t &>(err_ref);
                int code = err.code();
            }
            else if (typeid(err_ref) == typeid(dns_err_t))
            {
                const auto &err = dynamic_cast<const dns_err_t &>(err_ref);
                int code = err.code();
            }
            else
            {
                assert(!"failed");
            }
        }
    }

    auto total = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start).count();
    auto per_iteration = static_cast<double>(total)  / ITERATIONS / (sizeof(errors) / sizeof(*errors));
    cout << __FUNCTION__ << ": " << total << " uS"
         << ", " << per_iteration << " uS" << endl;
}

int main()
{
    test_dynamic_cast();
    test_dynamic_typeinfo();
    benchmark_dynamic_cast();
    benchmark_typeinfo();
}