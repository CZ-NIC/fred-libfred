/*
 * Copyright (C) 2011  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @decimal.hh
 *  decimal floating point type wrapper
 */

#ifndef DECIMAL_HH_A18DFE7A4A94487A8508AC39F9CE7C77
#define DECIMAL_HH_A18DFE7A4A94487A8508AC39F9CE7C77

#include <mpdecimal.h>

#include <boost/operators.hpp>
#include <boost/lexical_cast.hpp>

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <string>
#include <typeinfo>

class decimal_exception_handler
{
public:
    virtual ~decimal_exception_handler() {}

    decimal_exception_handler(
            const char* _message_prefix,
            bool _rethrow = true)
        : message_prefix(_message_prefix),
          rethrow(_rethrow)
    {}
    void operator ()()
    {
        try
        {
            throw;
        }
        catch (const std::exception &e)
        {
            if (rethrow)
            {
                throw std::runtime_error(message_prefix + " std::exception: " + e.what());
            }
        }
        catch (...)
        {
            if (rethrow)
            {
                throw std::runtime_error(message_prefix + " unknown exception");
            }
        }
    }//operator()
private:
    std::string message_prefix;
    bool rethrow;
};

template <class T>
std::string decode_status(const T arg)
{
    std::string ret;
    if (arg & MPD_Clamped) { ret += " MPD_Clamped "; }
    if (arg & MPD_Conversion_syntax) { ret += " MPD_Conversion_syntax "; }
    if (arg & MPD_Division_by_zero) { ret += " MPD_Division_by_zero "; }
    if (arg & MPD_Division_impossible) { ret += " MPD_Division_impossible "; }
    if (arg & MPD_Division_undefined) { ret += " MPD_Division_undefined "; }
    if (arg & MPD_Fpu_error) { ret += " MPD_Fpu_error "; }
    if (arg & MPD_Inexact) { ret += " MPD_Inexact "; }
    if (arg & MPD_Invalid_context) { ret += " MPD_Invalid_context "; }
    if (arg & MPD_Invalid_operation) { ret += " MPD_Invalid_operation "; }
    if (arg & MPD_Malloc_error) { ret += " MPD_Malloc_error "; }
    if (arg & MPD_Not_implemented) { ret += " MPD_Not_implemented "; }
    if (arg & MPD_Overflow) { ret += " MPD_Overflow "; }
    if (arg & MPD_Rounded) { ret += " MPD_Rounded "; }
    if (arg & MPD_Subnormal) { ret += " MPD_Subnormal "; }
    if (arg & MPD_Underflow) { ret += " MPD_Underflow "; }
    //if (arg & MPD_Max_status) { ret += " MPD_Max_status "; }
    return ret;
}

class dealocate_mpd_memory
{
public:
    dealocate_mpd_memory()
        : ptr_(nullptr)
    {}
    dealocate_mpd_memory(char* ptr)
        : ptr_(ptr)
    {}
    dealocate_mpd_memory(const dealocate_mpd_memory&) = delete;
    dealocate_mpd_memory& operator=(const dealocate_mpd_memory&) = delete;

    ~dealocate_mpd_memory()
    {
        this->release_ptr();
    }

    char* get_ptr()
    {
        return ptr_;
    }

    void set_ptr(char* ptr)
    {
        release_ptr();
        ptr_ = ptr;
    }
private:
    void release_ptr()
    {
        if (ptr_ != nullptr)
        {
            mpd_free(ptr_);
            ptr_ = nullptr;
        }
    }
    char* ptr_;
};

static std::string string_dump_mpd_context(const mpd_context_t* ctx)
{
    std::ostringstream sstr;
    sstr << "mpdecimal mpd_context dump -- precision: " << ctx->prec
         << " max positive exp: " << ctx->emax
         << " min negative exp: " << ctx->emin
         << " status events that should be trapped: " << ctx->traps
         << decode_status(ctx->traps)
         << " status flags: " << ctx->status
         << decode_status(ctx->status)
         << " newtrap: " << ctx->newtrap
         << decode_status(ctx->newtrap)
         << " rounding mode: " << ctx->round
         << " clamp mode: " << ctx->clamp
         << " all functions correctly rounded: " << ctx->allcr;
    return sstr.str();
}

//compile time type check of types supported by ctor template
template <typename>
struct supported_types_of_Decimal_ {};

template <>
struct supported_types_of_Decimal_<const std::string&>
{
    static void Type(const std::string&) { }
};

template <>
struct supported_types_of_Decimal_<std::string>
{
    static void Type(const std::string&) { }
};

template <>
struct supported_types_of_Decimal_<const char*>
{
    static void Type(const char* str_ptr)
    {
        if (str_ptr == nullptr)
        {
            throw std::runtime_error("Decimal string pointer is nullptr");
        }
    }
};

class Decimal : boost::operators<Decimal>
{
public:
    Decimal()
        : traphandler(nullptr),
          pvalue(nullptr)
    {
        try
        {
            this->init(default_precision);
            this->alloc_pvalue();
            this->set_string("");
        }
        catch (...)
        {
            decimal_exception_handler("Decimal() ctor exception", true)();
        }
    }

    template <class T>
    Decimal(T  t)
        : traphandler(nullptr),
          pvalue(nullptr)
    {
        try
        {
            supported_types_of_Decimal_<T>::Type(t);

            this->init(default_precision);
            this->alloc_pvalue();
            this->set_string(t);
        }
        catch (...)
        {
            decimal_exception_handler("Decimal( T t ) ctor exception", true)();
        }
    }

    Decimal(const Decimal& param)
        : traphandler(nullptr),
          pvalue(nullptr)
    {
        try
        {
            this->init(param.ctx.prec);
            ctx = param.ctx;
            pvalue = mpd_qncopy(param.pvalue);
            if (pvalue == nullptr)
            {
                check_status_exception(&ctx, param.pvalue, MPD_Malloc_error);
            }
        }
        catch (...)
        {
            decimal_exception_handler("Decimal(const Decimal& param ) ctor exception", true)();
        }
    }

    Decimal& operator=(const Decimal& param)
    {
        try
        {
            if (this != &param)
            {
              Decimal(param).swap(*this);
            }
        }
        catch (...)
        {
            decimal_exception_handler("Decimal& operator=(const Decimal& param) exception", true)();
        }
        return *this;
    }

    Decimal(const char* str, unsigned precision)
        : traphandler(nullptr),
          pvalue(nullptr)
    {
        try
        {
            this->init(precision);
            this->alloc_pvalue();
            this->set_string(str);
        }
        catch (...)
        {
            constexpr std::size_t max_msg_len = 2048;
            char msg[max_msg_len] = { '\0' };
            snprintf(msg,
                     max_msg_len,
                     "Decimal(const char *str: %s, const std::size_t precision: %u) ctor exception",
                     str,
                     precision);
            decimal_exception_handler(msg, true)();
        }
    }

    ~Decimal()
    {
        if (pvalue != nullptr)
        {
            mpd_del(pvalue);
        }
    }

    void swap(Decimal& dec) //throw()
    {
        std::swap(this->traphandler, dec.traphandler);
        std::swap(this->ctx, dec.ctx);
        std::swap(this->pvalue, dec.pvalue);
    }

    unsigned long get_precision()
    {
        return mpd_getprec(&ctx);
    }

    void set_string(const char* str)
    {
        try
        {
            if (str == nullptr)
            {
                throw std::runtime_error("string pointer is nullptr");
            }

            uint32_t status = 0;
            mpd_qset_string(pvalue, str, &ctx, &status);
            check_status_exception(&ctx, pvalue, status);
        }
        catch (...)
        {
            constexpr unsigned long max_msg_len = 2048;
            char msg[max_msg_len] = { '\0' };
            snprintf(msg,
                     max_msg_len,
                     "set_string(const char *str: %s ) exception",
                     str);
            decimal_exception_handler(msg, true)();
        }
    }

    void set_string(const std::string& str)
    {
        set_string(str.c_str());
    }

    bool is_special()const
    {
        return mpd_isspecial(pvalue);
    }

    bool is_nan()const
    {
        return mpd_isnan(pvalue);
    }

    bool is_zero()const
    {
        return mpd_iszero(pvalue);
    }

    bool is_infinite()const
    {
        return mpd_isinfinite(pvalue);
    }

    bool is_negative()const
    {
        return mpd_isnegative(pvalue);
    }

    std::string get_string()const
    {
        try
        {
            if (this->is_special())
            {
                return std::string();
            }
            //simple
            dealocate_mpd_memory decstring;
            decstring.set_ptr(mpd_to_sci(pvalue, 1));
            if (decstring.get_ptr() != nullptr)
            {
                return std::string(decstring.get_ptr());
            }
        }
        catch (...)
        {
            decimal_exception_handler("get_string() exception", true)();
        }
        return std::string();
    }

    //format string fmt syntax is the same as in Python PEP 3101 Standard Format Specifiers
    std::string get_string(const char* fmt)const
    {
        try
        {
            if (this->is_special())
            {
                return std::string();
            }
            uint32_t status = 0;
            dealocate_mpd_memory tmp_ret(mpd_qformat(pvalue, fmt, &ctx, &status));
            check_status_exception(&ctx, pvalue, status);
            if (tmp_ret.get_ptr() != nullptr)
            {
                return std::string(tmp_ret.get_ptr());
            }
        }
        catch (...)
        {
            decimal_exception_handler("get_string(const char* fmt) exception", true)();
        }
        return std::string();
    }

    bool operator<(const Decimal& x)const
    {
        bool is_lesser = false;
        try
        {
            Decimal result;
            uint32_t status = 0;

            if (this->is_special())
            {
                throw std::runtime_error("value is special");
            }
            if (x.is_special())
            {
                throw std::runtime_error("argument is special");
            }
            is_lesser = mpd_qcompare(result.pvalue, pvalue, x.pvalue, &ctx, &status) == -1;
            check_status_exception(&ctx, pvalue, status);

            if (result.is_special())
            {
                throw std::runtime_error("result is special");
            }
        }
        catch (...)
        {
            decimal_exception_handler("operator< exception", true)();
        }
        return is_lesser;
    }

    bool operator==(const Decimal& x)const
    {
        bool is_equal = false;
        try
        {
            Decimal result;
            uint32_t status = 0;

            if (this->is_special())
            {
                throw std::runtime_error("value is special");
            }
            if (x.is_special())
            {
                throw std::runtime_error("argument is special");
            }
            is_equal = mpd_qcompare(result.pvalue, pvalue, x.pvalue, &ctx, &status) == 0;
            check_status_exception(&ctx, pvalue, status);

            if (result.is_special())
            {
                throw std::runtime_error("result is special");
            }
        }
        catch (...)
        {
            decimal_exception_handler("operator== exception", true)();
        }
        return is_equal;
    }

    Decimal& operator+=(const Decimal& x)
    {
        try
        {
            Decimal result;
            uint32_t status = 0;

            if (this->is_special())
            {
                throw std::runtime_error("value is special");
            }
            if (x.is_special())
            {
                throw std::runtime_error("argument is special");
            }
            mpd_qadd(result.pvalue, pvalue, x.pvalue, &ctx, &status);
            check_status_exception(&ctx, pvalue, status);

            if (result.is_special())
            {
                throw std::runtime_error("result is special");
            }
            *this = result;
        }
        catch (...)
        {
            decimal_exception_handler("operator+= exception", true)();
        }
        return *this;
    }

    Decimal& operator-=(const Decimal& x)
    {
        try
        {
            Decimal result;
            uint32_t status = 0;

            if (this->is_special())
            {
                throw std::runtime_error("value is special");
            }
            if (x.is_special())
            {
                throw std::runtime_error("argument is special");
            }
            mpd_qsub(result.pvalue, pvalue, x.pvalue, &ctx, &status);
            check_status_exception(&ctx, pvalue, status);

            if (result.is_special())
            {
                throw std::runtime_error("result is special");
            }
            *this = result;
        }
        catch (...)
        {
            decimal_exception_handler("operator-= exception", true)();
        }
        return *this;
    }

    Decimal& operator*=(const Decimal& x)
    {
        try
        {
            Decimal result;
            uint32_t status = 0;

            if (this->is_special())
            {
                throw std::runtime_error("value is special");
            }
            if (x.is_special())
            {
                throw std::runtime_error("argument is special");
            }
            mpd_qmul(result.pvalue, pvalue, x.pvalue, &ctx, &status);
            check_status_exception(&ctx, pvalue, status);

            if (result.is_special())
            {
                throw std::runtime_error("result is special");
            }
            *this = result;
        }
        catch (...)
        {
            decimal_exception_handler("operator*= exception", true)();
        }
        return *this;
    }

    Decimal& operator/=(const Decimal& x)
    {
        try
        {
            Decimal result;
            uint32_t status = 0;

            if (this->is_special())
            {
                throw std::runtime_error("value is special");
            }
            if (x.is_special())
            {
                throw std::runtime_error("argument is special");
            }
            mpd_qdiv(result.pvalue, pvalue, x.pvalue, &ctx, &status);
            check_status_exception(&ctx, pvalue, status);

            if (result.is_special())
            {
                throw std::runtime_error("result is special");
            }
            *this = result;
        }
        catch (...)
        {
            decimal_exception_handler("operator/= exception", true)();
        }
        return *this;
    }

    Decimal& integral_division(const Decimal& x)
    {
        try
        {
            Decimal result;
            uint32_t status = 0;

            if (this->is_special())
            {
                throw std::runtime_error("value is special");
            }
            if (x.is_special())
            {
                throw std::runtime_error("argument is special");
            }
            mpd_qdivint(result.pvalue, pvalue, x.pvalue, &ctx, &status);
            check_status_exception(&ctx, pvalue, status);

            if (result.is_special())
            {
                throw std::runtime_error("result is special");
            }
            *this = result;
        }
        catch (...)
        {
            decimal_exception_handler("integral_division exception", true)();
        }
        return *this;
    }

    Decimal& integral_division_remainder(const Decimal& x)
    {
        try
        {
            Decimal result;
            uint32_t status = 0;

            if (this->is_special())
            {
                throw std::runtime_error("value is special");
            }
            if (x.is_special())
            {
                throw std::runtime_error("argument is special");
            }
            mpd_qrem(result.pvalue, pvalue, x.pvalue, &ctx, &status);
            check_status_exception(&ctx, pvalue, status);

            if (result.is_special())
            {
                throw std::runtime_error("result is special");
            }
            *this = result;
        }
        catch (...)
        {
            decimal_exception_handler("integral_division_remainder exception", true)();
        }
        return *this;
    }

    Decimal& round_half_up(long precision_places)
    {
        try
        {
            Decimal result(*this);//copy

            if (result.is_special())
            {
                throw std::runtime_error("value is special");
            }
            int saved_context_round = mpd_getround(&(result.ctx));

            if (mpd_qsetround(&(result.ctx), MPD_ROUND_HALF_UP) == 0)
            {
                throw std::runtime_error(
                    "Decimal::round_half_up unable to set context_newround");
            }
            const std::string new_string =
                    result.get_string(("." + boost::lexical_cast<std::string>(precision_places) + "f").c_str());

            if (mpd_qsetround(&(result.ctx), saved_context_round) == 0)
            {
                throw std::runtime_error(
                    "Decimal::round_half_up unable to set saved_context_round");
            }
            result.set_string(new_string.c_str());

            if (result.is_special())
            {
                throw std::runtime_error("result is special");
            }
            this->swap(result);
        }
        catch (...)
        {
            decimal_exception_handler("round_half_up() exception", true)();
        }
        return *this;
    }

    Decimal& abs()
    {
        try
        {
            Decimal result;
            uint32_t status = 0;

            if (this->is_special())
            {
                throw std::runtime_error("value is special");
            }
            mpd_qabs(result.pvalue, pvalue, &ctx, &status);
            check_status_exception(&ctx, pvalue, status);

            if (result.is_special())
            {
                throw std::runtime_error("result is special");
            }
            *this = result;
        }
        catch (...)
        {
            decimal_exception_handler("abs() exception", true)();
        }
        return *this;
    }

/*
    operator std::string() const
    {
        return (is_special() ? std::string("") : get_string());
    }
*/
private:
    //throw exception from trap handler
    //with context and value
    void check_status_exception(
            const mpd_context_t *ctx,
            const mpd_t *pvalue,
            uint32_t flags)const
    {
        mpd_context_t ctx_copy = *ctx;

        ctx_copy.status |= flags;
        if (flags & ctx->traps)
        {
            ctx_copy.newtrap = (flags & ctx_copy.traps);
            traphandler(&ctx_copy, pvalue);
        }
    }

    void init(const std::size_t precision)
    {
        struct ExTraphandler
        {
            static void fnc(const mpd_context_t* ctx, const mpd_t* pvalue)
            {
                std::string value;//in pvalue
                if (pvalue == nullptr)
                {
                    value = "pvalue is null";//pvalue might be 0
                }
                else if (pvalue->len == 0)
                {
                    value = "pvalue is empty";//pvalue might be empty
                }
                else
                {
                    value = mpd_to_sci(pvalue, 1);//pvalue might have value
                }
                throw std::runtime_error(string_dump_mpd_context(ctx) + " pvalue: " + value );
            }
        };
        //throw exception with context and value
        traphandler = ExTraphandler::fnc;

        //set default context configuration
        mpd_defaultcontext(&ctx);

        //set traps
        if (mpd_qsettraps(&ctx, MPD_Invalid_context) == 0)
        {
            throw std::runtime_error("MPD_Invalid_context > MPD_Max_status");
        }
        if (mpd_qsettraps(&ctx, MPD_Malloc_error) == 0)
        {
            throw std::runtime_error("MPD_Malloc_error > MPD_Max_status");
        }
        if (mpd_qsettraps(&ctx, MPD_Conversion_syntax) == 0)
        {
            throw std::runtime_error("MPD_Conversion_syntax > MPD_Max_status");
        }
        if (mpd_qsettraps(&ctx, MPD_Invalid_operation) == 0)
        {
            throw std::runtime_error("MPD_Invalid_operation > MPD_Max_status");
        }
        if (mpd_qsettraps(&ctx, MPD_Division_undefined) == 0)
        {
            throw std::runtime_error("MPD_Division_undefined > MPD_Max_status");
        }
        if (mpd_qsettraps(&ctx, MPD_Division_by_zero) == 0)
        {
            throw std::runtime_error("MPD_Division_by_zero > MPD_Max_status");
        }
        //set precision
        if (!mpd_qsetprec(&ctx, precision))
        {
            check_status_exception(&ctx, pvalue, MPD_Invalid_context);
            return;
        }

        //init alloc only once; NOT thread safe
        static int init_is_set = 0;
        if (init_is_set == 0)
        {
            mpd_setminalloc(MPD_MINALLOC_MAX);
            init_is_set = 1;
        }
    }

    void alloc_pvalue()
    {
        if (pvalue == nullptr)
        {
            pvalue = mpd_qnew();
            if (pvalue == nullptr)
            {
                if (mpd_qsettraps(&ctx, MPD_Malloc_error) == 0)
                {
                    throw std::runtime_error("MPD_Malloc_error > MPD_Max_status");
                }
                check_status_exception(&ctx, pvalue, MPD_Malloc_error);
            }
        }
        else
        {
            throw std::runtime_error("alloc_pvalue: pvalue not 0");
        }
    }

    friend std::ostream& operator<<(std::ostream& stream, Decimal d)
    {
        try
        {
            if (d.is_special())
            {
                throw std::runtime_error("argument is special");
            }
            stream << d.get_string();
        }
        catch (...)
        {
            decimal_exception_handler("operator<< exception", true)();
        }
        return stream;
    }

    friend std::istream& operator>>(std::istream& stream, Decimal &d)
    {
        try
        {
            std::string tmp_str;
            stream >> tmp_str;
            d.set_string(tmp_str);
        }
        catch (...)
        {
            decimal_exception_handler("operator>> exception", true)();
        }
        return stream;
    }

    //postgresql numeric type limit
    static constexpr std::size_t default_precision = 1000;
    void (*traphandler)(const mpd_context_t*, const mpd_t*);
    mpd_context_t ctx;
    mpd_t* pvalue;
};

#endif
