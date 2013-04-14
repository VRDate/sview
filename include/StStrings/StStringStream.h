/**
 * Copyright © 2011 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StStringStream_h__
#define __StStringStream_h__

#include <stTypes.h>

#include <sstream>
#include <locale>

#ifndef _MSC_VER
    #include <xlocale.h>
#endif

/**
 * Wrapper over locale_t C structure which should be allocated within special functions.
 * Notice that there NO implicit convertion from/to std::locale class!
 */
class StCLocale {

        public:

    /**
     * Default constructor for C locale.
     */
    StCLocale()
    #ifdef _MSC_VER
    : myCLocale(_create_locale(LC_ALL, "C")) {}
    #else
    : myCLocale(newlocale(LC_ALL_MASK, "C", NULL)) {}
    #endif

    /**
     * Destructor.
     */
    ~StCLocale() {
    #ifdef _MSC_VER
        _free_locale(myCLocale);
    #else
        freelocale(myCLocale);
    #endif
    }

#ifdef _MSC_VER
    operator _locale_t() const { return myCLocale; }
#else
    operator  locale_t() const { return myCLocale; }
#endif
        private:

#ifdef _MSC_VER
    _locale_t myCLocale;
#else
     locale_t myCLocale;
#endif

};

inline double stStringToDouble(const char*      theString,
                               char**           theNextPtr,
                               const StCLocale& theCLocale) {
#ifdef _MSC_VER
    return _strtod_l(theString, theNextPtr, theCLocale);
#else
    return  strtod_l(theString, theNextPtr, theCLocale);
#endif
}

inline double stStringToDouble(const char*      theString,
                               const StCLocale& theCLocale) {
    return stStringToDouble(theString, NULL, theCLocale);
}

inline long stStringToLong(const char*      theString,
                           char**           theNextPtr,
                           const int        theBase,
                           const StCLocale& theCLocale) {
#ifdef _MSC_VER
    return _strtol_l(theString, theNextPtr, theBase, theCLocale);
#else
    return  strtol_l(theString, theNextPtr, theBase, theCLocale);
#endif
}

inline long stStringToLong(const char*      theString,
                           const int        theBase,
                           const StCLocale& theCLocale) {
    return stStringToLong(theString, NULL, theBase, theCLocale);
}

/**
 * This class defines own std::stringstream template.
 * The main reason for replacement is that original template can not be easily
 * used over alien string buffer.
 *
 * This is the class draft which will be improved when the usage scenarios
 * will be defined. Currently it defined just to read numbers from strings
 * using C locale.
 */
class StStringStream : public std::basic_iostream< char, std::char_traits<char> > {

        private:

    /**
     * Defines simple stream buffer which should be initialized over alien buffer.
     */
    typedef std::basic_iostream< char, std::char_traits<char> > base;
    class IStreamBuf : public std::basic_streambuf< char, std::char_traits<char> > {

            public:

        IStreamBuf(char* theBuffer, std::streamsize theBufferLength) {
            setupInBuffer(theBuffer, theBufferLength);
        }

        void setupInBuffer(char* theBuffer, std::streamsize theBufferLength) {
            setg(theBuffer, theBuffer, theBuffer + theBufferLength);
        }

    } myStringBuffer;

        public:

    /**
     * Initialize the empty stream.
     */
    StStringStream()
    : base(&myStringBuffer),
      myStringBuffer(NULL, 0) {
        //
    }

    /**
     * Destructor.
     */
    virtual ~StStringStream() {}

    /**
     * Use C locale for numbers interpretations instead of global locale.
     * This is preferred for system-independent format.
     */
    void setCLocale() {
        imbue(std::locale("C"));
    }

    /**
     * Setup alien buffer for reading.
     */
    void setInputBuffer(char* theBuffer, std::streamsize theBufferLength) {
        myStringBuffer.setupInBuffer(theBuffer, theBufferLength);
        clear(); // reset flags
    }

};

#endif //__StStringStream_h__
